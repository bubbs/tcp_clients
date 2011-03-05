//// tcp_smtp
//
// SMTP (Simple Mail Transfer Protocol) implementation.
//
// SMTP daemon, simple interface and event API.

/// history
// v0.7; 23/Jul/2010: Added query_mesg_*(mesg_id) functions, revised sender name support.
// v0.6; 22/Jul/2010: Added SMTP time-out structure.
// v0.5; 21/Jul/2010: Debugged, first successfully sent email.
// v0.4; 19/Jul/2010: Persistance and unknown SMTP tracking.
// v0.3; 18/Jul/2010: Basic outgoing SMTP conversation, email validation and parsing.
// v0.2; 17/Jul/2010: send_mail() argument parsing and message queuing.
// v0.1; ??/May/2010: Basic structure.

/// to do;
// ! check out save/restore issues.

/// thoughts:
// ! incoming mail:
//   ? questionable, since it depends on other remote server having mail for the mud
//   ? From time to time, check with the SMTP server for new mail.
//   ? Sporadic 'check_mail()' call, to check the server for incoming messages.
//     On send_mail(), give the SMTP server a message to send.
//     Whenever there's a SMTP connection, re-use to send all queued outgoing messages.
//     Whenever there's a SMTP connection, conclude by inviting incoming messages.
// ? callback event SMTP_GET_NAME, to override the default "%N@%M" guff.
// ? ability to pause/cancel a message and/or SMTP engine.

/*
[connected to 'dune.servint.com 25']
 >|220 dune.servint.com ESMTP Sendmail 8.13.8/8.13.8; Sun, 18 Jul 2010 11:55:14 -0400
< |HELO dune.servint.com
 >|250 dune.servint.com Hello dune.servint.com [206.214.219.254], pleased to meet you
< |MAIL FROM: mud@dune.servint.com
 >|250 2.1.0 mud@dune.servint.com... Sender ok
< |RCPT TO: bubbs@eclipse.co.uk
 >|250 2.1.5 bubbs@eclipse.co.uk... Recipient ok
< |DATA
 >|354 Enter mail, end with "." on a line by itself
< |To: bubbs@eclipse.co.uk
< |From: Bubbs%Dune Mud <mud@dune.servint.com>
< |Subject: Test Mail
< |
< |Just a thought, does it work?
< |
< |Bubbs.
< |.
 >|250 2.0.0 o6IFtExG020250 Message accepted for delivery
< |RSET
< |250...
< |QUIT
 >|221 2.0.0 dune.servint.com closing connection
[connection closed]
*/


#pragma no_clone
#pragma no_shadow


virtual inherit __DIR__ "tcp_socket";


#include <tcp_smtp.h>


/// tcp_smtp state variables.

protected mapping  outgoing_messages  = 0;
//protected mapping  incoming_messages  = 0;

#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
protected mapping  unknown_smtp_codes = 0;
#endif

protected int      last_message_id    = 0;
protected int      current_message_id = 0;

nosave    int      current_state      = SMTP_NOT_CONNECTED;


/// tcp_smtp query method prototypes.

#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
public    mapping  query_unknown_smtp   ();
#endif

public    int      query_outgoing_count ();

public    int      query_last_mesg_id   ();
public    int      query_current_mesg_id();
public    int      query_state          ();

public    int      query_valid_email    (string address);

public    string   query_mesg_send_name (int mesg_id);
public    string*  query_mesg_recipients(int mesg_id);
public    string*  query_mesg_copy_to   (int mesg_id);
public    string*  query_mesg_blind_copy(int mesg_id);
public    string*  query_mesg_headers   (int mesg_id);
public    string   query_mesg_body      (int mesg_id);
public    mixed*   query_mesg_args      (int mesg_id);
public    int      query_mesg_rety_count(int mesg_id);

//public    int      query_mail           (string address);


/// tcp_smtp interface method prototypes.

public    int      send_mail            (varargs mixed* args);

//public    int*     check_mail           (string address);
//public    mixed*   collect_mail         (string address, int mesg_id);


/// tcp_smtp process method prototypes.

protected mixed    smtp_event           (int event, varargs mixed* args);
protected void     parse_smtp           (string message);
protected void     smtp_timeout         (int mesg_id, int state);


/// tcp_smtp utility method prototypes.

public    string   read_address         (string address);


#if defined(TEST_SMTP_METHODS) && !defined(NO_TEST_SMTP_METHODS)
/// tcp_smtp test harness method prototypes.

public    string  mail_file             (string address, string file, int options);
protected mixed   mail_file_callback    (int event, int mesg_id, object user, varargs mixed* args);
#endif


/// mudlib interface methods.

public void create() {
#if defined(SMTP_MAIL_SAVE_FILE) && !defined(NO_SMTP_MAIL_SAVE_FILE)
  if (restore_object(SMTP_MAIL_SAVE_FILE))
    return;
#endif

  outgoing_messages  = ([:MESSAGE_WIDTH ]);
  //incoming_messages  = ([:MESSAGE_WIDTH ]);
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
  unknown_smtp_codes = ([ ]);
#endif
}

public int deny_update() {
#if defined(SMTP_MAIL_SAVE_FILE) && !defined(NO_SMTP_MAIL_SAVE_FILE)
  return ::deny_update();
#else
  return ::deny_update() || sizeof(outgoing_messages);
#endif
}

public int clean_up(int flag) {
  return ::clean_up(flag);
}

public void destructor() {
#if defined(SMTP_MAIL_SAVE_FILE) && !defined(NO_SMTP_MAIL_SAVE_FILE)
  save_object(SMTP_MAIL_SAVE_FILE);
#endif
  ::destructor();
}


/// tcp_socket callback methods.

// called when the socket is initially connected.
protected void do_socket_connect() {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q::do_socket_connect(); %#Q %#Q\n", this_object(), query_host_address(), query_host_port()));
  smtp_event(SMTP_DEBUG, "[connected to '" +query_host_address() +"', port " +query_host_port() +"]\n");

  current_state = SMTP_CONNECTED;
  //parse_smtp("\n");
}

// called when the socket receives a message.
protected void do_socket_read(string message) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q::do_socket_read(%#Q);\n", this_object(), message));
  smtp_event(SMTP_DEBUG, implode(map(explode((message[<1] == '\n') ? message[..<2] : message, "\n"), (: sprintf(" >|%s\n", $1) :)), ""));

  parse_smtp(message);
}

// called when the socket has been sent a message.
protected void do_socket_write(string message) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q::do_socket_write(%#Q);\n", this_object(), message));
  smtp_event(SMTP_DEBUG, implode(map(explode((message[<1] == '\n') ? message[..<2] : message, "\n"), (: sprintf("< |%s\n", $1) :)), ""));
}

// called when the socket closes.
protected void do_socket_close() {
  smtp_event(SMTP_DEBUG, "[closing '" +query_host_address() +"', port " +query_host_port() +"]\n");

  current_state = 0;
}

// called when the socket fails to connect.
protected void do_socket_error(string error) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q::do_socket_error(%#Q);\n", this_object(), error));
  smtp_event(SMTP_DEBUG, "[error: '" +trim(error, TRIM_BOTH, "\r\n") +"]\n");

}


/// tcp_smtp query methods.

#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
public mapping query_unknown_smtp() { return deep_copy(unknown_smtp_codes); }
#endif

public int query_outgoing_count() {
  return sizeof(outgoing_messages);
}

public int query_last_mesg_id() {
  return last_message_id;
}

public int query_current_mesg_id() {
  return current_message_id;
}

public int query_state() {
  return current_state;
}

public int query_valid_email(string address) {
  return
    stringp(address) &&
    strlen(address) &&
    !strlen(regreplace(address, "^" +EMAIL_ADDRESS_REGEXP +"$", "", 0));
}

public string query_mesg_send_name(int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_SENDER_DISPLAY_NAME];
}

public string* query_mesg_recipients(int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_RECIPIENT_ADDRESSES];
}

public string* query_mesg_copy_to   (int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_COPY_ADDRESSES];
}

public string* query_mesg_blind_copy(int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_BLIND_COPY_ADDRESSES];
}

public string* query_mesg_headers   (int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_MESSAGE_HEADERS];
}

public string query_mesg_body      (int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_MESSAGE_BODY];
}

public mixed* query_mesg_args      (int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_EXTRA_ARGUMENTS];
}

public int query_mesg_rety_count(int mesg_id) {
  return
    member(outgoing_messages, mesg_id) &&
    (previous_object() == to_object(outgoing_messages[mesg_id, MESG_CALLBACK])) &&
    outgoing_messages[mesg_id, MESG_SEND_ATTEMPT_COUNT];
}


/// tcp_smtp interface methods.

// int send_mail("bubbs@eclipse.co.uk", "Mud Mail Message", "Hi from " +MUD_NAME\n\nAll the best.", (: map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("[%d] smtp_event %d: %#Q\n", $2, $1, $3)) :));
// int send_mail(string|string* recipient_address, [string* copy_to_addresses, [string* blind_copy_addresses, ]] [string subject,] string|string* message_body, closure callback);

public int send_mail(varargs mixed* args) {
  string* recipient_addresses  = 0;
  string* copy_to_addresses    = 0;
  string* blind_copy_addresses = 0;
  string  message_subject      = 0;
  string  message_body         = 0;
  closure event_callback       = 0;
  mixed*  extra_arguments      = ({ });

//printf("args: %#Q\n", args);

  int size = sizeof(args);
  for(int index = 0; index < size; index++) {
//printf("----------------\n");
//printf("args[%d]             : %#Q\n", index, args[index]);

    //if (!sender_address && () && stringp(args[index]))
    //  sender_address = args[index];
    //else
    if (!recipient_addresses && stringp(args[index]))
      recipient_addresses = explode(args[index], "\n");
    else if (!recipient_addresses && pointerp(args[index]))
      recipient_addresses = filter(args[index], #'stringp);
    else if (!copy_to_addresses && ((index +2) < size) && pointerp(args[index]))
      copy_to_addresses = filter(args[index], #'stringp);
    else if (!blind_copy_addresses && ((index +2) < size) && pointerp(args[index]))
      blind_copy_addresses = filter(args[index], #'stringp);
    else if (!message_subject && ((index +2) < size) && stringp(args[index]))
      message_subject = args[index];
    else if (!message_body && pointerp(args[index]))
      message_body = implode(filter(args[index], #'stringp), "\n");
    else if (!message_body && stringp(args[index]))
      message_body = args[index];
    //else if (!message_body && closurep(args[index]))
    //  message_body = funcall((: stringp($1) ? $1 : pointerp($1) ? implode(filter($1, #'stringp), "\n") : "" :), funcall(args[index]));
    else if (!event_callback && closurep(args[index]))
      event_callback = args[index];
    else if (event_callback)
      extra_arguments += ({ args[index], });
    else
      raise_error("Bad argument " +(index +1) +" type to tcp_smtp::send_mail()\n");
      //return 0;

//printf("sender_address      : %#Q\n", sender_address);
//printf("recipient_addresses : %#Q\n", recipient_addresses);
//printf("copy_to_addresses   : %#Q\n", copy_to_addresses);
//printf("blind_copy_addresses: %#Q\n", blind_copy_addresses);
//printf("message_subject     : %#Q\n", message_subject);
//printf("message_body        :\n%s\n", message_body));
//printf("event_callback      : %#Q\n", event_callback);
//printf("extra_arguments     : %#Q\n", extra_arguments);
  }

//printf("recipient_addresses : %#Q\n", recipient_addresses);
//printf("message_body && 1   : %#Q\n", message_body && 1);
//printf("event_callback      : %#Q\n", event_callback);
//printf("extra_arguments     : %#Q\n", extra_arguments);
  if (!sizeof(recipient_addresses) ||
      !message_body ||
      !event_callback)
    return 0;

  string  sender_name       = "";
  string  sender_address    = capitalize((this_player() && GET_USER_NAME(this_player())) || SMTP_SENDER_NAME(to_object(event_callback)) || UNKNOWN_SENDER_NAME);
  foreach(string bit : regexplode(SMTP_SENDER_DISPLAY_NAME, "\\$[$NM]"))
    switch(bit) {
      case "$$":  sender_name += "$";             break;
      case "$M":  sender_name += MUD_NAME;        break;
      case "$N":  sender_name += sender_address;  break;
      default  :  sender_name += bit;             break;
    }
  sender_address = sender_name +" <" +SMTP_SENDER_ADDRESS +">";
//printf("sender_name         : %#Q\n", sender_name);
//printf("sender_address      : %#Q\n", sender_address);

  string* recipient_emails  = filter(map(recipient_addresses, #'read_address), #'query_valid_email);
  string* copy_to_emails    = copy_to_addresses    && filter(map(copy_to_addresses, #'read_address), #'query_valid_email);
  string* blind_copy_emails = blind_copy_addresses && filter(map(blind_copy_addresses, #'read_address), #'query_valid_email);
//printf("recipient_emails    : %#Q\n", recipient_emails);

#if defined(VALID_EMAIL_CHECK) && !defined(NO_VALID_EMAIL_CHECK)
  closure valid_email_check = (: VALID_EMAIL_CHECK($2, $1) :);
//printf("recipient_emails    : %#Q\n", recipient_emails);
  recipient_emails  = filter(recipient_emails,  valid_email_check, this_player() || to_object(event_callback));
  copy_to_emails    = copy_to_addresses    && filter(copy_to_emails,    valid_email_check, this_player() || to_object(event_callback));
  blind_copy_emails = blind_copy_addresses && filter(blind_copy_emails, valid_email_check, this_player() || to_object(event_callback));
#endif

  if ((sizeof(recipient_emails)  != sizeof(recipient_addresses))  ||
//    (sizeof(copy_to_emails)    != sizeof(copy_to_addresses))    ||
//    (sizeof(blind_copy_emails) != sizeof(blind_copy_addresses)) ||
      0)
    return 0;

  string* message_headers =
    ({
      "From: "        +sender_address,
      "To: "          +implode(recipient_addresses, "; "),
      sizeof(copy_to_addresses) &&
        ("Copy-To: "     +implode(copy_to_addresses, ";")),
      message_subject &&
        ("Subject: "     +message_subject),
#if defined(SMTP_REPLY_TO_ADDRESS) && !defined(NO_SMTP_REPLY_TO_ADDRESS)
      "Reply-To: "    +SMTP_REPLY_TO_ADDRESS,
#endif
#if defined(SMTP_RETURN_PATH_ADDRESS) && !defined(NO_SMTP_RETURN_PATH_ADDRESS)
      "Return-Path: " +SMTP_RETURN_PATH_ADDRESS,
#endif
#if !defined(SMTP_X_MAILER_IDENTITY) && !defined(NO_SMTP_X_MAILER_IDENTITY)
      "X-Mailer: "    +SMTP_MAILER_IDENTITY,
#endif
    }) -({ 0, });

  int mesg_id = ++last_message_id;
  m_add(outgoing_messages, mesg_id);
  outgoing_messages[mesg_id, MESG_CALLBACK]             = event_callback;
  outgoing_messages[mesg_id, MESG_SENDER_DISPLAY_NAME]  = sender_name;
  outgoing_messages[mesg_id, MESG_RECIPIENT_ADDRESSES]  = recipient_emails;
  outgoing_messages[mesg_id, MESG_COPY_ADDRESSES]       = copy_to_emails;
  outgoing_messages[mesg_id, MESG_BLIND_COPY_ADDRESSES] = blind_copy_emails;
  outgoing_messages[mesg_id, MESG_MESSAGE_HEADERS]      = message_headers;
  outgoing_messages[mesg_id, MESG_MESSAGE_BODY]         = message_body;
  outgoing_messages[mesg_id, MESG_EXTRA_ARGUMENTS]      = extra_arguments;
  outgoing_messages[mesg_id, MESG_SEND_ATTEMPT_COUNT]   = 0;
  outgoing_messages[mesg_id, MESG_RCPT_INDEX]           = 0;

  (current_state == SMTP_NOT_CONNECTED) && parse_smtp("\n");

  return mesg_id;
}


/// tcp_smtp process methods.

protected mixed smtp_event(int event, varargs mixed* args) {
  return
    current_message_id &&
    //outgoing_messages[current_message_id, MESG_CALLBACK] &&
    apply(outgoing_messages[current_message_id, MESG_CALLBACK], event, current_message_id, outgoing_messages[current_message_id, MESG_EXTRA_ARGUMENTS] +args);
}

protected void parse_smtp(string message) {
  int    mesg_code = 0;
  string mesg_text = 0;
  sscanf(message, "%+U%t%+s\n", mesg_code, mesg_text);

  if (!current_message_id || !member(outgoing_messages, current_message_id)) {
    current_message_id = sizeof(outgoing_messages) && min(m_indices(outgoing_messages));
    current_message_id && smtp_event(SMTP_SENDING);
  }

  string smtp_error    = 0;
  string message_error = 0;
  int    quit_smtp     = 0;
  string socket_output = 0;
  int    close_mesg    = 0;

  int    send_index    = 0;
  switch(current_state) {
    case SMTP_SERVER_ERROR:
      // a server error occurred.
      smtp_error    = "SMTP Engine Error.\n";
    break;
    case SMTP_NOT_CONNECTED:
      // not connected.
      if (current_message_id) {
        smtp_error    = socket_connect(SMTP_SERVER_HOST, SMTP_SERVER_PORT, 0);
        current_state = SMTP_CONNECTING;
      }
      //else
      //  0;// what's the problem?
    break;
    case SMTP_CONNECTING:
      // attempting to connect.
      //if (current_message_id)
      //  1;// wait, be patient.
      //else
      //  0;// eh? odd, but no problem.
    break;
    case SMTP_CONNECTED:
      // connected.
      switch(mesg_code) {
        case 220:  // <domain> Service ready
          socket_output = "HELO " +SMTP_SERVER_HOST +"\n";
          current_state = SMTP_SAID_HELO;
        break;
        case 421:  // <domain> Service not available, closing transmission channel
          smtp_error    = "Connect error - " +message;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["Connect/" +mesg_code] = mesg_text;
#endif
          smtp_error    = "Connect error - " +message;
        break;
      }
    break;
    case SMTP_SAID_HELO:
      // said 'HELO'.
      switch(mesg_code) {
        case 250:  // Requested mail action okay, completed
          if (current_message_id) {
            socket_output = "MAIL FROM: " +SMTP_SENDER_ADDRESS +"\n";
            current_state = SMTP_SAID_MAIL;
          }
          else
            quit_smtp     = 1;
        break;
        case 500:  // Syntax error, command unrecognised
        case 501:  // Syntax error in parameters or arguments
        case 504:  // Command parameter not implemented
        case 521:  // <domain> does not accept mail [rfc1846]
        case 421:  // <domain> Service not available, closing transmission channel
          if (current_message_id)
            message_error = "HELO error - " +message;
          else
            quit_smtp     = 1;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["HELO/" +mesg_code] = mesg_text;
#endif
          if (current_message_id)
            message_error = "HELO error - " +message;
          else
            quit_smtp     = 1;
        break;
      }
    break;
    case SMTP_SAID_MAIL:
      // said 'MAIL FROM'
      switch(mesg_code) {
        case 250:  // Requested mail action okay, completed
          if (current_message_id) {
            outgoing_messages[current_message_id, MESG_RCPT_INDEX] = 1;
            socket_output = "RCPT TO: " +outgoing_messages[current_message_id, MESG_RECIPIENT_ADDRESSES][0] +"\n";
            current_state = SMTP_SAID_RCPT;
          }
          else
            quit_smtp     = 1;
        break;
        case 552:  // Requested mail action aborted: exceeded storage allocation
        case 451:  // Requested action aborted: local error in processing
        case 452:  // Requested action not taken: insufficient system storage
        case 500:  // Syntax error, command unrecognised
        case 501:  // Syntax error in parameters or arguments
        case 421:  // <domain> Service not available, closing transmission channel
          if (current_message_id)
            message_error = "MAIL error - " +message;
          else
            quit_smtp     = 1;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["MAIL/" +mesg_code] = mesg_text;
#endif
          if (current_message_id)
            message_error = "MAIL error - " +message;
          else
            quit_smtp     = 1;
        break;
      }
    break;
    case SMTP_SAID_RCPT:
      // said 'RCPT TO'
      switch(mesg_code) {
        case 250:  // Requested mail action okay, completed
          if (current_message_id) {
            send_index = outgoing_messages[current_message_id, MESG_RCPT_INDEX]++;
            if (send_index < sizeof(outgoing_messages[current_message_id, MESG_RECIPIENT_ADDRESSES]))
              socket_output = "RCPT TO: " +outgoing_messages[current_message_id, MESG_RECIPIENT_ADDRESSES][send_index] +"\n";
            else {
              socket_output = "DATA\n";
              current_state = SMTP_SAID_DATA;
            }
          }
          else
            quit_smtp     = 1;
        break;
        case 251:  // User not local; will forward to <forward-path>
          message_error = mesg_text +"\n";
        break;
        case 550:  // Requested action not taken: mailbox unavailable
        case 551:  // User not local; please try <forward-path>
        case 552:  // Requested mail action aborted: exceeded storage allocation
        case 553:  // Requested action not taken: mailbox name not allowed
        case 450:  // Requested mail action not taken: mailbox unavailable
        case 451:  // Requested action aborted: local error in processing
        case 452:  // Requested action not taken: insufficient system storage
        case 500:  // Syntax error, command unrecognised
        case 501:  // Syntax error in parameters or arguments
        case 503:  // Bad sequence of commands
        case 521:  // <domain> does not accept mail [rfc1846]
        case 421:  // <domain> Service not available, closing transmission channel
          if (current_message_id)
            message_error = "RCPT error - " +message;
          else
            quit_smtp     = 1;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["RCPT/" +mesg_code] = mesg_text;
#endif
          if (current_message_id)
            message_error = "RCPT error - " +message;
          else
            quit_smtp     = 1;
        break;
      }
    break;
    case SMTP_SAID_DATA:
      // said 'DATA'
      switch(mesg_code) {
        case 354:  // Start mail input; end with <CRLF>.<CRLF>
          if (current_message_id) {
            if (sizeof(outgoing_messages[current_message_id, MESG_MESSAGE_HEADERS])) {
              foreach(string header : outgoing_messages[current_message_id, MESG_MESSAGE_HEADERS])
                socket_write(((header[0] == '.') ? "." : "") +header +"\n");
              socket_write("\n");
            }
            foreach(string body : explode(outgoing_messages[current_message_id, MESG_MESSAGE_BODY], "\n"))
              socket_write(((body[0] == '.') ? "." : "") +body +"\n");
            socket_output = ".\n";
            current_state = SMTP_SAID_COMPLETE;
          }
          else
            quit_smtp     = 1;
        break;
        case 451:  // Requested action aborted: local error in processing
        case 554:  // Transaction failed
        case 500:  // Syntax error, command unrecognised
        case 501:  // Syntax error in parameters or arguments
        case 503:  // Bad sequence of commands
        case 421:  // <domain> Service not available, closing transmission channel
          if (current_message_id)
            message_error = "DATA error - " +message;
          else
            quit_smtp     = 1;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["DATA/" +mesg_code] = mesg_text;
#endif
          if (current_message_id)
            message_error = "DATA error - " +message;
          else
            quit_smtp     = 1;
        break;
      }
    break;
    case SMTP_SAID_COMPLETE:
      // said '.'
      switch(mesg_code) {
        case 250:  // Requested mail action okay, completed
          if (current_message_id) {
            smtp_event(SMTP_SUCCESS, trim(mesg_text, TRIM_BOTH, "\r\n"));
            close_mesg    = 1;
            if (sizeof(outgoing_messages) > 1) {
              socket_output = "RSET\n";
              current_state = SMTP_SAID_RESET;
            }
            else
              quit_smtp     = 1;
          }
          else
            quit_smtp     = 1;
        break;
        case 552:  // Requested mail action aborted: exceeded storage allocation
        case 554:  // Transaction failed
        case 451:  // Requested action aborted: local error in processing
        case 452:  // Requested action not taken: insufficient system storage
          if (current_message_id)
            message_error = "Complete error - " +message;
          else
            quit_smtp     = 1;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["Complete/" +mesg_code] = mesg_text;
#endif
          if (current_message_id)
            message_error = "Complete error - " +message;
          else
            quit_smtp     = 1;
        break;
      }
    break;
    case SMTP_SAID_RESET:
      // said 'RSET'
      switch(mesg_code) {
        case 200:  // (nonstandard success response, see rfc876)
        case 250:  // Requested mail action okay, completed
          if (current_message_id) {
            socket_output = "MAIL FROM: " +SMTP_SENDER_ADDRESS +"\n";
            current_state = SMTP_SAID_MAIL;
          }
          else
            quit_smtp     = 1;
        break;
        case 500:  // Syntax error, command unrecognised
        case 501:  // Syntax error in parameters or arguments
        case 504:  // Command parameter not implemented
        case 421:  // <domain> Service not available, closing transmission channel
          if (current_message_id)
            message_error = "RSET error - " +message;
          else
            quit_smtp     = 1;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["RSET/" +mesg_code] = mesg_text;
#endif
          smtp_error    = "RSET error - " +message;
        break;
      }
    break;
    case SMTP_SAID_QUIT:
      // said 'QUIT'
      switch(mesg_code) {
        case 221:  // Service closing transmission channel
          socket_close();
        break;
        case 500:  // Syntax error, command unrecognised
          smtp_error    = "QUIT error - " +message;
        break;
        default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
          if (mesg_code)
            unknown_smtp_codes["QUIT/" +mesg_code] = mesg_text;
#endif
          smtp_error    = "QUIT error - " +message;
        break;
      }
    break;
    default:
#if defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(NO_TRACK_UNKNOWN_SMTP_CODES)
      if (mesg_code)
        unknown_smtp_codes["Status[" +current_state +"]/" +mesg_code] = mesg_text;
      else
        unknown_smtp_codes["Status[" +current_state +"]"] = message;
#endif
      smtp_error    = "state[" +current_state +"] error - " +message;
    break;
  }

  if (smtp_error) {
    socket_close();
    message_error = smtp_error;
    current_state = SMTP_SERVER_ERROR;
  }
  else if (quit_smtp) {
    socket_output = "QUIT\n";
    current_state = SMTP_SAID_QUIT;
  }

  if (message_error) {
    smtp_event(SMTP_FAILED, message_error);
    close_mesg    = 1;
  }
  else if (socket_output)
#if defined(SLOW_SOCKET_WRITE) && !defined(NO_SLOW_SOCKET_WRITE)
    call_out(#'socket_write, SLOW_SOCKET_WRITE, output);
#else
    socket_write(socket_output);
#endif

  (find_call_out(#'smtp_timeout) != -1) && remove_call_out(#'smtp_timeout);
  if (close_mesg) {
    m_delete(outgoing_messages, current_message_id);
    current_message_id = 0;
  }
  else
#if defined(SLOW_SOCKET_WRITE) && !defined(NO_SLOW_SOCKET_WRITE)
    call_out(#'smtp_timeout, SLOW_SOCKET_WRITE +30, current_message_id, current_state);
#else
    call_out(#'smtp_timeout, 30, current_message_id, current_state);
#endif
}



protected void smtp_timeout(int mesg_id, int state) {
  if (!current_message_id || (mesg_id != current_message_id) || (state != current_state))
    return;

  socket_close();
  current_state = SMTP_SERVER_ERROR;
  smtp_event(SMTP_FAILED, "SMTP server timed out at state '" +state +"'.\n");
  m_delete(outgoing_messages, current_message_id);
  current_message_id = 0;
}


/// tcp_smtp utility method prototypes.

public string read_address(string address) {
  string email = 0;
  if (stringp(address) &&
      (sscanf(address, "%+~s<%+s>", email) != 2) &&
//      (sscanf(address, "(%+~s)%+s", email) != 2) &&
//      (sscanf(address, "%+s(%+~s)", email) != 2) &&
      1)
    email = address;

  return email && trim(email);
}


#if defined(TEST_SMTP_METHODS) && !defined(NO_TEST_SMTP_METHODS)
/// tcp_smtp test harness methods.

public string mail_file(string address, string file, int options) {
  if (!this_player())
    return "user not found.\n";

  if (!query_valid_email(address))
    return sprintf("invalid email address '%s'.\n", address);

#if 1
  if (!(master()->valid_read(file, GET_USER_NAME(this_player()), "read_file", this_player())))
    return sprintf("unreadable file '%s'.\n", file);
#endif
#if defined(EVAL_PATH)
  file = EVAL_PATH(file);
#endif
  string file_data = read_file(file);
  if (!file_data)
    return sprintf("file '%s' not found or unreadable.\n", file);

  int mesg_id = send_mail(address, file, file_data, #'mail_file_callback, this_player(), options);
  mesg_id && printf("Sending file '%s' to '%s' (message %d).\n", file, address, mesg_id);

  return !mesg_id && sprintf("unable to send file '%s'.\n", file);
}

protected mixed mail_file_callback(int event, int mesg_id, object user, int options, varargs mixed* args) {
  mixed  result = 0;
  string mesg   = 0;
//user && tell_object(user, sprintf("mesg %Q, event %#Q: %#Q\n", mesg_id, event, args));

  switch(event) {
    case SMTP_DEBUG:
      // data  : string debug_message;
      // return: void;
      // An SMTP debug message.
      mesg = (options &MFOPT_DEBUG) && args[0];
    break;
    case SMTP_FAILED:
      // data  : string error_message;
      // return: void;
      // The SMTP request failed.
      mesg = sprintf("Message %d send failed - %s\n", mesg_id, trim(args[0], TRIM_BOTH, "\r\n"));
    break;
//    case SMTP_PAUSED:
//      // data  : int request_paused;
//      // return: void;
//      // Pause callbacks.  Not implemented.
//    break;
    case SMTP_SENDING:
      // data  : void;
      // return: void;
      // The mail is now being sent.
      mesg = sprintf("Sending message %d...\n", mesg_id);
    break;
    case SMTP_SUCCESS:
      // data  : string sent_message;
      // return: void;
      // The mail was successfully sent.
      mesg = sprintf("Message %d sent successfully - '%s'.\n", mesg_id, args[0]);
    break;
    default:
      mesg = "Unknown SMTP event '" +event +"'.\n";
    break;
  }

  user && mesg && !(options &MFOPT_QUIET) && tell_object(user, mesg);
  return result;
}
#endif


