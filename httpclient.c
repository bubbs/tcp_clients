//// httpclient.c
// v0.5.2.
//
// LDMud daemon and object providing http client services.
// see httpclient.txt for history and usage notes.
//
// By Bubbs (bubbs@eclipse.co.uk).


#pragma combine_strings
#pragma local_scopes
#pragma no_inherit
#pragma no_shadow
#pragma no_warn_empty_casts
#pragma pedantic
#pragma save_types
#pragma strict_types
#pragma verbose_errors
#pragma warn_deprecated
#pragma warn_function_inconsistent


#if defined(TIMEWARP)
// Timewarp style mudlib.

#include <httpclient.h>
#include <files.h>
#include <lpctypes.h>
#include <strings.h>

#elif defined(DUNE)
// Dune style mudlib.

#include <httpclient.h>
#include <sys/files.h>
#include <sys/lpctypes.h>
#include <sys/strings.h>

virtual inherit HTTPCLIENT_DIR "parse_time";

#else
// Unknown mudlib.

#include <httpclient.h>
#include <sys/files.h>
#include <sys/lpctypes.h>
#include <sys/strings.h>

virtual inherit HTTPCLIENT_DIR "parse_time";

#endif


virtual inherit HTTPCLIENT_DIR "tcp_socket";


/// httpclient variables.

nosave    mapping source_types          = 0;   // Source type information.

nosave    string  language              = 0;   // Can be defined, not used otherwise.
nosave    string  media_types           = 0;   // Can be defined, defaults otherwise.
nosave    string  referer_url           = 0;   // Can be defined, not used otherwise.
nosave    string  user_agent            = 0;   // Can be defined, defaults otherwise.

nosave    mixed*  cookies               = 0;   // Any available cookies: ({ cookie, ... }).
nosave    string* authorization         = 0;   // HTTP authorization: ({ user, password, }).

nosave    string  url                   = 0;   // Target url.
nosave    mixed*  url_data              = 0;   // Exploded url: ({ protocol, host, port, path, args, }).
nosave    mixed   req_content           = 0;   // Used for POST, PUT methods.
nosave    closure callback              = 0;   // API for external services.

nosave    string  method                = 0;   // GET, HEAD, OPTIONS and POST are supported.
nosave    string  base_url              = 0;   // Base url for relative urls.
nosave    mapping req_headers           = 0;   // Request headers.

nosave    mixed   raw_response          = 0;   // Raw unparsed response from server.
nosave    string  status_version        = 0;   // The server http version.
nosave    int     status_code           = 0;   // The status code (200, 404, etc).
nosave    string  status_desc           = 0;   // The status code mnemonic (ok, not found, etc).
nosave    mapping resp_headers          = 0;   // Response headers.
nosave    mixed   content               = 0;   // The requested, decoded, document.


/// httpclient query methods.

public    string  query_default_user_agent();
public    string  query_http_version      ();
public    string  query_httpclient_version();

public    string* query_source_types      ();
public    string  query_source_type_desc  (string source_type);
public    string* query_source_type_exts  (string source_type);
public    string  query_source_type_regexp(string source_type);

public    int     query_busy              ();
public    object  query_busy_object       ();

public    string  query_language          ();
public    string  query_media_types       ();
public    string  query_referer_url       ();
public    string  query_user_agent        ();

public    mixed*  query_cookies           ();
public    string* query_authorization     ();

public    string  query_url               ();
public    mixed*  query_url_data          ();

public    string  query_method            ();
public    string  query_base_url          ();
public    mapping query_req_headers       ();
public    mixed   query_req_content       ();

public    mixed   query_response          ();
public    string  query_status_version    ();
public    int     query_status_code       ();
public    string  query_status_desc       ();
public    mapping query_resp_headers      ();
public    mixed   query_content           ();


/// httpclient set methods.

public    int     set_language            (string l);
public    int     set_media_types         (string mt);
public    int     set_referer_url         (string ru);
public    int     set_user_agent          (string ua);

public    int     set_cookies             (mixed* cs);
public    int     set_authorization       (string u, string p);

/// httpclient public process method prototypes.

public    int     clear                   ();
public    int     request                 (varargs mixed* args);
public    int     request_delete          (mixed url, closure callback);
public    int     request_get             (mixed url, closure callback);
public    int     request_head            (mixed url, closure callback);
public    int     request_options         (mixed url, closure callback);
public    int     request_post            (mixed url, mixed req_content, closure callback);
public    int     request_put             (mixed url, mixed req_content, closure callback);
public    int     request_trace           (mixed url, closure callback);
public    int     repeat_request          ();
public    int     abort_request           ();

public    string  apply_base_url          (string url, string base_url);

/// httpclient protected process method prototypes.

protected void    init_request            (string method, string u, mixed* udata, mixed rec_content, string rec_ctype, closure callback);
protected void    perform_request         ();
protected string  perform_about_request   ();
protected string  perform_file_request    ();
protected string  perform_http_request    ();

protected mixed   client_callback_event   (int event, mixed data);

protected void    log_message             (string context, string message);
protected void    process_request         (mixed raw_response);
protected void    process_header          (string header, string value);
protected void    process_cookies         (string cookie_text);
protected mapping process_authenticate    (string authenticate, string error);
protected string  process_chunked         (string content, string error);
protected void    process_status_1xx      (string content);
protected void    process_status_2xx      (string content);
protected void    process_status_3xx      (string content);
protected void    process_status_4xx      (string content);
protected void    process_status_5xx      (string content);

/// httpclient public utilty method prototypes.

public    mixed   parse_url               (string target_url);
public    mixed   parse_about_url         (string target_url);
public    mixed   parse_file_url          (string target_url);
public    mixed   parse_http_url          (string target_url);

public    string  unparse_url             (mixed* url_data);
public    string  unparse_about_url       (mixed* url_data);
public    string  unparse_file_url        (mixed* url_data);
public    string  unparse_http_url        (mixed* url_data);

public    int     parse_cookie_date       (string date_text);

public    mapping read_source_types       (string data_file);
public    string  determine_source_type   (string path, string source);

public    string  unquote_string          (string str);
public    string  encode_www_form         (mixed value, varargs string* excludes);
public    string  decode_www_form         (string value);
public    string  encode_base64           (string value);
public    string  decode_base64           (string value);
public    int     hex_to_int              (string hex);
public    string  int_to_hex              (int number);


/// httpclient query methods.

public    string  query_default_user_agent()  { return DEFAULT_USER_AGENT; }
public    string  query_http_version      ()  { return HTTP_VERSION; }
public    string  query_httpclient_version()  { return CLIENT_VERSION; }

public string* query_source_types() {
  return
    mappingp(source_types) &&
    m_indices(source_types);
}

public string query_source_type_desc(string source_type) {
  return
    mappingp(source_types) &&
    stringp(source_types[source_type, SRCTYP_DESCRIPTION]) &&
    strlen(source_types[source_type, SRCTYP_DESCRIPTION]) &&
    source_types[source_type, SRCTYP_DESCRIPTION];
}

public string* query_source_type_exts(string source_type) {
  return
    mappingp(source_types) &&
    pointerp(source_types[source_type, SRCTYP_FILE_EXTENSIONS]) &&
    sizeof(source_types[source_type, SRCTYP_FILE_EXTENSIONS]) &&
    source_types[source_type, SRCTYP_FILE_EXTENSIONS];
}

public string query_source_type_regexp(string source_type) {
  return
    mappingp(source_types) &&
    stringp(source_types[source_type, SRCTYP_MATCH_REGEXP]) &&
    strlen(source_types[source_type, SRCTYP_MATCH_REGEXP]) &&
    source_types[source_type, SRCTYP_MATCH_REGEXP];
}

public    int     query_busy              () { return (url != 0) && !status_code; }
public    object  query_busy_object       () { return query_busy() && to_object(callback); }

public    string  query_language          () { return language; }
public    string  query_media_types       () { return media_types; }
public    string  query_referer_url       () { return referer_url; }
public    string  query_user_agent        () { return (strlen(user_agent) > 0) ? user_agent : DEFAULT_USER_AGENT; }

public    mixed*  query_cookies           () { return deep_copy(cookies); }
public    string* query_authorization     () { return deep_copy(authorization); }

public    string  query_url               () { return url; }
public    mixed*  query_url_data          () { return deep_copy(url_data); }

public    string  query_method            () { return method; }
public    string  query_base_url          () { return base_url; }
public    mapping query_req_headers       () { return deep_copy(req_headers); }
public    mixed   query_req_content       () { return deep_copy(req_content); }

public    mixed   query_response          () { return deep_copy(raw_response); }
public    string  query_status_version    () { return status_version; }
public    int     query_status_code       () { return status_code; }
public    string  query_status_desc       () { return status_desc; }
public    mapping query_resp_headers      () { return deep_copy(resp_headers); }
public    mixed   query_content           () { return deep_copy(content); }


/// httpclient set methods.

public int set_language(string l) {
  return stringp(l) && stringp(language = l);
}

public int set_media_types(string mt) {
  return stringp(mt) && stringp(media_types = mt);
}

public int set_referer_url(string ru) {
  return stringp(ru) && stringp(referer_url = ru);
}

public int set_user_agent(string ua) {
  return stringp(ua) && stringp(user_agent = ua);
}


public int set_cookies(mixed* cs) {
  return pointerp(cs) && (sizeof(cs) == sizeof(filter(cs, (: VALID_COOKIE($1) :)))) && pointerp(cookies = deep_copy(cs));
}

public int set_authorization(string u, string p) {
  return stringp(u) && stringp(p) && pointerp(authorization = ({ u, p, }));
}


/// tcp_socket stub methods.

// called when the socket is initially connected.
protected void do_socket_connect() {
  if (!pointerp(url_data))
    return;

  string req = strlen(url_data[URD_ARGS]) ? ("?" +url_data[URD_ARGS]) : "";
  req = method +" " +url_data[URD_PATH] +req +" HTTP/" +HTTP_VERSION +"\n";

  foreach(string key, string value : req_headers)
    req += key +": " +value +"\n";
  req += "\n";
  if (sizeof(req_content) && ((method == "POST") || (method == "PUT")))
    req += to_string(req_content) +"\n";

  socket_write(req);
}

// called when the socket receives a message.
protected void do_socket_read(mixed message) {
//write_file("/secure/sockets/bin/dump", message);
  if (!raw_response) {
    if (stringp(message))
      raw_response = "";
    else
      raw_response = ({ });
  }
  raw_response += message;
}

// called when a message is written to the socket.
protected void do_socket_write(mixed message) {
//map(filter_objects(users(), "id", "bubbs"), #'tell_object, sprintf("%#Q->do_socket_write();\n%s\n", this_object(), stringp(message) ? message : sprintf("%#Q", message)));
}

// called when the socket closes.
protected void do_socket_close() {
  if (!req_headers)
    return;
  if (member(req_headers, "aborted")) {
    status_code = 0;
    client_callback_event(HTTP_FAILED, sprintf("Request aborted by '%s'.", req_headers["aborted"]));
  }
  else
    process_request(raw_response);
}

// called when the socket fails to connect.
protected void do_socket_error(string error) {
  if (!clonep())
    return;
  client_callback_event(HTTP_FAILED, error);
  clear();
}


/// httpclient public process methods.

public int clear() {
  if (!clonep())
    return 0;

  source_types     = read_source_types(DEFAULT_SOURCE_TYPES_FILE);

  //language         = 0;
  //media_types      = 0;
  //referer_url      = 0;
  //user_agent       = 0;

  cookies          = 0;
  authorization    = 0;

  url              = 0;
  url_data         = 0;
  req_content      = 0;
  callback         = 0;

  // assert socket close.
  socket_close();

  method           = 0;
  base_url         = 0;
  req_headers      = 0;

  raw_response     = 0;
  status_version   = 0;
  status_code      = 0;
  status_desc      = 0;
  resp_headers     = 0;
  content          = 0;
  return 1;
}

public int request(varargs mixed* args) {
  source_types ||= read_source_types(DEFAULT_SOURCE_TYPES_FILE);

  string  m   = 0;
  mixed   u   = 0;
  mixed   rc  = 0;
  mixed   rct = 0;
  closure cb  = 0;

  switch(sizeof(args) && stringp(args[0]) && upper_case(args[0])) {
    case "DELETE" :
    case "GET"    :
    case "HEAD"   :
    case "OPTIONS":
    case "POST"   :
    case "PUT"    :
    case "TRACE"  :
      m    = upper_case(args[0]);
      args = args[1..];
    break;
    default:
      m    = DEFAULT_HTTP_METHOD;
    break;
  }

  switch(sizeof(args)) {
    case 0:  break;
    case 1:
      u   = (stringp(args[0]) && args[0]) || (pointerp(args[0]) && unparse_url(args[0]));
    break;
    case 2:
      u   = (stringp(args[0]) && args[0]) || (pointerp(args[0]) && unparse_url(args[0]));
      cb  = closurep(args[1]) && args[1];
    break;
    case 3:
      u   = (stringp(args[0]) && args[0]) || (pointerp(args[0]) && unparse_url(args[0]));
      rc  = args[1];
      cb  = closurep(args[2]) && args[2];
    break;
    default:
      u   = (stringp(args[0]) && args[0]) || (pointerp(args[0]) && unparse_url(args[0]));
      rc  = args[1];
      rct = stringp(args[2]) && args[2];
      cb  = closurep(args[3]) && args[3];
    break;
  }

  u = pointerp(u) ? unparse_url(u) : u;
  if (!stringp(u))
    return 0;
  //if (!closurep(cb))
  //  return 0;
  if (!clonep())
    return (int)(clone_object(this_object())->request(m, u, rc, rct, cb));
  if (query_busy()) {
    funcall(cb, this_object(), HTTP_BUSY);
    return 0;
  }
  mixed udata = parse_url(u);
  if (stringp(udata)) {
    funcall(cb, this_object(), HTTP_FAILED, udata);
    return 0;
  }

//printf("m=%#Q; u=%#Q; rc=%#Q; rct=%#Q; cb=%#Q\n", m, u, rc, rct, cb);

  switch(m) {
    case "OPTIONS":
      if (udata[URD_PATH] == "/*")
        udata[URD_PATH] = "*";
    // no break.
    case "DELETE" :
    case "GET"    :
    case "HEAD"   :
    case "TRACE"  :
      rc = closurep(rc) ? funcall(rc, u, udata) : rc;
      if (stringp(rc) || pointerp(rc)) {
        udata[URD_ARGS] = encode_www_form(rc);
        url = unparse_url(udata);
      }
    break;
    case "POST"   :
    case "PUT"    :
      rc = closurep(rc) ? funcall(rc, u, udata) : rc;
      switch(typeof(rc)) {
        case T_STRING :
        case T_MAPPING:
          rc    = rct ? encode_www_form(rc) : rc;
          rct ||= WWW_FORM_CONTENT_TYPE;
        break;
        case T_QUOTED_ARRAY:
          rc    = unquote(rc);
        // no break.
        case T_POINTER:
          rc    = filter(rc, #'intp);
        // no break.
        case T_NUMBER :
        case T_FLOAT  :
        case T_OBJECT :
        case T_SYMBOL :
        case T_CLOSURE:
          rc  &&= to_string(rc);
          //rct ||= TEXT_CONTENT_TYPE;
          rct ||= determine_source_type(udata[URD_PATH], rc);
        break;
        default:
          rc  = 0;
          rct = 0;
          //client_callback_event(HTTP_FAILED, "Invalid request content.");
          //return 0;
        break;
      }
    break;
  }

  init_request(m, u, udata, rc, rct, cb);
  call_out(#'perform_request, 0);
  return 1;
}

public int request_delete (mixed u, closure cb)           { return request("DELETE", u, cb); }
public int request_get    (mixed u, closure cb)           { return request("GET",    u, cb); }
public int request_head   (mixed u, closure cb)           { return request("HEAD",   u, cb); }
public int request_options(mixed u, closure cb)           { return request("OPTION", u, cb); }
public int request_post   (mixed u, mixed rc, closure cb) { return request("POST",   u, rc, cb); }
public int request_put    (mixed u, mixed rc, closure cb) { return request("PUT",    u, rc, cb); }
public int request_trace  (mixed u, closure cb)           { return request("TRACE",  u, cb); }

public int repeat_request() {
//  if (!closurep(cb))
//    return 0;
  if (!clonep() || query_busy() || !url || !pointerp(url_data) || !callback || !method)
    return 0;
  raw_response   = 0;
  status_version = 0;
  status_code    = 0;
  status_desc    = 0;
  resp_headers   = 0;
  content        = 0;
  init_request(method, url, url_data, req_content, req_headers["Content-Type"], callback);
  call_out(#'perform_request, 0);
  return 1;
}

public int abort_request() {
  if (!clonep() || !query_busy() || !req_headers)
    return 0;
  req_headers["aborted"] = object_name(previous_object());
  catch(socket_close(); nolog);
  return (status_code != 0);
}

public string apply_base_url(string u, string b) {
//log_message("apply_base_url", sprintf("url %#Q, base %#Q.", u, b));
//printf("url %#Q, base %#Q.\n", u, b);
  string new_url = 0;
  mixed  udata   = stringp(b) ? parse_url(b) : url_data;
//printf("udata: %#Q\n", udata);
  if (!stringp(u) || !udata || stringp(udata));
  else if (!strlen(u))
    new_url = b || url;
  else if (!strstr(u, "about:") || !strstr(u, "file:") || !strstr(u, "http:"))
    new_url = u;
  else {
    new_url = udata[URD_PROTOCOL] +":";
    if (udata[URD_PROTOCOL] == "http") {
      new_url += "//" +udata[URD_HOST];
      if (udata[URD_PORT] != DEFAULT_HTTP_PORT)
        new_url += ":" +udata[URD_PORT];
    }
    if (u[0] == '#')
      new_url += udata[URD_PATH] +u;
    else if (u[0] == '/')
      new_url += u;
    else if (u[0..1] == "./") {
      int pos = strrstr(udata[URD_PATH], "/");
      if (pos == -1)
        u = "/" +u;
      new_url += udata[URD_PATH][0..pos] +u[2..];
    }
    else if (u[0..2] == "../") {
      string new_path = udata[URD_PATH];
      while(u[0..2] == "../") {
        int pos = strrstr(new_path, "/");
        if (pos != -1)
          pos = strrstr(new_path, "/", pos -1);
        new_path = new_path[0..pos];
        u = u[3..];
      }
      new_url += new_path +u;
    }
    else {
      int pos = strrstr(udata[URD_PATH], "/");
      if (pos == -1)
        u = "/" +u;
      new_url += udata[URD_PATH][0..pos] +u;
    }
  }
//log_message("apply_base_url", sprintf("new url %Q.", new_url));
//printf("new url %Q.\n", new_url);
  return new_url;
}


/// httpclient protected process methods.

protected void init_request(string m, string u, mixed* udata, string rc, string rct, closure cb) {
  if (!clonep())
    return;

  cookies     ||= client_callback_event(HTTP_GET_COOKIES, url);
  cookies       = pointerp(cookies) && cookies;
  req_headers ||= ([ ]);

  url           = u;
  url_data      = udata;
  req_content   = rc;
  callback      = cb;

  method        = m;
  int pos       = strrstr(udata[URD_PATH], "/");
  base_url      = udata[URD_PROTOCOL] +"://" +udata[URD_HOST] +((udata[URD_PORT] == DEFAULT_HTTP_PORT) ? "" : (":" +udata[URD_PORT])) +((pos == -1) ? "/" : udata[URD_PATH][0..pos -1]);


  m_add(req_headers, "Accept", strlen(media_types) ? media_types : DEFAULT_MEDIA_TYPES);
  m_add(req_headers, "Accept-Encoding", "chunked, identity");
//m_add(req_headers, "Accept-Charset", "US-ASCII");
  //language ||= "en-gb, en:q=0.9, en-*=0.8"
  stringp(language) &&
    m_add(req_headers, "Accept-Language", language);
  pointerp(authorization) && (sizeof(authorization) == 2) &&
    stringp(authorization[0]) && stringp(authorization[1]) &&
    m_add(req_headers, "Authorization", "Basic " +encode_base64(authorization[0] +":" +authorization[1]));
  m_add(req_headers, "Cache-Control", "no-cache, protected");
  m_add(req_headers, "Connection", "close");
  if (rc && ((method == "POST") || (method == "PUT"))) {
    m_add(req_headers, "Content-Length", strlen(rc));
    m_add(req_headers, "Content-Type",   rct || TEXT_CONTENT_TYPE);
  }
  if (sizeof(cookies)) {
    pos = strrstr(url_data[URD_PATH], "/");
    string  path    = (pos == -1) ? "/" : url_data[URD_PATH][0..pos];
    int     secure  = (url_data[URD_PROTOCOL] == "https");
    mapping matched = ([ ]);
    foreach(mixed* cookie : cookies) {
      if (VALID_COOKIE(cookie) &&
          ((cookie[CK_EXPIRES] < 0) || (cookie[CK_EXPIRES] > time())) &&
          (url_data[URD_HOST][<strlen(cookie[CK_DOMAIN])..] == cookie[CK_DOMAIN]) &&
          !strstr(path, cookie[CK_PATH]) &&
          (cookie[CK_SECURE] == secure) &&
          client_callback_event(HTTP_SEND_COOKIE, cookie)) {
        if (!member(matched, cookie[CK_NAME]))
          matched[cookie[CK_NAME]] = ([ ]);
        matched[cookie[CK_NAME]][strlen(cookie[CK_PATH])] = cookie[CK_VALUE];
      }
    }
    string cookie_str = "";
    foreach(string key, mapping values : matched) {
      int* lens = sort_array(m_indices(values), #'<);
      foreach(int len : lens)
        cookie_str += "; " +key +"=" +values[len];
    }
    (strlen(cookie_str) > 2) && m_add(req_headers, "Cookie", cookie_str[2..]);
//client_callback_event(HTTP_DEBUG, "Sending Cookie: " +cookie_str[2..]);
  }
  m_add(req_headers, "Host", udata[URD_HOST] +((udata[URD_PORT] == DEFAULT_HTTP_PORT) ? "" : ":" +udata[URD_PORT]));
//m_add(req_headers, "Proxy-Authorization", ... needs work
  strlen(referer_url) &&
    m_add(req_headers, "Referer", referer_url);
  m_add(req_headers, "TE", "chunked");
  m_add(req_headers, "User-Agent", strlen(user_agent) ? user_agent : DEFAULT_USER_AGENT);
}

protected void perform_request() {
  if (!clonep())
    return;

  string error  = 0;
  mixed  denied = (mixed)(master()->valid_http_request(url, url_data, method));
  if (denied)
    error = stringp(denied) ? denied : "Unable to retrieve url.";

  switch(!error && url_data[URD_PROTOCOL]) {
    case "about":
      error = perform_about_request();
    break;
    case "http":
      error = perform_http_request();
    break;
    case "file":
      error = perform_file_request();
    break;
    default:
      error = "Unknown protocol \"" +url_data[URD_PROTOCOL] +"\".";
    break;
  }

  if (error) {
    client_callback_event(HTTP_FAILED, error);
    clear();
  }
}

protected string perform_about_request() {
  switch(method) {
    case "DELETE" :
    case "OPTIONS":
    case "PUT"    :
    case "TRACE"  :
      return "Method '" +method +"' not available on 'about:' urls.";
  }

  if (url_data[URD_ARGS])
    return "URL arguments not available on 'about:' urls.";
  if (req_content)
    return "Request content not available on 'about:' urls.";

  int* lmod = 0;
  switch(url_data[URD_PATH]) {
    case "blank":
      content = "";
    break;
#if defined(ABOUT_HTTP_FILE)
    case "httpclient":
      content = read_file(ABOUT_HTTP_FILE);
      lmod    = get_dir(ABOUT_HTTP_FILE, GETDIR_DATES);
    break;
#endif
  }

  if (!content)
    return "Unknown 'about' url : " +url_data[URD_PATH];

  client_callback_event(HTTP_GOT_HEADER, ({ "Content-Type", determine_source_type(0, content), }));
  client_callback_event(HTTP_GOT_HEADER, ({ "Content-Length", to_string(strlen(content)), }));

  sizeof(lmod) && intp(lmod[0]) && lmod[0] && client_callback_event(HTTP_GOT_HEADER, ({ "Last-Modified", ctime(lmod[0]), }));

  client_callback_event(HTTP_SUCCESS, ((method == "GET") || (method == "POST")) && content);
  return 0;
}

protected string perform_file_request() {
#if defined(LOG_FILE_REQUESTS)
  object user = this_player() || this_interactive();
  log_message("file-request", sprintf("'%s' requested %#Q.", user ? HTTP_USER_NAME(user) : "<unknown>", url));
#endif

#if defined(ENABLE_FILE_URLS)
  switch(method) {
    case "DELETE" :
    case "OPTIONS":
    case "PUT"    :
    case "TRACE"  :
      return "Method '" +method +"' not available on 'file:' urls.";
  }

  if (url_data[URD_ARGS])
    return "URL arguments not available on 'file:' urls.";
  if (req_content)
    return "Request content not available on 'file:' urls.";

  if (!(master()->valid_read(url_data[URD_PATH], creator(previous_object()), MASTER_VALID_READ_FUNC, previous_object())))
    return "Unable to read path: " +url_data[URD_PATH];

  string error = catch(content = read_file(url_data[URD_PATH]); nolog);
  if (error)
    return "Error reading path: " +trim(error, TRIM_BOTH, "* \n");
  if (!content)
    return "Unable to read path: " +url_data[URD_PATH];

#if 1
  client_callback_event(HTTP_GOT_HEADER, ({ "Content-Type", determine_source_type(url_data[URD_PATH], content), }));
#else
  int    pos = rmember(url_data[URD_PATH], '/');
  string ext = (pos != -1) ? url_data[URD_PATH][pos +1..] : url_data[URD_PATH];
  pos = rmember(ext, '.');
  ext = (pos != -1) && ext[pos +1..];
  switch(ext) {
    case "c"    :
    case "h"    :  ext = "text/lpc";             break;
    case "o"    :  ext = "text/lpc-data";        break;
    case "ini"  :
    case "txt"  :  ext = "text/plain";           break;
    case "htm"  :
    case "html" :  ext = "text/html";            break;
    case "js"   :  ext = "text/javascript";      break;
    case "rdf"  :  ext = "text/rdf";             break;
    case "rss"  :  ext = "text/rss";             break;
    case "sql"  :  ext = "text/sql";             break;
    case "ansi" :
    case "vt"   :
    case "vt100":  ext = "text/ansi";            break;
    case "xml"  :  ext = "text/xml";             break;
    case "wfe"  :  ext = WWW_FORM_CONTENT_TYPE;  break;
    default     :  ext = "text/plain";           break;
  }
  client_callback_event(HTTP_GOT_HEADER, ({ "Content-Type", ext, }));
#endif
  client_callback_event(HTTP_GOT_HEADER, ({ "Content-Length", to_string(strlen(content)), }));

  int* lmod = get_dir(url_data[URD_PATH], GETDIR_DATES);
  sizeof(lmod) && intp(lmod[0]) && lmod[0] && client_callback_event(HTTP_GOT_HEADER, ({ "Last-Modified", ctime(lmod[0]), }));

  client_callback_event(HTTP_SUCCESS, (method != "HEAD") && content);
  return 0;
#else
  return "'file:' protocol is not enabled.";
#endif
}

protected string perform_http_request() {
#if defined(LOG_HTTP_REQUESTS)
  object user = this_player() || this_interactive();
  log_message("http-request", sprintf("'%s' requested %#Q.", user ? HTTP_USER_NAME(user) : "<unknown>", url));
#endif

  string error = socket_connect(url_data[URD_HOST], url_data[URD_PORT], 0);
  if (error)
    return "" +error;

  return 0;
}

protected mixed client_callback_event(int event, mixed data) {
  if (!clonep())
    return 0;
  return funcall(callback, this_object(), event, data);
}

protected void log_message(string context, string message) {
  int    log_time      = time();
  string client_obname = object_name();
  string app_obname    = object_name(to_object(callback));

#if defined(ADVANCED_LOG)
  ADVANCED_LOG(log_time, client_obname, app_obname, context, message);
#endif

  string log_message   = "[" +ctime(log_time) +"] <" +client_obname +":" +app_obname +"> " +context +": " +message +"\n";
#if defined(HTTPCLIENT_LOG)
  HTTPCLIENT_LOG(log_message);
#endif
}

protected void process_request(mixed raw_response) {
  // identify and read the status line.
  if (!raw_response)
    raw_response = "";
  int pos = member(raw_response, '\n');
//(pos == -1) && client_callback_event(HTTP_DEBUG, sprintf("raw_response: %#Q\n", raw_response));
  if (pos == -1) {
    client_callback_event(HTTP_FAILED, "Cannot read HTTP status line.");
    return;
  }

  string status_line = to_string(raw_response[0..pos -1]);
  raw_response = raw_response[pos +1..];
  // fix CRLF or LFCR
  if (sizeof(raw_response) && (raw_response[0] == '\r'))
    raw_response = raw_response[1..];
  else if (strlen(status_line) && (status_line[<1] == '\r'))
    status_line = status_line[0..<2];
//if (url == "http://www.bbc.co.uk/devon/") write_file("/secure/sockets/bin/dev", raw_response);

  string stat_ver  = 0;
  int    stat_code = 0;
  string stat_desc = 0;
  if (sscanf(status_line, "HTTP/%+s%+t%+U%+t%+s", stat_ver, stat_code, stat_desc) != 3) {
    client_callback_event(HTTP_FAILED, "Unable to parse HTTP status line.");
    return;
  }

  status_version = stat_ver;
  status_code    = stat_code;
  status_desc    = stat_desc;
  client_callback_event(HTTP_GOT_STATUS, ({ stat_ver, stat_code, stat_desc, }));

  // now read the response headers and content.
  resp_headers = ([ ]);
  while((pos = member(raw_response, '\n')) != -1) {
    string line = to_string(raw_response[0..pos -1]);
    raw_response = raw_response[pos +1..];
    // fix CRLF or LFCR
    if (sizeof(raw_response) && (raw_response[0] == '\r'))
      raw_response = raw_response[1..];
    if (strlen(line) && (line[<1] == '\r'))
      line = line[0..<2];
    if (line != "") {
      pos = member(line, ':');
      string headkey = (pos == -1) ? line : line[..pos -1];
      string headval = (pos == -1) ? 0    : trim(line[pos +1..]);
      process_header(headkey, headval);
    }
    else {
      content      = raw_response;
      raw_response = "";
    }
  }

  if (pointerp(content)) {
    log_message("content-format",
      sprintf("%s data (size %d).",
        (sizeof(content) == sizeof(filter(content, #'intp))) ? "8 bit" : "mixed",
        sizeof(content)));
  }

  if (member(resp_headers, "transfer-encoding")) {
    string* encodings = filter(map(regexplode(lower_case(resp_headers["transfer-encoding"]), ", *"), #'trim, TRIM_BOTH, ", "), #'strlen);
    for(int index = sizeof(encodings); index-- > 0; )
      switch(encodings[index]) {
        case "":
        case "identity":
          // no decode neccessary
        break;
        case "chunked":
          string error = 0;
          content = process_chunked(content, &error);
          if (error) {
            client_callback_event(HTTP_FAILED, error);
            return;
          }
          encodings[index] = 0;
        break;
        default:
          client_callback_event(HTTP_FAILED, "Unknown Transfer-Encoding \"" +encodings[index] +"\".");
          return;
        break;
      }
    m_add(resp_headers, "transfer-encoding", implode(encodings -({ 0, }), ""));
  }

  switch(status_code /100) {
    case 1:  // Informational.
      process_status_1xx(content);
    break;
    case 2:  // Success.
      process_status_2xx(content);
    break;
    case 3:  // Redirection.
      process_status_3xx(content);
    break;
    case 4:  // Client Error.
      process_status_4xx(content);
    break;
    case 5:  // Server Error.
      process_status_5xx(content);
    break;
    default:
      client_callback_event(HTTP_FAILED, "Unknown status code " +status_code +" (" +status_desc +").");
    break;
  }
}

protected void process_header(string header, string value) {
  string low_header = lower_case(header);
  switch(low_header) {
    case "accept-ranges":
    break;
    case "cache-control":
    break;
    case "connection":
    break;
    case "content-encoding":
      if (member(resp_headers, low_header))
        value = resp_headers[low_header] +", " +value;
    break;
    case "content-language":
    break;
    case "content-length":
    break;
    case "content-location":
    break;
    case "content-type":
    break;
    case "date":
    break;
    case "etag":
    break;
    case "expires":
    break;
    case "keep-alive":
    break;
    case "last-modified":
    break;
    case "location":
    break;
    case "server":
    break;
    case "set-cookie":
//object bubbs = find_player("bubbs");
//bubbs && tell_object(bubbs, sprintf("httpclient::set-cookie: value=%#Q\n", value));
      process_cookies(value);
    break;
    case "pragma":
    break;
    case "transfer-encoding":
      if (member(resp_headers, low_header))
        value = resp_headers[low_header] +", " +value;
    break;
    case "vary":
    break;
    case "www-authenticate":
    break;
    case "x-aspnet-version":
    case "x-powered-by":
      // Ignored.
    break;
    default:
      log_message("receive-header", sprintf("header %#Q %#Q.", header, value));
    break;
  }
  client_callback_event(HTTP_GOT_HEADER, ({ header, value, }));
  // perhaps the callback should actually be a check for 'accept header'.
  m_add(resp_headers, low_header, value);
}

protected void process_cookies(string cookie_text) {
//object bubbs = find_player("bubbs");
//bubbs && tell_object(bubbs, sprintf("httpclient process_cookies(%#Q);\n", cookie_text));
  mixed* new_cookies       = ({ });
  mixed* blank_cookie      = allocate(COOKIE_SIZE);
  blank_cookie[CK_EXPIRES] = -1;
  blank_cookie[CK_DOMAIN]  = url_data[URD_HOST];
  blank_cookie[CK_PATH]    = url_data[URD_PATH];

  mixed*  cookie = deep_copy(blank_cookie);
  string* bits   = filter(map(regexplode(cookie_text, "; *"), #'trim, TRIM_BOTH, "; "), #'strlen);
  foreach(string bit : bits) {
    int    pos   = member(bit, '=');
    string name  = bit;
    string value = 0;
    if (pos != -1) {
      name = bit[0..pos -1];
      value = bit[pos +1..];
    }
    switch(lower_case(name)) {
      case "expires":
        cookie[CK_EXPIRES] = (value == "-1") ? -1 : parse_cookie_date(value);
        if (!cookie[CK_EXPIRES]) {
          log_message("parse-cookie-date", sprintf("unparsable %#Q.", value));
          cookie[CK_EXPIRES] = -1;
        }
      break;
      case "domain":  cookie[CK_DOMAIN] = value;  break;
      case "path"  :  cookie[CK_PATH]   = value;  break;
      case "secure":  cookie[CK_SECURE] = 1;      break;
      default:
        if (cookie[CK_NAME] && cookie[CK_VALUE]) {
//object bubbs = find_player("bubbs");
//bubbs && tell_object(bubbs, sprintf("httpclient: internal save cookie %#Q\n", cookie));
          int keep = 1;
          if ((cookie[CK_EXPIRES] != "-1") && (cookie[CK_EXPIRES] < time()))
            keep = 0;
          //else if ((cookie[CK_EXPIRES] != -1) && (cookie[CK_EXPIRES] > MAX_COOKIE_EXPIRY_TIME))
          //  keep = 0;
#if 1
          else if (cookie[CK_DOMAIN] && (lower_case(cookie[CK_DOMAIN]) != lower_case(__HOST_NAME__))) {
            string* dom_bits = explode(lower_case(cookie[CK_DOMAIN]), ".");
            if (sizeof(dom_bits) &&
                ((dom_bits[<1] == "com") || (dom_bits[<1] == "edu") || (dom_bits[<1] == "net") ||
                 (dom_bits[<1] == "org") || (dom_bits[<1] == "gov") || (dom_bits[<1] == "mil") ||
                 (dom_bits[<1] == "int")))
              keep = (sizeof(dom_bits) > 1);
            else
              keep = (sizeof(dom_bits) > 2);
          }
#endif
          if (keep)
            new_cookies += ({ cookie, });
          cookie = deep_copy(blank_cookie);
        }
        cookie[CK_NAME]  = name;
        cookie[CK_VALUE] = value;
      break;
    }
  }
  if (cookie[CK_NAME] && cookie[CK_VALUE]) {
//object bubbs = find_player("bubbs");
//bubbs && tell_object(bubbs, sprintf("httpclient: final save cookie %#Q\n", cookie));
    int keep = 1;
    if ((cookie[CK_EXPIRES] != -1) && (cookie[CK_EXPIRES] < time()))
      keep = 0;
    //else if ((cookie[CK_EXPIRES] != -1) && (cookie[CK_EXPIRES] > MAX_COOKIE_EXPIRY_TIME))
    //  keep = 0;
#if 1
    else if (cookie[CK_DOMAIN] && (lower_case(cookie[CK_DOMAIN]) != lower_case(__HOST_NAME__))) {
      string* dom_bits = explode(lower_case(cookie[CK_DOMAIN]), ".");
      if (sizeof(dom_bits) &&
          ((dom_bits[<1] == "com") || (dom_bits[<1] == "edu") || (dom_bits[<1] == "net") ||
           (dom_bits[<1] == "org") || (dom_bits[<1] == "gov") || (dom_bits[<1] == "mil") ||
           (dom_bits[<1] == "int")))
        keep = (sizeof(dom_bits) > 1);
      else
        keep = (sizeof(dom_bits) > 2);
    }
#endif
    if (keep)
      new_cookies += ({ cookie, });
    cookie = deep_copy(blank_cookie);
  }

//bubbs && tell_object(bubbs, sprintf("httpclient::set-cookie: new_cookies=%#Q\n", new_cookies));
  if (sizeof(new_cookies)) {
    cookies ||= ({ });
    foreach(mixed* new_cookie : new_cookies) {
//bubbs && tell_object(bubbs, sprintf("httpclient::set-cookie: new_cookie=%#Q\n", new_cookie));
      int updated = 0;
      if (!client_callback_event(HTTP_KEEP_COOKIE, new_cookie))
        updated = 1;
      if (!updated)
        foreach(cookie : cookies)
          if ((cookie[CK_NAME] == new_cookie[CK_NAME]) && (cookie[CK_DOMAIN] == new_cookie[CK_DOMAIN])) {
            cookie[CK_VALUE]   = new_cookie[CK_VALUE];
            cookie[CK_EXPIRES] = new_cookie[CK_EXPIRES];
            cookie[CK_SECURE]  = new_cookie[CK_SECURE];
            cookie[CK_PATH]    = new_cookie[CK_PATH];
            updated = 1;
          }
      if (!updated)
        cookies += ({ new_cookie, });
    }
    client_callback_event(HTTP_SET_COOKIES, cookies);
  }
}

protected mapping process_authenticate(string authenticate, string error) {
  mapping user_pass   = ([ ]);

  string  chal_type   = 0;
  string  chal_realm  = 0;
  mapping chal_props  = ([ ]);

  string  key_word    = 0;
  string  curr_word   = "";
  string  quoted_word = 0;
  int     quoted_char = 0;
  int     size        = strlen(authenticate);
  for(int index = 0; (index < size) && user_pass && !sizeof(user_pass); index++) {
    string auth_char = authenticate[index..index];
    string new_word  = 0;
    if (quoted_word != 0) {
      if (quoted_char) {
        quoted_char  = 0;
        quoted_word += auth_char;
      }
      else if (auth_char == "\\")
        quoted_char  = 1;
      else if (auth_char == "\"") {
        new_word = quoted_word;
        quoted_word  = 0;
      }
      else
        quoted_word += auth_char;
    }
    else if (auth_char == "\"")
      quoted_word  = "";
    else if (auth_char == "=") {
      key_word  = curr_word;
      curr_word = "";
    }
    else if ((auth_char == " ") || (auth_char == ",")) {
      if (strlen(curr_word)) {
        new_word  = curr_word;
        curr_word = "";
      }
    }
    else {
      curr_word += auth_char;
      if (index == size -1)
        new_word = curr_word;
    }

    int check = 0;
    if (new_word && key_word) {
      if (lower_case(key_word) == "realm")
        chal_realm = new_word;
      else
        m_add(chal_props, key_word, new_word);
      key_word = 0;
      new_word = 0;
      check = (index == size -1);
    }

    switch(check && chal_type && lower_case(chal_type)) {
      case 0:
        // ignored.
      break;
      case "basic":
        mixed up_data = client_callback_event(HTTP_AUTHENTICATE, ({ url_data[URD_HOST], chal_realm, }));
        if (!up_data) {
          error = "Unable to authenticate \"" +url +"\" - authentication failed.";
          return 0;
        }
        else if (stringp(up_data)) {
          foreach(string up : map(regexplode(up_data, "[\t\n ,]+"), #'trim, TRIM_BOTH, ",\t\n "))
            if (strlen(up) && (up[0] != '#')) {
              int pos = strstr(up, ":");
              if (pos != -1)
                m_add(user_pass, up[0..pos -1], up[pos +1..]);
            }
        }
        else if (mappingp(up_data))
          user_pass = filter(up_data, (: stringp($1) && stringp($2) :));
        else if (intp(up_data))
          return 0;
        else {
          error = "Unable to authenticate \"" +url +"\" - unrecognised callback result.";
          return 0;
        }
      break;
      default:
        error = "Unable to authenticate \"" +url +"\" - unknown type \"" +chal_type +"\".";
        return 0;
      break;
    }

    if (new_word) {
      chal_type  = new_word;
      chal_props = ([ ]);
    }
  }

  if (quoted_char || quoted_word) {
    error = "Unable to authenticate \"" +url +"\" - unterminated string in WWW-Authenticate.";
    return 0;
  }

  return user_pass;
}

protected string process_chunked(string content, string error) {
  string entity    = "";
  int    chunk_num = 1;
  int    length    = 0;

  int pos  = strstr(content, "\n\r");
  int pos2 = strstr(content, "\r\n");
  if ((pos == -1) || ((pos2 != -1) && (pos2 < pos)))
    pos = pos2;
  if (pos == -1) {
    error = "Chunk " +chunk_num +" - cannot read chunk header.";
    return 0;
  }
  string chunk_info = content[0..pos -1];
  content = content[pos +2..];

  pos = strstr(chunk_info, ";");
  if (pos != -1) {
    string ext = trim(chunk_info[pos +1..]);
    chunk_info = trim(chunk_info[0..pos -1]);
    error = "Chunk " +chunk_num +" - extension \"" +ext +"\" not supported.";
    return 0;
  }

  int chunk_size = hex_to_int(chunk_info);
  if (chunk_size < 0) {
    error = "Chunk " +chunk_num +" - invalid chunk size \"" +chunk_info +"\".";
    return 0;
  }
  while(chunk_size > 0) {
    entity += content[0..chunk_size -1];
    content[0..chunk_size -1] = "";
    length += chunk_size;

    if (!strstr(content, "\n\r") || !strstr(content, "\r\n"))
      content[0..1] = "";
    else {
      error = "Chunk " +chunk_num +" - cannot read chunk data.";
      return 0;
    }

    chunk_num++;

    pos  = strstr(content, "\n\r");
    pos2 = strstr(content, "\r\n");
    if ((pos == -1) || ((pos2 != -1) && (pos2 < pos)))
      pos = pos2;
    if (pos == -1) {
      error = "Chunk " +chunk_num +" - cannot read chunk header.";
      return 0;
    }
    chunk_info = content[0..pos -1];
    content = content[pos +2..];

    pos = strstr(chunk_info, ";");
    if (pos != -1) {
      string ext = trim(chunk_info[pos +1..]);
      chunk_info = trim(chunk_info[0..pos -1]);
      error = "Chunk " +chunk_num +" - extension \"" +ext +"\" not supported.";
      return 0;
    }

    chunk_size = hex_to_int(chunk_info);
    if (chunk_size < 0) {
      error = "Chunk " +chunk_num +" - invalid chunk size \"" +chunk_info +"\".";
      return 0;
    }
  }

  m_add(resp_headers, "content-length", length);

  // optional headers.
  mixed* optional_headers = ({ });
  while((pos = strstr(content, "\n")) > 0) {
    string line = content[0..pos -1];
    content = content[pos +1..];
    // fix CRLF or LFCR
    if (strlen(content) && (content[0] == '\r'))
      content = content[1..];
    else if (strlen(line) && (line[<1] == '\r'))
      line = line[0..<2];
    if (strlen(line) > 0) {
      pos = strstr(line, ":");
      string headkey = (pos == -1) ? line : line[..pos -1];
      string headval = (pos == -1) ? 0    : trim(line[pos +1..]);
      string lowkey  = lower_case(headkey);
      log_message("dechunk-header", sprintf("optional header %#Q %#Q.", headkey, headval));
      process_header(lowkey, headval);
    }
  }

  return entity;
}

protected void process_status_1xx(string content) {
  // Informational.
  switch(status_code) {
    case 100:  // Continue.
      // ignored.
    break;
    case 101:  // Switching Protocols.
      string protocol = resp_headers["upgrade"];
      if (!member(resp_headers, "upgrade"))
        client_callback_event(HTTP_FAILED, "Cannot switch protocol; no protocol supplied.");
      else {
        log_message("status-101", sprintf("unable to switch protocol %#Q.", resp_headers["upgrade"]));
        client_callback_event(HTTP_FAILED, "Cannot switch protocol; \"" +resp_headers["upgrade"] +"\" no supported.");
      }
    break;
    default:
      log_message("status-1xx", sprintf("unknown status code %d (%#Q).", status_code, status_desc));
      client_callback_event(HTTP_FAILED, "Unknown status code " +status_code +" (" +status_desc +").");
    break;
  }
}

protected void process_status_2xx(string content) {
  // Success.
  switch(status_code) {
    case 200:  // OK.
      client_callback_event(HTTP_SUCCESS, content);
    break;
    case 201:  // Created.
      // New resource created; entity contains list of places to find it.
      // resp_headers["location"] gives the most specific URI.
      // resp_headers["etag"] gives an optional entity-tag.
      if (!member(resp_headers, "location"))
        client_callback_event(HTTP_SUCCESS, content);
      else {
        string u      = apply_base_url(resp_headers["location"], 0);
        mixed  udata = parse_url(u);
        if (stringp(udata))
          client_callback_event(HTTP_FAILED, "Failed to parse redirect \"" +u +"\" (from \"" +url +"\") - " +udata);
        else if (client_callback_event(HTTP_REDIRECT, u = unparse_url(udata))) {
          string etag = resp_headers["etag"];
          init_request(method, u, udata, req_content, req_headers["content-type"], callback);
          etag && m_add(req_headers, "ETag", etag);
          call_out(#'perform_request, 0);
        }
      }
    break;
    case 202:  // Accepted.
      // Request is accepted, but not completed.
      // It may or may not be processed.
      // Content should contain a description.
      client_callback_event(HTTP_SUCCESS, content);
    break;
    case 203:  // Non-Authoritative Information.
      // Content is kinda made up, but should be good to go.
      client_callback_event(HTTP_SUCCESS, content);
    break;
    case 204:  // No Content.
      // Silent 'it worked, don't change the view'.
      // New meta information should still be applied, however.
      // Hmm... HTTP_SUCCESS_NO_CONTENT ?  Ick.
      client_callback_event(HTTP_SUCCESS, 0);
      //client_callback_event(HTTP_FAILED, "Status 204 No Content.");
    break;
    case 205:  // Reset Content.
      // Not as silent 'it worked, reset the view'.
      // Hmmm...
      //client_callback_event(HTTP_FAILED, "Status 205 Reset Content.");
      client_callback_event(HTTP_SUCCESS, content);
    break;
    case 206:  // Partial Content.
      // Fulfilled a partial GET.
      client_callback_event(HTTP_FAILED, "Status 206 Partial Content.");
    break;
    default:
      log_message("status-2xx", sprintf("Unknown status code %d (%#Q).", status_code, status_desc));
      //client_callback_event(HTTP_FAILED, "Unknown status code " +status_code +" (" +status_desc +").");
      client_callback_event(HTTP_SUCCESS, content);
    break;
  }
}

protected void process_status_3xx(string content) {
  // Redirection.
  string redirect_location = 0;
  switch(status_code) {
    case 300:  // Multiple Choices.
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Status 300 Multiple Choices - No Location.");
    break;
    case 301:  // Moved Permanently.
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Status 301 Moved Permanently - No Location.");
    break;
    case 302:  // Found.
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Status 302 Found - No Location.");
    break;
    case 303:  // See Other.
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Status 302 Found - No Location.");
    break;
    case 304:  // Not Modified.
      // Used for 'silent not updated' flag for conditional GETs.
      client_callback_event(HTTP_FAILED, "Status 304 Not Modified.");
    break;
    case 305:  // Use Proxy.
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Status 305 Use Proxy - No Location.");
    break;
    case 306:  // (Unused).
      client_callback_event(HTTP_FAILED, "Status 306 (Unused).");
    break;
    case 307:  // Temporary Redirect.
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Status 307 Temporary Redirect - No Location.");
    break;
    default:
      log_message("status-3xx", sprintf("Unknown status code %d (%#Q).", status_code, status_desc));
      if (member(resp_headers, "location"))
        redirect_location = resp_headers["location"];
      else
        client_callback_event(HTTP_FAILED, "Unknown status code " +status_code +" (" +status_desc +").");
    break;
  }
  if (redirect_location) {
    string u     = apply_base_url(redirect_location, 0);
    mixed  udata = parse_url(u);
    if (stringp(udata))
      client_callback_event(HTTP_FAILED, "Failed to parse redirect \"" +u +"\" (from \"" +url +"\") - " +udata);
    else if (client_callback_event(HTTP_REDIRECT, u = unparse_url(udata))) {
      init_request(method, u, udata, req_content, req_headers["content-type"], callback);
      call_out(#'perform_request, 0);
    }
  }
}

protected void process_status_4xx(string content) {
  // Client Error.
  switch(status_code) {
    case 400:  // Bad Request.
      client_callback_event(HTTP_FAILED, "Status 400 Bad Request.");
    break;
    case 401:  // Unauthorized.
      if (!member(resp_headers, "www-authenticate"))
        client_callback_event(HTTP_FAILED, "Status 401 Unauthorized - No authenticate.");
      else if (member(map(m_indices(req_headers), #'lower_case), "authorization") != -1)
        client_callback_event(HTTP_FAILED, "Status 401 Unauthorized - Authorization failed.");
      else {
        string  auth_error = 0;
        mapping user_pass  = process_authenticate(resp_headers["www-authenticate"], &auth_error);
        if (auth_error) {
          client_callback_event(HTTP_FAILED, auth_error);
          return;
        }
        else if (!user_pass)
          return;
        else if (!sizeof(user_pass))
          client_callback_event(HTTP_FAILED, "Unable to authenticate \"" +url +"\" - no username/password supplied.");
        else {
          string auth = m_indices(user_pass)[0];
          m_add(req_headers, "Authorization", "Basic " +encode_base64(auth + ":" +user_pass[auth]));
          // can we give all username/password pairs?
          //repeat_request();
          call_out(#'perform_request, 0);
        }
      }
    break;
    case 402:  // Payment Required.
      client_callback_event(HTTP_FAILED, "Status 402 Payment Required.");
    break;
    case 403:  // Forbidden.
    client_callback_event(HTTP_FAILED, "Status 403 Forbidden.");
    break;
    case 404:  // Not Found.
      client_callback_event(HTTP_FAILED, "Status 404 Not Found.");
    break;
    case 405:  // Method not ot aallowed.
      client_callback_event(HTTP_FAILED, "Status 405 Method Not Allowed.");
    break;
    case 406:  // Not Accetpable.
      client_callback_event(HTTP_FAILED, "Status 406 Not Acceptable.");
    break;
    case 407:  // Proxy Authentication Required.
      client_callback_event(HTTP_FAILED, "Status 407 Proxy Authentication Required.");
    break;
    case 408:  // Request Timeout.
      client_callback_event(HTTP_FAILED, "Status 408 Request Timeout.");
    break;
    case 409:  // Conflict.
      client_callback_event(HTTP_FAILED, "Status 409 Conflict.");
    break;
    case 410:  // Gone.
      client_callback_event(HTTP_FAILED, "Status 410 Gone.");
    break;
    case 411:  // Length Required.
      client_callback_event(HTTP_FAILED, "Status 411 Length Required.");
    break;
    case 412:  // Precondition Failed.
      client_callback_event(HTTP_FAILED, "Status 412 Precondition Failed.");
    break;
    case 413:  // Request Entity Too Large.
      client_callback_event(HTTP_FAILED, "Status 413 Request Entity Too Large.");
    break;
    case 414:  // Request-URI Too Long.
      client_callback_event(HTTP_FAILED, "Status 414 Request-URI Too Long.");
    break;
    case 415:  // Unsupported Media Type.
      client_callback_event(HTTP_FAILED, "Status 415 Unsupported Media Type.");
    break;
    case 416:  // Requested Range Not Satisfiable.
      client_callback_event(HTTP_FAILED, "Status 416 Requested Range Not Satisfiable.");
    break;
    case 417:  // Expectation Failed.
      client_callback_event(HTTP_FAILED, "Status 417 Expectation Failed.");
    break;
    default:
     log_message("status-4xx", sprintf("Unknown status code %d (%#Q).", status_code, status_desc));
      client_callback_event(HTTP_FAILED, "Unknown status code " +status_code +" (" +status_desc +").");
    break;
  }
}

protected void process_status_5xx(string content) {
  // Server Error.
  switch(status_code) {
    case 500:  // Internal Server Error.
      client_callback_event(HTTP_FAILED, "Status 500 Internal Server Error.");
    break;
    case 501:  // Not Implemented.
      client_callback_event(HTTP_FAILED, "Status 501 Not Implemented.");
    break;
    case 502:  // Bad Gateway.
      client_callback_event(HTTP_FAILED, "Status 502 Bad Gateway.");
    break;
    case 503:  // Service Unavailable.
      client_callback_event(HTTP_FAILED, "Status 503 Service Unavailable.");
    break;
    case 504:  // Gateway Timeout.
      client_callback_event(HTTP_FAILED, "Status 504 Gateway Timeout.");
    break;
    case 505:  // HTTP Version Not Supported.
      client_callback_event(HTTP_FAILED, "Status 505 HTTP Version Not Supported..");
    break;
    default:
      log_message("status-5xx", sprintf("Unknown status code %d (%#Q).", status_code, status_desc));
      client_callback_event(HTTP_FAILED, "Unknown status code " +status_code +" (" +status_desc +").");
    break;
  }
}


/// httpclient public utilty methods.

public mixed parse_url(string u) {
  if (!u && url)
    u = url;

  if (!stringp(u) || !strlen(u))
    return "No URL given.";

  u = trim(u);
  if (u[0] == '/')
    return parse_file_url(u);

  string error = 0;
  int    colon = member(u, ':');
  mixed* udata = 0;
  switch(colon) {
    case -1:
#if defined(DEFAULT_PROTOCOL)
      udata = parse_url(DEFAULT_PROTOCOL +":" +u);
#else
      error = "No protocol defined.";
#endif
    break;
    case 0:
#if defined(DEFAULT_PROTOCOL)
      udata = parse_url(DEFAULT_PROTOCOL +u);
#else
      error = "Protocol not defined.";
#endif
    break;
    default:
      switch(u[..colon -1]) {
        case "about":  udata = parse_about_url(u);         break;
        case "file" :  udata = parse_file_url(u);          break;
        case "http" :  udata = parse_http_url(u);          break;
        default     :  error = "Protocol not supported.";  break;
      }
    break;
  }

  return error || udata;
}

public mixed parse_about_url(string target_url) {
  if (strstr(target_url, "about:"))
    return "Badly formed 'about:' URL.";

  int pos = member(target_url, '?');
  mixed udata = allocate(URL_DATA_SIZE);
  udata[URD_PROTOCOL] = "about";
  udata[URD_HOST]     = "";
  udata[URD_PORT]     = 0;
  udata[URD_PATH]     = decode_www_form(trim((pos == -1) ? target_url[6..] : target_url[6..pos -1], TRIM_LEFT, ":"));
  udata[URD_ARGS]     = (pos == -1) ? 0 : target_url[pos +1..];
  return udata;
}

// file url format:
//   absolute path  OR  file:[/] www form encoded absolute path
// url_data format:
//   ({ protocol, "", 0, path, args  })
public mixed parse_file_url(string u) {
  mixed* fdata = explode(regreplace(u, FILE_URL_REGEXP, "\\1\n\\2\n\\4", 1), "\n");
//printf("parse_file_url(%#Q); fdata=%#Q\n", u, fdata);
  if (sizeof(fdata) < 3) {
    if (u[0] == '/') {
      fdata = allocate(3);
      int pos = strstr(u, "?");
      if (pos == -1)
        fdata[1] = u;
      else {
        fdata[1] = u[..pos -1];
        fdata[2] = u[pos +1..];
      }
    }
    else
      return "Badly formed 'file:' URL.";
  }
  if (fdata[0])
    fdata[1] = decode_www_form(fdata[1]);
  if (!strstr(fdata[1], "//"))
    fdata[1] = fdata[1][1..];
  mixed* udata = allocate(URL_DATA_SIZE);
  udata[URD_PROTOCOL] = "file";
  udata[URD_HOST]     = "";
  udata[URD_PORT]     = 0;
  udata[URD_PATH]     = fdata[1];
  udata[URD_ARGS]     = strlen(fdata[2]) && fdata[2];
  return udata;
}

// http url format:
//   protocol : // host [ : port ] [ abs_path [ ? args ] ]     ? [ # ref ]
// url_data format:
//   ({ protocol, host, port, path, args })
public mixed parse_http_url(string u) {
  mixed udata = explode(regreplace(u, HTTP_URL_REGEXP, "\\1\n\\2\n\\4\n\\5\n\\7", 1), "\n");
  if (sizeof(udata) < 5)
    return "Badly formed 'http:' URL.";
  if (udata[URD_PROTOCOL] == "")
    return "Protocol not defined.";
  if (udata[URD_HOST] == "")
    return "Host not defined.";
  udata[URD_PORT] = (udata[URD_PORT] == "") ? DEFAULT_HTTP_PORT : to_int(udata[URD_PORT]);
  if (udata[URD_PATH] == "")
    udata[URD_PATH] = "/";
  if (!strlen(udata[URD_ARGS]))
    udata[URD_ARGS] = 0;
  return udata;
}

public string unparse_url(mixed* url_data) {
  string url = 0;
  if (pointerp(url_data) && (sizeof(url_data) >= URL_DATA_SIZE) && stringp(url_data[0]))
    switch(url_data[0]) {
      case "about":  url = unparse_about_url(url_data);  break;
      case "file" :  url = unparse_file_url(url_data);   break;
      case "http" :  url = unparse_http_url(url_data);   break;
    }
 return url;
}

public string unparse_about_url(mixed* url_data) {
  return
    ((url_data[URD_HOST] == "") && !url_data[URD_PORT] && stringp(url_data[URD_PATH]) && stringp(url_data[URD_ARGS] || "")) &&
    (url_data[URD_PROTOCOL] +":" +(url_data[URD_PATH]) +(url_data[URD_ARGS] ? ("?" +url_data[URD_ARGS]) : ""));
}

public string unparse_file_url(mixed* url_data) {
  return
    ((url_data[URD_HOST] == "") && !url_data[URD_PORT] && stringp(url_data[URD_PATH]) && stringp(url_data[URD_ARGS] || "")) &&
    (url_data[URD_PROTOCOL] +":" +(url_data[URD_PATH]) +(url_data[URD_ARGS] ? ("?" +url_data[URD_ARGS]) : ""));
}

public string unparse_http_url(mixed* url_data) {
  return
    (stringp(url_data[URD_HOST]) && intp(url_data[URD_PORT]) && stringp(url_data[URD_PATH]) && stringp(url_data[URD_ARGS] || "")) &&
    (url_data[URD_PROTOCOL] +"://" +url_data[URD_HOST] +((url_data[URD_PORT] == DEFAULT_HTTP_PORT) ? "" : (":" +url_data[URD_PORT])) +url_data[URD_PATH] +(url_data[URD_ARGS] ? ("?" +url_data[URD_ARGS]) : ""));
}

public int parse_cookie_date(string date_text) {
  string* date_formats = COOKIE_DATE_FORMATS;
  int     size         = sizeof(date_formats);
  mapping date_bits    = 0;
  for(int index = 0; (index < size) && !date_bits; index++)
    date_bits = parse_time(date_text, date_formats[index]);

  if (!date_bits)
    return 0;
//debug_message(sprintf("parse_cookie_date(%#Q): %#Q\n", date_text, date_bits));
//map(filter(users(), #'call_other, "id", "bubbs"), #'tell_object, sprintf("parse_cookie_date(%#Q): %#Q\n", date_text, date_bits));

  // date should be UTC, not mud zone based.
  date_bits["ZONE"] = 0.0;

  return mktime(date_bits);
}

public mapping read_source_types(string data_file) {
  string  file_data   = stringp(data_file) && read_file(data_file);
  if (!file_data)
    return 0;

  mapping file_stypes = ([:SOURCE_TYPES_WIDTH ]);
  foreach(string line : explode(file_data, "\n"))
    if (strlen(line) && strstr(line, ";") && strstr(line, "#")) {
      int     pos   = member(line, '=');
      string  stype = (pos != -1) && line[..pos -1];
      string* bits  = (pos != -1) && pos && explode(line[pos +1..], ":");
      pos = sizeof(bits);
//printf("stype=%#Q; sizeof(bits)=%#Q; bits=%#Q\n", stype, pos, bits);
      if (line && (pos > SRCTYP_DESCRIPTION) && strlen(bits[SRCTYP_DESCRIPTION])) {
        m_add(file_stypes, stype);
        file_stypes[stype, SRCTYP_DESCRIPTION]     = bits[SRCTYP_DESCRIPTION];
        file_stypes[stype, SRCTYP_FILE_EXTENSIONS] =
          (pos > SRCTYP_FILE_EXTENSIONS) &&
          filter(map(explode(bits[SRCTYP_FILE_EXTENSIONS], ","), #'trim, TRIM_BOTH, " \t."), #'strlen);
        file_stypes[stype, SRCTYP_MATCH_REGEXP]    =
          (pos > SRCTYP_MATCH_REGEXP) &&
          unquote_string(implode(bits[SRCTYP_MATCH_REGEXP..], ":"));
//printf("%#Q\n", file_stypes & ({ stype, }));
     }
    }

  return sizeof(file_stypes) ? file_stypes : (source_types || ([:SOURCE_TYPES_WIDTH ]));
}

public string determine_source_type(string path, string source) {
  source_types ||= read_source_types(DEFAULT_SOURCE_TYPES_FILE);

  string source_type = 0;

  if (source_types) {
    int    pos  = stringp(path) ? rmember(path, '/') : -1;
    string name = (pos != -1) ? path[pos +1..] : path;
    pos = name ? rmember(name, '.') : -1;
    string ext  = (pos != -1) && name[pos +1..];
    int    len  = stringp(source) && strlen(source);

    foreach(string stype : source_types)
      source_type ||=
        ((ext && source_types[stype, SRCTYP_FILE_EXTENSIONS] && (member(source_types[stype, SRCTYP_FILE_EXTENSIONS], ext) != -1)) ||
         (len && source_types[stype, SRCTYP_MATCH_REGEXP] && (strlen(regreplace(source, source_types[stype, SRCTYP_MATCH_REGEXP], "", 0)) != len))) &&
        stype;

    if (!source_type)
      foreach(string stype : source_types)
        source_type ||=
          source_types[stype, SRCTYP_FILE_EXTENSIONS] && (member(source_types[stype, SRCTYP_FILE_EXTENSIONS], "*") != -1) && stype;
  }

  return source_type || DEFAULT_SOURCE_TYPE;
}

public string unquote_string(string str) {
  string output = "";
  int    length = strlen(str);
  for(int index = 0; index < length; index++) {
    if (str[index] == '\\')
      switch((++index < length) && str[index]) {
        case 0  :                                break;
        case 'a':  output += "\a";               break;
        case 'b':  output += "\b";               break;
        case 'e':  output += "\e";               break;
        case 'f':  output += "\f";               break;
        case 'n':  output += "\n";               break;
        case 'r':  output += "\r";               break;
        case 't':  output += "\t";               break;
        case '0'..'9':
          string numeric = regreplace(str[index..], "^([0-9](([0-9]([0-9]|))|)).*$", "\\1", 0);
          output += sprintf("%c", to_int(numeric));
          index  += strlen(numeric) -1;
        break;
        default :  output += str[index..index];  break;
      }
    else
      output += str[index..index];
  }
  return output;
}

public string encode_www_form(mixed value, varargs string* excludes) {
  string exclude = implode(filter(excludes, #'stringp), "");
  string output  = 0;
  if (stringp(value))
    for(int index = strlen(output = to_string(value)); index-- > 0; )
      switch(output[index]) {
        case 'a'..'z':
        case 'A'..'Z':
        case '0'..'9':
        case '.':
        case '-':
        case '*':
        case '_':  /* remain the same. */  break;
        case ' ':  output[index] = '+';    break;
        default:
          if (member(exclude, output[index]) == -1)
            output[index..index] = sprintf("%%%02X", output[index]);
        break;
      }
  else if (!mappingp(value))
    output = encode_www_form(to_string(value));
  else
    foreach(mixed key : value)
      if (stringp(key))
        output = (output ? (output +"&") : "") +encode_www_form(key, exclude) +"=" +encode_www_form(value[key], exclude);

  return output;
}

public string decode_www_form(string value) {
  string out = value;
  for(int index = strlen(out); index-- > 0; )
    switch(out[index]) {
      case 'a'..'z':
      case 'A'..'Z':
      case '0'..'9':
      case '/':
      case '*':
      case '-':
      case '.':
      case ':':
      case '=':
      case '_':  /* remain the same. */  break;
      case '+':  out[index] = ' ';       break;
      case '%':
        int num = hex_to_int(out[index +1..index +2]);
        if (num < 0)
          ;//raise_error("decode_www_form(): cannot decode \"" +out[index..index +2] +"\".\n");
        out[index +1..index +2] = "";
        out[index] = num;
      break;
      default:
        // ack.
        //raise_error("decode_www_form(): cannot decode character '\\" +out[index] +"'.\n");
      break;
    }
  return out;
}

public string encode_base64(string value) {
  string alphabet = BASE64_ALPHABET;
  string output   = "";
  int    size     = strlen(value);
  for(int index = 0; (index < size) && output; index += 3) {
    string inbit  = value[index..min(index +2, size -1)];
    string outbit = "====";
    switch(strlen(inbit)) {
      case 0:
        // ignored.
      break;
      case 1:
        outbit[0] = alphabet[inbit[0] >>2];
        outbit[1] = alphabet[((inbit[0] &0b0000011) <<4)];
      break;
      case 2:
        outbit[0] = alphabet[inbit[0] >>2];
        outbit[1] = alphabet[((inbit[0] &0b0000011) <<4) |(inbit[1] >>4)];
        outbit[2] = alphabet[((inbit[1] &0b0001111) <<2)];
      break;
      case 3:
        outbit[0] = alphabet[inbit[0] >>2];
        outbit[1] = alphabet[((inbit[0] &0b0000011) <<4) |(inbit[1] >>4)];
        outbit[2] = alphabet[((inbit[1] &0b0001111) <<2) |(inbit[2] >>6)];
        outbit[3] = alphabet[((inbit[2] &0b0111111) <<0)];
      break;
      default:
        output = 0;
      break;
    }
    if (output)
      output += outbit;
  }
  return output;
}

public string decode_base64(string value) {
  string alphabet = BASE64_ALPHABET;
  string output   = "";
  int    size     = strlen(value);
  for(int index = 0; (index < size) && output; index += 4) {
    string inbit  = value[index..min(index +3, size -1)];
    inbit += "=" *(4 -strlen(inbit));
    int inch0 = member(alphabet, inbit[0]);
    int inch1 = member(alphabet, inbit[1]);
    int inch2 = member(alphabet, inbit[2]);
    int inch3 = member(alphabet, inbit[3]);
    if (inch3 != -1)
      output +=
        sprintf("%c%c%c",
          ((inch0 &0b111111) <<2) |((inch1 &0b110000) >>4),
          ((inch1 &0b001111) <<4) |((inch2 &0b111100) >>2),
          ((inch2 &0b000011) <<6) |((inch3 &0b111111) >>0));
    else if (inch2 != -1)
      output +=
        sprintf("%c%c",
          ((inch0 &0b111111) <<2) |((inch1 &0b110000) >>4),
          ((inch1 &0b001111) <<4) |((inch2 &0b111100) >>2));
    else if (inch1 != -1)
      output +=
        sprintf("%c",
          ((inch0 &0b111111) <<2) |((inch1 &0b110000) >>4));
  }

  return output;
}

public int hex_to_int(string hex) {
  return stringp(hex) && strlen(hex) && to_int("0x" +regreplace(hex, "[^0-9A-Da-f]+", "", 1));
}

public string int_to_hex(int number) {
  return intp(number) && sprintf("%x", number);
}



/// mudlib interface methods.

public int deny_update() { return query_busy(); }

public int clean_up(int arg) {
  if (deny_update())
    return 1;
  destruct(this_object());
  return 0;
}

public void destructor() {
  (previous_object() == master()) && abort_request();
}



