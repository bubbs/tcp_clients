#define TESTING
//// tcp_gopher
//
// Gopher implementation.
//
// Bubbs.  July/2010.


/// thoughts:
// * needs time-outs.
// * maybe decode_base64() and uuencode() / uudecode() utility methods.


/*
[connect]
< |<request-item>\n\r
 >|<item-descriptions>\n\r
.\n\r
[disconnect]
*/


#pragma no_shadow


#include <tcp_gopher.h>


virtual inherit "/" __DIR__ "tcp_socket";


/// tcp_gopher variables.

nosave    string  request_host        = 0;   // request host.
nosave    int     request_port        = 0;   // request port.
nosave    string  request_selector    = 0;   // request selector.

nosave    closure event_callback      = 0;   // event callback.
nosave    mixed*  event_extra_args    = 0;   // event extra arguments.

nosave    string  response_text       = 0;   // raw server response.
nosave    mixed*  response_items      = 0;   // parsed response items.


/// tcp_gopher query method prototypes.

public    int     query_busy            ();
public    object  query_busy_object     ();

public    mixed*  query_request_data    ();
public    string  query_request_url     ();

public    string  query_request_host    ();
public    int     query_request_port    ();
public    string  query_request_selector();

public    string  query_response_text   ();
public    mixed*  query_response_items  ();


/// tcp_gopher interface method prototypes.

public    int     clear                 ();

public    int     request               (varargs mixed* args);
public    int     abort                 ();


/// tcp_gopher process method prototypes.

protected void    gopher_success        ();
protected void    gopher_event          (int event, varargs mixed* args);


/// tcp_gopher public utility methods.

public    mixed   parse_gopher_url      (string url);
public    string  unparse_gopher_url    (mixed* req_data);

public    mixed*  parse_gopher_item     (string item_spec);


#if defined(TEST_GOPHER_URL) && !defined(NO_TEST_GOPHER_URL)
/// tcp_gopher test method prototypes.

public    string  gopher                (string url);
protected mixed   do_gopher_event       (int event, object client, mixed* req_data, object user, varargs mixed* args);
#endif


/// mudlib interface methods.

public int deny_update() {
  return ::deny_update();
}

public int clean_up(int flag) {
  return ::clean_up(flag);
}

public void destructor() {
  ::destructor();
}


/// tcp_socket callback methods.

// called when the socket is initially connected.
protected void do_socket_connect() {
  gopher_event(GOPHER_DEBUG, "[connected to '" +query_host_address() +"', port " +query_host_port() +", selector '" +query_request_selector() +"']\n");
  socket_write(request_selector +"\n\r");
}

// called when the socket receives a message.
protected void do_socket_read(string message) {
  gopher_event(GOPHER_DEBUG, implode(map(explode((message[<1] == '\n') ? message[..<2] : message, "\n"), (: sprintf(" >|%s\n", $1) :)), ""));
  response_text += message;
}

// called when the socket has been sent a message.
protected void do_socket_write(string message) {
  gopher_event(GOPHER_DEBUG, implode(map(explode((message[<1] == '\n') ? message[..<2] : message, "\n"), (: sprintf("< |%s\n", $1) :)), ""));
}

// called when the socket closes.
protected void do_socket_close() {
  gopher_event(GOPHER_DEBUG, "[closing '" +query_host_address() +"', port " +query_host_port() +"]\n");
  if (sizeof(response_items) && (response_items[0] == "aborted")) {
    response_items = 0;
    gopher_event(GOPHER_FAILED, sprintf("Request aborted by '%s'.\n", response_items[1]));
  }
  else if (!response_text) {
    response_items = 0;
    gopher_event(GOPHER_FAILED, "No response from server.\n");
  }
  else
    gopher_success();
}

// called when the socket fails to connect.
protected void do_socket_error(string error) {
  gopher_event(GOPHER_DEBUG, "[error: '" +trim(error, TRIM_BOTH, "\n\r") +"]\n");
  gopher_event(GOPHER_FAILED, error);
  clear();
}


/// tcp_gopher query method prototypes.

public    int     query_busy            () { return request_selector && !response_text; }
public    object  query_busy_object     () { return query_busy() && to_object(event_callback); }


public    mixed*  query_request_data    () {
  mixed* req_data = allocate(GOPHER_REQUEST_DATA_SIZE);
  req_data[GREQ_PROTOCOL] = "gopher";
  req_data[GREQ_HOST]     = request_host;
  req_data[GREQ_PORT]     = request_port;
  req_data[GREQ_SELECTOR] = request_selector;

  return req_data;
}

public    string  query_request_url     () {
  return unparse_gopher_url(query_request_data());
}

public    string  query_request_host    () { return request_host; }
public    int     query_request_port    () { return request_port; }
public    string  query_request_selector() { return request_selector; }

public    string  query_response_text   () { return response_text; }
public    mixed*  query_response_items  () { return deep_copy(response_items); }


/// tcp_gopher interface method prototypes.

public int clear() {
  if (!clonep())
    return 0;

  request_host     = 0;
  request_port     = 0;
  request_selector = 0;

  event_callback   = 0;
  event_extra_args = 0;

  response_text    = 0;
  response_items   = 0;

  // assert socket close.
  socket_close();

  return 1;
}

public int request(varargs mixed* args) {
  string  host       = 0;
  int     port       = 0;
  string  selector   = 0;
  closure callback   = 0;
  mixed*  extra_args = ({ });

  foreach(mixed arg : args) {
    if (!host && stringp(arg))
      host = arg;
    else if (!port && !selector && intp(arg))
      port = arg;
    else if (!selector && !callback && stringp(arg))
      selector = arg;
    else if (!callback && closurep(arg))
      callback = arg;
    else
      extra_args += ({ arg, });
  }

  if (!host || !callback)
    return 0;
  if (!clonep()) {
    object clone = clone_object(this_object());
    return (int)apply(symbol_function("request", clone), host, port, selector, callback, extra_args);
  }
  if (query_busy()) {
    apply(callback, this_object(), ({ "gopher", host, port, selector, }), GOPHER_BUSY, extra_args);
    return 0;
  }

  clear();
  request_host     = host;
  request_port     = port     || DEFAULT_GOPHER_PORT;
  request_selector = selector || "";

  event_callback   = callback;
  event_extra_args = extra_args;

  string error = socket_connect(request_host, request_port, 0);

  if (error) {
    gopher_event(GOPHER_FAILED, error);
    clear();
  }

  return 1;
}

public int abort() {
  if (!clonep() || !query_busy() || !request_host)
    return 0;

  response_items = ({ "aborted", object_name(previous_object()), });
  catch(socket_close(); nolog);

  return !response_items;
}


/// tcp_gopher process method prototypes.

protected void gopher_success() {
  gopher_event(GOPHER_SUCCESS, response_text);
  clear();
}

protected void gopher_event(int event, varargs mixed* args) {
  return event_callback && apply(event_callback, event, this_object(), query_request_data(), event_extra_args +args);
}


/// tcp_gopher public utility methods.

public mixed parse_gopher_url(string url) {
  if (!stringp(url))
    return "Bad URL.\n";

  string host     = 0;
  int    port     = 0;
  string selector = 0;
  if ((sscanf(url, "gopher://%+s:%+D/%+s", host, port, selector) == 3) ||
      (sscanf(url, "gopher://%+s:%+D", host, port) == 2) ||
      (sscanf(url, "gopher://%+s/%+s", host, selector) == 2) ||
      (sscanf(url, "gopher://%+s", host) == 1)) {
    mixed* req_data = allocate(GOPHER_REQUEST_DATA_SIZE);
    req_data[GREQ_PROTOCOL] = "gopher";
    req_data[GREQ_HOST]     = host;
    req_data[GREQ_PORT]     = port || DEFAULT_GOPHER_PORT;
    req_data[GREQ_SELECTOR] = selector || "";
    return req_data;
  }
  else
    return "Bad 'gopher:' URL.\n";
}

public string unparse_gopher_url(mixed* req_data) {
  if (!pointerp(req_data) || (sizeof(req_data) != GOPHER_REQUEST_DATA_SIZE) || (req_data[0] != "gopher"))
    return 0;

  return "gopher://" +req_data[GREQ_HOST] +((req_data[GREQ_PORT] == DEFAULT_GOPHER_PORT) ? "" : (":" +req_data[GREQ_PORT])) +"/" +req_data[GREQ_SELECTOR];
}

public mixed* parse_gopher_item(string item_spec) {
  mixed* gopher_item = 0;

  if (strlen(item_spec) && !strlen(regreplace(item_spec, "^" +GOPHER_ITEM_REGEXP +"$", "", 0))) {
    string* bits = explode(regreplace(item_spec, "^" +GOPHER_ITEM_REGEXP +"$", "\\1\n\\2\n\\3\n\\4\n\\5", 0), "\n");
    gopher_item = allocate(GOPHER_ITEM_SIZE);
    gopher_item[GITEM_ITEM_TYPE] = bits[0][0] || GITTY_PLAIN_TEXT;
    gopher_item[GITEM_DISPLAY]   = trim(bits[1]);
    gopher_item[GITEM_SELECTOR]  = trim(bits[2]);
    gopher_item[GITEM_HOST_NAME] = trim(bits[3]);
    gopher_item[GITEM_PORT]      = to_int(bits[4]) || DEFAULT_GOPHER_PORT;
  }

  return gopher_item;
}


/* * * add to httpclient;

public string decode_base64(string str) {
  if (!stringp(str))
    return 0;

  string output = "";
  int    length = strlen(str);

  return output;
}


// consider encode/decode _uuuuencode() uudecode() ?

*/


#if defined(TEST_GOPHER_URL) && !defined(NO_TEST_GOPHER_URL)
/// tcp_gopher test methods.

public string gopher(string url) {
  string error    = 0;
  mixed  req_data = parse_gopher_url(strlen(url) ? url : TEST_GOPHER_URL);
  if (stringp(req_data))
    error = req_data;
  else
    error = !request(req_data[GREQ_HOST], req_data[GREQ_PORT], req_data[GREQ_SELECTOR], #'do_gopher_event, this_player()) && "Unable to 'gopher'.\n";

  return error;
}

protected mixed do_gopher_event(int event, object client, mixed* req_data, object user, varargs mixed* args) {
  string user_mesg   = 0;
  int    dest_client = 0;

  switch(event) {
    case GOPHER_DEBUG:
      // args  : string debug_message;
      // return: void;
      // A gopher debug message.
      user_mesg   = sizeof(args) && args[0];
    break;
    case GOPHER_BUSY:
      // args  : void;
      // return: void;
      // The gopher client object is busy.
      user_mesg   = "Gopher client busy.\n";
    break;
    case GOPHER_FAILED:
      // args  : string error_message;
      // return: void;
      // The gopher request failed.
      user_mesg   = (sizeof(args) && args[0]) || "Error in gopher request.\n";
      dest_client = 1;
    break;
    case GOPHER_SUCCESS:
      // args  : string response_text;
      // return: void;
      // The gopher request succeeded.
      user_mesg   = "[Gopher]: Retrieved " +(/*string)(TCP_GOPHER_D->*/unparse_gopher_url(req_data)) +"!\n";
      dest_client = 1;

      int number = 0;
      foreach(mixed* gopher_item : args[0]) {
        user_mesg += sprintf("[%d] [%-30s]  %s\n", ++number, sprintf("%s:%d%s", gopher_item[GITEM_HOST_NAME], gopher_item[GITEM_PORT], gopher_item[GITEM_SELECTOR]), gopher_item[GITEM_DISPLAY]);
      }
    break;
    default:
      user_mesg = "Unknown gopher event '" +event +"'.\n";
    break;
  }

  user_mesg && user && tell_object(user, user_mesg);
  dest_client && destruct(client);
}
#endif
