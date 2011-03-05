//// tcp_socket
//
// tcp socket handler
//
// Bubbs, Dec/2006.

// this version uses a socket daemon.
// it should be possible to tweak to use net_connect().


/// pragmas.

#pragma combine_strings
#pragma local_scopes
#pragma no_clone
#pragma no_shadow
#pragma no_warn_empty_casts
#pragma pedantic
#pragma save_types
#pragma strict_types
#pragma verbose_errors
#pragma warn_deprecated
#pragma warn_function_inconsistent


/// tcp_socket includes.

#if defined(TIMEWARP)
// Timewarp style mudlib.

#include <socket.h>
#include <errno.h>
#include <erq.h>
#include <strings.h>

#elif defined(DUNE)
// Dune style mudlib.

#include <socketd.h>
#include <errno.h>
#include <erq.h>
#include <sys/strings.h>

#else
// Unknown mudlib.

#error "Unable to locate resource header files.\n"

#endif


/// socket manipulation definitions.

#if defined(TIMEWARP) && defined(SOCKETD)
// Timewarp style SOCKETD.

#define DO_SOCKET_CONNECT(A, P, M)  (socket_handle = (mixed)(SOCKETD->socket_connect((A), (P), \
  (: switch($2) { \
       case SOCKET_READY:  cb_socket_connect();  break; \
       case SOCKET_ERROR: \
         if ($3 == ERQ_E_NOTFOUND) \
           cb_socket_error("host not found."); \
         else \
           switch($4) { \
             case ECONNREFUSED:  cb_socket_error("connection refused.");  break; \
             case EHOSTUNREACH:  cb_socket_error("host unreachable.");    break; \
             case ENETUNREACH :  cb_socket_error("network unrechable.");  break; \
             default          :  cb_socket_error(sprintf("socket error %#Q" , $4)); \
           } \
       break; \
       case SOCKET_CLOSE:  cb_socket_close();    break; \
       case SOCKET_READ :  cb_socket_read($3);   break; \
       default          :  cb_socket_error(sprintf("unknown socket event %#Q.\n", $2)); \
     } \
  :), ((M) == SMODE_BINARY) ? SOCKET_BINARY : 0)))

#define DO_SOCKET_CLOSE             (SOCKETD->socket_close(socket_handle))

#define DO_SOCKET_WRITE(M)          (SOCKETD->socket_write(socket_handle, (M)))

#elif defined(DUNE) && defined(SOCKETD)
// Dune style SOCKETD.

#define DO_SOCKET_CONNECT(A, P, M)  (socket_handle = (object)(SOCKETD->New(TCP_OPEN, (A), (P), \
  (: switch($2) { \
       case SOCKET_READY  :  cb_socket_connect();  break; \
       case SOCKET_ERROR  :  cb_socket_error($3);  break; \
       case SOCKET_CLOSE  :  cb_socket_close();    break; \
       case SOCKET_READ   :  cb_socket_read($3);   break; \
       case SOCKET_WRITTEN:  /* do we care? */     break; \
       default            :  cb_socket_error(sprintf("unknown socket event %#Q.\n", $2)); \
     } \
  :), ((M) == SMODE_BINARY) ? SOCKET_BINARY : SOCKET_ASCII)))

#define DO_SOCKET_CLOSE             (socket_handle ? \
                                      socket_handle->Close() : \
                                      cb_socket_error("unable to close socket (closed).\n"))

#define DO_SOCKET_WRITE(M)          (socket_handle ? \
                                      socket_handle->Write((M)) : \
                                      cb_socket_error("unable to write to socket (closed).\n"))

#elif defined(SOCKETD)
// Unknown socket daemon.

#error "tcp_socket: Unknown SOCKETD daemon.\n"

#else
// No socket daemon.

#error "tcp_socket: No SOCKETD daemon; net_connect() not yet supported.\n"

#endif


/// tcp_socket definitions.

#define SMODE_ASCII                 0
#define SMODE_BINARY                1

#define CONNECT_WAIT                15


/// tcp_socket state variables.

nosave    int    socket_connected = 0;   // socket is connected.
nosave    int    socket_mode      = 0;   // ascii, binary (, mud mode?)

nosave    string host_address     = 0;   // socket host address.
nosave    int    host_port        = 0;   // socket host port.

nosave    mixed  socket_handle    = 0;   // socket handle.


/// tcp_socket prototypes.

protected void   socket_clear       ();
protected string socket_connect     (string ip_address, int port, int mode);
protected void   socket_timeout     ();
protected string socket_close       ();
protected string socket_write       (mixed message);

private   void   cb_socket_connect  ();
private   void   cb_socket_close    ();
private   void   cb_socket_error    (string error);
private   void   cb_socket_read     (string message);


/// mudlib integration methods.

int deny_update() {
  return (host_address != 0);
}

int clean_up(int arg) {
  if (deny_update())
    return 1;
  destruct(this_object());
  return 0;
}

void destructor() {
  if (deny_update()) {
    socket_close();
    //cb_socket_error("socket object destructed.");
  }
}


/// tcp_socket stub methods.

// called when the socket is initially connected.
protected void do_socket_connect() { }

// called when the socket receives a message.
protected void do_socket_read(mixed message) { }

// called when a message is written to the socket.
protected void do_socket_write(mixed message) { }

// called when the socket closes.
protected void do_socket_close() { }

// called when the socket fails to connect.
protected void do_socket_error(string error) { /*socket_connected &&*/ socket_close(); }


/// tcp_socket query methods.

public int    query_socket_connected()  { return socket_connected; }

public string query_host_address()      { return host_address; }
public int    query_host_port()         { return host_port; }

public int    query_socket_mode()       { return socket_mode; }

public mixed  query_socket_handle()     { return socket_handle; }


/// tcp_socket process methods.

protected void socket_clear() {
  if (host_address)
    return;

  host_address  = 0;
  host_port     = 0;
  socket_mode   = 0;
  socket_handle = 0;
}

protected string socket_connect(string ip_address, int port, int mode) {
  if (host_address)
    return (socket_connected ? "socket busy." : "socket connecting.");

  string error = catch(DO_SOCKET_CONNECT(ip_address, port, mode); publish);
  if (error)
    return "error connecting: " +trim(error, TRIM_BOTH, "* \n");

  host_address = ip_address;
  host_port    = port;
  socket_mode  = mode;

  call_out(#'socket_timeout, CONNECT_WAIT);
  return 0;
}

protected void socket_timeout() {
  cb_socket_error("timed out.");
}

protected string socket_close() {
  if (!socket_connected)
    return (host_address ? "socket closing." : "socket not connected.");

  DO_SOCKET_CLOSE;
  socket_connected = 0;
  return 0;
}

protected string socket_write(mixed message) {
//object bubbs = find_player("bubbs");
//bubbs && tell_object(bubbs, sprintf("tcp_socket::socket_write(%#Q);\n", message));

//bubbs && tell_object(bubbs, sprintf("socket_connected=%#Q\n", socket_connected));
  if (!socket_connected)
    return (host_address ? "socket closing." : "socket not connected.");
//bubbs && tell_object(bubbs, sprintf("strlen(message)=%#Q\n", strlen(message)));

//if (strlen(message)) {
//mixed sw =
//  DO_SOCKET_WRITE(message);
//bubbs && tell_object(bubbs, sprintf("DO_SOCKET_WRITE()=%#Q\n", sw));
//}
  if (strlen(message)) {
    DO_SOCKET_WRITE(message);
    do_socket_write(message);
  }
  return 0;
}


/// tcp_socket callback methods.

private void cb_socket_connect() {
  socket_connected = 1;
  remove_call_out(#'socket_timeout);

  do_socket_connect();
}

private void cb_socket_close() {
  socket_connected = 0;

  do_socket_close();

  host_address  = 0;
  host_port     = 0;
}

private void cb_socket_error(string error) {
  host_address  = 0;
  host_port     = 0;
  socket_mode   = 0;
  socket_handle = 0;

  do_socket_error(error);
}

private void cb_socket_read(string message) {
//  if (socket_connected && host_address && strlen(message))
  if (socket_connected && host_address)
    do_socket_read(message);
}


