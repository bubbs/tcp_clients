#define TESTING
#if defined(TESTING)
//#define protected public
//#define private public
#endif
//// tcp_telnet
//
// an attempt at a better telnet protocol implementation.

/// to do:
// virtually all of the telnet protocol.
// query_url(), parse_url(), unparse_url(), telnet_url(string url|mixed* url_data).



virtual inherit __DIR__ "tcp_socket";


#pragma no_shadow


#if defined(TIMEWARP)
// Timewarp style mudlib.

#include <daemons.h>

#elif defined(DUNE)
// Dune style mudlib.

#define DEFAULT_ADDRESS               "dune.servint.com 6789"
#define SUSPENDD                      "obj/cmds/wizard/tasks"

#else
// Unknown mudlib.

#endif


/// tcp_telnet definitions.

#if !defined(DEFAULT_ADDRESS)
//#define DEFAULT_ADDRESS               ("localhost " +query_mud_port())
#endif

#define DEFAULT_TELNET_PORT           23

// Callback events.
#define TELCB_DEBUG                   -1    // args: ({ string format, args... });   return void.
#define TELCB_ERROR                   00    // args: ({ string error_message })      return: void.
#define TELCB_CONNECTING              01    // args: ({ string address, int port }); return: void.
#define TELCB_CONNECTED               02    // args: ({ string address, int port }); return: void.
//#define TELCB_INPUT                   03    // args: ({ });                          return: void.
#define TELCB_RECEIVED_OUTPUT         04    // args: ({ });                          return int.
#define TELCB_OUTPUT                  05    // args: ({ string message, });          return: string error.
#define TELCB_CLOSING                 06    // args: ({ });                          return: void.
#define TELCB_CLOSED                  07    // args: ({ });                          return: void.


//////// oooh would you look at this stuff - lifted from Wunderlands telnet.h:
/*
// ************ Definitions for the TELNET protocol *************
#define IAC                           255   // interpret as command:
#define DONT                          254   // you are not to use option
#define DO                            253   // please, you use option
#define WONT                          252   // I won't use option
#define WILL                          251   // I will use option
#define SB                            250   // interpret as subnegotiation
#define SE                            240   // end sub negotiation
#define EOR                           239   // end of record (transparent mode)

#define TELCMDS                       \
({\
        "EOR", "SE", "NOP", "DMARK", "BRK", "IP", "AO", "AYT", "EC",\
        "EL", "GA", "SB", "WILL", "WONT", "DO", "DONT", "IAC",\
})
// backward starting with IAC == 255
#define TELCMD2STRING(x)              (((256-x)<sizeof(TELCMDS))?TELCMDS[<(256-x)]:(""+x))

// telnet options
#define TELOPT_BINARY                 0     // 8-bit data path
#define TELOPT_ECHO                   1     // echo
#define TELOPT_RCP                    2     // prepare to reconnect
#define TELOPT_SGA                    3     // suppress go ahead
#define TELOPT_NAMS                   4     // approximate message size
#define TELOPT_STATUS                 5     // give status
#define TELOPT_TM                     6     // timing mark
#define TELOPT_RCTE                   7     // remote controlled transmission and echo
#define TELOPT_NAOL                   8     // negotiate about output line width
#define TELOPT_NAOP                   9     // negotiate about output page size
#define TELOPT_NAOCRD                 10    // negotiate about CR disposition
#define TELOPT_NAOHTS                 11    // negotiate about horizontal tabstops
#define TELOPT_NAOHTD                 12    // negotiate about horizontal tab disposition
#define TELOPT_NAOFFD                 13    // negotiate about formfeed disposition
#define TELOPT_NAOVTS                 14    // negotiate about vertical tab stops
#define TELOPT_NAOVTD                 15    // negotiate about vertical tab disposition
#define TELOPT_NAOLFD                 16    // negotiate about output LF disposition
#define TELOPT_XASCII                 17    // extended ascic character set
#define TELOPT_LOGOUT                 18    // force logout
#define TELOPT_BM                     19    // byte macro
#define TELOPT_DET                    20    // data entry terminal
#define TELOPT_SUPDUP                 21    // supdup protocol
#define TELOPT_SUPDUPOUTPUT           22    // supdup output
#define TELOPT_SNDLOC                 23    // send location
#define TELOPT_TTYPE                  24    // terminal type
#define TELOPT_EOR                    25    // end or record
#define TELOPT_TUID                   26    // TACACS user identification
#define TELOPT_OUTMRK                 27    // output marking
#define TELOPT_TTYLOC                 28    // terminal location number
#define TELOPT_3270REGIME             29    // 3270 regime
#define TELOPT_X3PAD                  30    // X.3 PAD
#define TELOPT_NAWS                   31    // window size
#define TELOPT_TSPEED                 32    // terminal speed
#define TELOPT_LFLOW                  33    // remote flow control
#define TELOPT_LINEMODE               34    // linemode negotiations
#define TELOPT_XDISPLOC               35    // X Display Location
#define TELOPT_ENVIRON                36    // Environment opt for Port ID
#define TELOPT_AUTHENTICATION         37    // authentication
#define TELOPT_ENCRYPT                38    // authentication
#define TELOPT_NEWENV                 39    // Environment opt for Port ID

// Inofficial, mud specific telnet options
#define TELOPT_COMPRESS               85     // Mud Compression Protocol, v.1
#define TELOPT_COMPRESS2              86     // Mud Compression Protocol, v.2
#define TELOPT_MSP                    90     // Mud Sound Protocol
#define TELOPT_MXP                    91     // Mud Extension Protocol

#define TELOPT_EXOPL                  255    // extended-options-list

#define NTELOPTS                      (1+TELOPT_NEWENV)
#define TELOPTS                       \
({\
  "BINARY", "ECHO", "RCP", "SGA", "NAME",\
  "STATUS", "TM", "RCTE", "NAOL", "NAOP",\
  "NAOCRD", "NAOHTS", "NAOHTD", "NAOFFD", "NAOVTS",\
  "NAOVTD", "NAOLFD", "XASCII", "LOGOUT", "BM",\
  "DET", "SUPDUP", "SUPDUP OUTPUT",\
  "SENDLOC", "TTYPE", "EOR", \
  "TACACS UID", "OUTPUT MARKING", "TTYLOC",\
  "3270 REGIME", "X.3 PAD", "NAWS","TSPEED","LFLOW","LINEMODE",\
  "XDISPLOC","ENVIRON","AUTHENTICATION","ENCRYPT","NEWENV",\
  "TELOPT 40", "TELOPT 41", "TELOPT 42", "TELOPT 43",\
  "TELOPT 44", "TELOPT 45", "TELOPT 46", "TELOPT 47",\
  "TELOPT 48", "TELOPT 49", "TELOPT 50", "TELOPT 51",\
  "TELOPT 52", "TELOPT 53", "TELOPT 54", "TELOPT 55",\
  "TELOPT 56", "TELOPT 57", "TELOPT 58", "TELOPT 59",\
  "TELOPT 60", "TELOPT 61", "TELOPT 62", "TELOPT 63",\
  "TELOPT 64", "TELOPT 65", "TELOPT 66", "TELOPT 67",\
  "TELOPT 68", "TELOPT 69", "TELOPT 70", "TELOPT 71",\
  "TELOPT 72", "TELOPT 73", "TELOPT 74", "TELOPT 75",\
  "TELOPT 76", "TELOPT 77", "TELOPT 78", "TELOPT 79",\
  "TELOPT 80", "TELOPT 81", "TELOPT 82", "TELOPT 83",\
  "TELOPT 84", "MCCP1", "MCCP2", "TELOPT 87",\
  "TELOPT 88", "TELOPT 89", "MSP", "MEP",\
})

#define TELOPT2STRING(x)              ((x<sizeof(TELOPTS))?TELOPTS[x]:(""+x))

// sub-option qualifiers
#define TELQUAL_IS                    0      // option is...
#define TELQUAL_SEND                  1      // send option
#define TELQUAL_INFO                  2

#define TELQUAL2STRING(x)             (x<3?({"IS","SEND","INFO"})[x]:""+x)

// LINEMODE suboptions

#define LM_MODE                       1
#define LM_FORWARDMASK                2
#define LM_SLC                        3

#define MODE_EDIT                     0x01
#define MODE_TRAPSIG                  0x02
#define MODE_ACK                      0x04
#define MODE_SOFT_TAB                 0x08
#define MODE_LIT_ECHO                 0x10

#define MODE_MASK                     (MODE_EDIT|MODE_TRAPSIG|MODE_ACK|MODE_SOFT_TAB|MODE_LIT_ECHO)

#define SLC_SYNCH                     1
#define SLC_BRK                       2
#define SLC_IP                        3
#define SLC_AO                        4
#define SLC_AYT                       5
#define SLC_EOR                       6
#define SLC_ABORT                     7
#define SLC_EOF                       8
#define SLC_SUSP                      9
#define SLC_EC                        10
#define SLC_EL                        11
#define SLC_EW                        12
#define SLC_RP                        13
#define SLC_LNEXT                     14
#define SLC_XON                       15
#define SLC_XOFF                      16
#define SLC_FORW1                     17
#define SLC_FORW2                     18
#define SLC_MCL                       19
#define SLC_MCR                       20
#define SLC_MCWL                      21
#define SLC_MCWR                      22
#define SLC_MCBOL                     23
#define SLC_MCEOL                     24
#define SLC_INSRT                     25
#define SLC_OVER                      26
#define SLC_ECR                       27
#define SLC_EWR                       28
#define SLC_EBOL                      29
#define SLC_EEOL                      30

#define SLC_NAMES                     \
({\
  "0", "SYNCH", "BRK", "IP", "AO", "AYT", "EOR", \
  "ABORT", "EOF", "SUSP", "EC", "EL", "EW", "RP", \
  "LNEXT", "XON", "XOFF", "FORW1", "FORW2", \
  "MCL", "MCR", "MCWL", "MCWR", "MCBOL", "MCEOL", \
  "INSRT", "OVER", "ECR", "EWR", "EBOL", "EEOL"\
})

#define SLC2STRING(x)                 ((x)<sizeof(SLC_NAMES)?SLC_NAMES[x]:sprintf("%02x",x))

#define SLC_NOSUPPORT                 0
#define SLC_CANTCHANGE                1
#define SLC_VARIABLE                  2
#define SLC_DEFAULT                   3
#define SLC_ACK                       0x80
#define SLC_FLUSHIN                   0x40
#define SLC_FLUSHOUT                  0x20
#define SLC_LEVELBITS                 0x03

#define SLC_FLAGNAME                  ({ "NOSUPPORT", "CANTCHANGE", "VARIABLE", "DEFAULT" })

#define ENV_VAR                       0
#define ENV_VALUE                     1
#define ENV_ESC                       2
#define ENV_USERVAR                   3
*/
////////



/// tcp_telnet state variables.

nosave    closure  callback         = 0;
nosave    string*  input_buffer     = 0;
nosave    string*  output_buffer    = 0;


/// tcp_telnet query method prototypes.

public    int      query_busy         ();
//public    int    query_connected      ();

public    string   query_host         ();
public    int      query_port         ();

public    int      query_input        ();
public    int      query_output       ();


/// tcp_telnet interface method prototypes.

public    string   telnet_connect     (string addr, int port, closure cb);
public    string   telnet_disconnect  ();

public    string   add_telnet_input   (string input);
public    string   add_telnet_output  (string output);

public    string   flush_telnet       ();
public    string   flush_telnet_input ();
public    string   flush_telnet_output();

/// tcp_telnet process method prototypes.

protected mixed    telnet_callback    (int event, varargs mixed* args);


/// tcp_socket callbacks.

protected void do_socket_connect() {
  telnet_callback(TELCB_CONNECTED, query_host_address(), query_host_port());
  telnet_callback(TELCB_DEBUG, "connected to '%s %d'", query_host_address(), query_host_port());
}

protected void do_socket_read(string message) {
#if 1
  string output = message;
#else
  string output = "";
  int    pos    = -1;
  while((pos = member(message, IAC)) != -1) {
    // do telnet negotiation.
  }
#endif

  strlen(output) && add_telnet_output(output);
}

// called when a message is written to the socket.
protected void do_socket_write(mixed message) { }

protected void do_socket_close() {
  telnet_callback(TELCB_CLOSED);

  socket_clear();

  callback = 0;
  //input_buffer  = 0;
  //output_buffer = 0;
}

protected void do_socket_error(string error) {
  telnet_callback(TELCB_ERROR, error);
  //clear();
}


/// tcp_telnet query methods.

public int    query_busy     () { return callback && 1; }

public int    query_input    () { return sizeof(input_buffer); }
public int    query_output   () { return sizeof(output_buffer); }


/// tcp_telnet interface methods.


public string telnet_connect(string addr, int port, closure cb) {
  if (!stringp(addr) || !intp(port) || !closurep(cb))
    return "invalid arguments";

  if (!clonep())
    return clone_object(load_name())->telnet_connect(addr, port, cb);

  if (query_busy())
    return "busy";

  addr = addr && trim(addr);
#if defined(DEFAULT_ADDRESS)
  if (!strlen(addr))
    addr = DEFAULT_ADDRESS;
#else
  if (!strlen(addr))
    return "no address";
#endif

  if (!port) {
    int    pos   = member(addr, ' ');
    port  = (pos == -1) ? DEFAULT_TELNET_PORT : to_int(addr[pos +1..]);
    addr  = (pos == -1) ? addr : addr[..pos -1];
  }

  string error = socket_connect(addr, port, 0);
  if (error)
    return error;

  callback      = cb;
  input_buffer  = ({ });
  output_buffer = ({ });
  telnet_callback(TELCB_DEBUG, "connecting to '%s %d'", addr, port);
  telnet_callback(TELCB_CONNECTING, addr, port);

  return 0;
}

public string telnet_disconnect() {
  if (!query_busy())
    return "not connected";

  if (extern_call() && (previous_object() != to_object(callback)))
    return "invalid call";

  telnet_callback(TELCB_DEBUG, "closing connection");
  telnet_callback(TELCB_CLOSING);
  query_socket_connected() && socket_close();

  return 0;
}

public string add_telnet_input(string input) {
  if (!query_busy())
    return "not connected";

  if (extern_call() && (previous_object() != to_object(callback)))
    return "invalid call";

  input_buffer ||= ({ });
  input_buffer  += ({ input, });
  telnet_callback(TELCB_DEBUG, "added input (%d bytes%s)", strlen(input), extern_call() ? ", external call" : "");

  return 0;
}

public string add_telnet_output(string output) {
  if (!query_busy())
    return "not connected";

  if (extern_call() && (previous_object() != to_object(callback)))
    return "invalid call";

  if (stringp(output)) {
    output_buffer ||= ({ });
    output_buffer += ({ output, });
  }
else map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->add_telnet_output(%#Q);\n", this_object(), output));
  telnet_callback(TELCB_RECEIVED_OUTPUT) && flush_telnet_output();
  telnet_callback(TELCB_DEBUG, "received output (%d bytes%s)", strlen(output), extern_call() ? ", external call" : "");

  return 0;
}

public string flush_telnet() {
  if (!query_busy())
    return "not connected";

  if (extern_call() && (previous_object() != to_object(callback)))
    return "invalid call";

  return flush_telnet_input() || flush_telnet_output();
}

public string flush_telnet_input() {
  if (!query_busy())
    return "not connected";

  if (extern_call() && (previous_object() != to_object(callback)))
    return "invalid call";

  string error = 0;
  int    size  = sizeof(input_buffer);
  for(int index = 0; (index < size) && !error; index++) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q::flush_telnet_input(); about to socket_write(%#Q);\n", this_object(), input_buffer[index]));
    error = socket_write(input_buffer[index]);
    //if (!error)
      input_buffer[index] = 0;
  }
  input_buffer = filter(input_buffer, #'stringp);

  return error;
}

public string flush_telnet_output() {
  if (!query_busy())
    return "not connected";

  if (extern_call() && (previous_object() != to_object(callback)))
    return "invalid call";

  string error = 0;
  int    size  = sizeof(output_buffer);
  for(int index = 0; (index < size) && !error; index++) {
    error = telnet_callback(TELCB_OUTPUT, output_buffer[index]);
    //if (!error)
      output_buffer[index] = 0;
  }
  output_buffer = filter(output_buffer, #'stringp);

  return error;
}


/// tcp_telnet process methods.


protected mixed telnet_callback(int event, varargs mixed* args) {
  return callback && funcall(callback, event, this_object(), args);
}


#if defined(TESTING)

#if defined(TIMEWARP)
// Timewarp style mudlib.

#include <input_to.h>

#elif defined(DUNE)
// Dune style mudlib.

#define TELL_OBJECT(O, M)           (efun::tell_object(O, M))

#include <sys/input_to.h>

#else
// Unknown mudlib.

#include <sys/input_to.h>

#endif

#if !defined(TELL_OBJECT)
#define TELL_OBJECT(O, M)           (tell_object(O, M))
#endif


nosave    object  telnet_user     = 0;
nosave    int     suspend_id      = 0;

public    int     main              (string str);
public    string  start             (string host, int port);
protected int     catch_input       ();
protected void    do_catch_input    (string input);
protected mixed   do_telnet_callback(int event, object client, mixed* args);


public int main(string str) {
  string verb  = query_verb() || "telnet";
  if (str == "-?")
    return notify_fail(sprintf("Usage: %s [<address> [<port>]]\n", verb));
  string error = start(str, 0);
  return error ? notify_fail(sprintf("%s: Error - %s.\n", verb, error)) : 1;
}

public string start(string host, int port) {
  !this_player() && raise_error("tcp_telnet::start(): this_player() must be non-zero.\n");
  host && !stringp(host) && raise_error("tcp_telnet::start(): Bad type to argument 1.\n");
  !intp(port) && raise_error("tcp_telnet::start(): Bad type to argument 2.\n");

  string error = 0;
  if (!telnet_user) {
#if defined(DEFAULT_ADDRESS)
    host ||= DEFAULT_ADDRESS;
#else
    if (!host)
      error = "no host specified";
#endif
    if (!error) {
      !port && (sscanf(host, "%~+s%t%~+U") == 2) && sscanf(host, "%s%t%U", host, port);
      port       ||= DEFAULT_TELNET_PORT;
      telnet_user  = this_player();
      error        = telnet_connect(host, port, #'do_telnet_callback);
      error      ||= !catch_input() && "unable to catch input";
      telnet_user  = !error && telnet_user;
    }
  }
  else if (telnet_user != this_player())
    error = "busy";
  else if (host)
    error = "busy";
  else {
    error = flush_telnet_output();
    error = error || (!catch_input() && "unable to catch input");
  }

  return error;
}

protected int catch_input() {
  //flush_telnet_output();

  int result = telnet_user && (telnet_user == this_player()) && !query_input_pending(telnet_user);
  result && input_to(#'do_catch_input, INPUT_PROMPT, "");
  return result && (find_input_to(telnet_user, #'do_catch_input) != -1) && 1;
}

protected void do_catch_input(string input) {
  string error      = 0;
  int    exit_input = 0;
  if (input[0] == '\029')
     switch(input[1..]) {
       case "q":
         exit_input = 1;
         telnet_disconnect();
       break;
       case "z":
         exit_input    = 1;
#if defined(SUSPENDD)
         string suspend_name = "telnet";
         if (query_socket_connected())
           suspend_name = sprintf("%s %s%s", suspend_name, query_host_address(), (query_host_port() == DEFAULT_TELNET_PORT) ? "" : sprintf(" %d", query_host_port()));
         suspend_id = (int)(SUSPENDD->suspend(suspend_name, (: flush_telnet_output(); catch_input(); suspend_id = 0; :)));
#endif
       break;
     }
  else
    error = add_telnet_input(input +(0 ? "" : "\n")) || flush_telnet_input();
  error && telnet_user && TELL_OBJECT(telnet_user, sprintf("[telnet: error - %s]\n", error));

  !exit_input && catch_input();
}

protected mixed do_telnet_callback(int event, object client, mixed* args) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->do_telnet_callback(%d, ...%#Q...); previous_object=%#Q\n", this_object(), event, args, previous_object()));
  mixed result = 0;

//1 && !telnet_user && map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->do_telnet_client(%d, ...) - telnet_user=%#Q\n", this_object(), event, telnet_user));
  int dest_client = !telnet_user && 1;

  switch(event) {
    case TELCB_DEBUG:
      // args  : ({ mesg_format, mixed* args... })
      // return: void
      telnet_user && TELL_OBJECT(telnet_user, sprintf("[telnet: %s]\n", apply(#'sprintf, args)));
    break;
    case TELCB_ERROR:
      // args  : ({ string error_message })
      // return: void
      telnet_user && TELL_OBJECT(telnet_user, sprintf("[telnet: error in connection - %s]\n", args[0]));
      dest_client = 1;
    break;
    case TELCB_CONNECTING:
      // args  : ({ string address, int port })
      // return: void
      telnet_user && TELL_OBJECT(telnet_user, sprintf("[telnet: connecting to '%s %d']\n", args[0], args[1]));
    break;
    case TELCB_CONNECTED:
      // args  : ({ string address, int port })
      // return: void
      telnet_user && TELL_OBJECT(telnet_user, sprintf("[telnet: connected to '%s %d']\n", args[0], args[1]));
    break;
//    case TELCB_INPUT:
//      // args  : ({ })
//      // return: void
//    break;
    case TELCB_RECEIVED_OUTPUT:
        // args  : ({ })
        // return: int do_flush
      result = !suspend_id;
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("TELCB_RECEIVED_OUTPUT: do_flush=%#Q\n", result));
    break;
    case TELCB_OUTPUT:
      // args   : ({ string message })
      // return: string error
      telnet_user && TELL_OBJECT(telnet_user, args[0]);
    break;
    case TELCB_CLOSING:
         // args  : ({ })
         // return: void
    break;
    case TELCB_CLOSED :
      // args   : ({ })
      // return: void
      telnet_user && TELL_OBJECT(telnet_user, "[telnet: connection closed]\n");
      if (telnet_user && (find_input_to(telnet_user, #'do_catch_input) != -1)) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->do_telnet_callback(%d...) - connection closed :(\n", this_object(), event));
        remove_input_to(telnet_user, #'do_catch_input);
        // the following lines sort out the 'still noecho' bug in remove_input_to().
        // this requires telnet_user == this_player(), so should do this via a command().
        if (this_player() == telnet_user) {
          input_to(#'do_catch_input, 0);
          remove_input_to(telnet_user, #'do_catch_input);
        }
        telnet_user = 0;
      }
      dest_client = 1;
    break;
    default:
      telnet_user && TELL_OBJECT(telnet_user, sprintf("[telnet: unrecognised callback '%d']\n", event));
      dest_client = 1;
    break;
  }

//dest_client && map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->do_telnet_callback(%d, ...) - desting client.\n", this_object(), event));
//dest_client && telnet_user && TELL_OBJECT(telnet_user, sprintf("%#Q->do_telnet_callback(%d, ...) desting client.\n", this_object(), event));
  //dest_client && client && destruct(client);
  dest_client && client && call_out((: $1 && destruct($1) :), 1, client);

  return result;
}

/*destructor() {
  ::destructor();
  //map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->destructor() called from %#Q (%#Q).\n", this_object(), previous_object(), caller_stack(1)));
  //raise_error("don't raise an error in a destructor!\n");
}*/

#endif

