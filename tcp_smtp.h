//// tcp_smtp
//
// SMTP (Simple Mail Transfer Protocol) implementation.
//
// SMTP Definitions.


#if !defined(TCP_SMTP_H)

#define TCP_SMTP_H


/// mudlib specific definitions.

#if defined(TIMEWARP)
// Timewarp style mudlib.

#include <strings.h>

#define TCP_SMTP_D                      "/secure/sockets/http/tcp_smtp"
//#define SMTP_MAIL_SAVE_FILE             "/data/mail/tcp_smtp"
#define NO_SMTP_MAIL_SAVE_FILE
#define VALID_EMAIL_CHECK(U, R)         (adminp((U)) || (lower_case((string)((U)->query_finger_info("email")) || "") == lower_case((R))))
#define SMTP_SERVER_HOST                "smtpout.karoo.kcom.com"
#define SMTP_SENDER_ADDRESS             "bubbs@eclipse.co.uk"
#define TRACK_UNKNOWN_SMTP_CODES

#elif defined(DUNE)
// Dune style mudlib.

#include <sys/strings.h>

#define TCP_SMTP_D                      "/secure/socketd/httpclient/tcp_smtp"
//#define SMTP_MAIL_SAVE_FILE             "/data/daemons/tcp_smtp"
////#define SMTP_MAIL_SAVE_FILE             "/data/mail/tcp_smtp"
#define NO_SMTP_MAIL_SAVE_FILE
#define VALID_EMAIL_CHECK(U, R)         (adminp((U)) || (lower_case((string)((U)->query_mailaddr()) || "") == lower_case((R))))
#define SMTP_SERVER_HOST                "dune.servint.com"
#define TRACK_UNKNOWN_SMTP_CODES
#define SMTP_REPLY_TO_ADDRESS           ("no-reply@" +SMTP_SERVER_HOST)
#define SMTP_RETURN_PATH_ADDRESS        ("no-reply@" +SMTP_SERVER_HOST)

#else
// Unknown mudlib.

#error "Unknown mudlib.\n"

#include <sys/strings.h>

// Define to override the default tcp_smtp daemon filename.
//#define TCP_SMTP_D                      "/secure/socket/tcp_smtp"

// Define to override the default tcp_smtp external name.
//#define MAILER_NAME                     "ldmud/tcp_smtp"

// Define to override the default tcp_smtp version.
//#define MAILER_VERSION                  "v0.7"

// Define to support (or prevent) persistance of messages in transit (leave off the '.o').
//#define SMTP_MAIL_SAVE_FILE             ("/" +__DIR__ +"tcp_smtp")
//#define NO_SMTP_MAIL_SAVE_FILE

// Define (or not) to perform access (object U, string R recipient).
//#define VALID_EMAIL_CHECK(U, R)         (master->query_player_level("error messages") && 1)
//#define NO_VALID_EMAIL_CHECK

// Define to override the default SMTP server host ("localhost").
//#define SMTP_SERVER_HOST                "localhost"

// Define to override the default SMTP server port (25)
//#define SMTP_SERVER_PORT                25

// Define to slow (or not) SMTP transmission by a call-out.
//#define SLOW_SOCKET_WRITE               0
//#define NO_SLOW_SOCKET_WRITE

// If defined, track unknown SMTP codes (or not).
//#define TRACK_UNKNOWN_SMTP_CODES
//#define NO_TRACK_UNKNOWN_SMTP_CODES

// Define to override the default sender email address ("mud@" +SMTP_SERVER_HOST)
//#define SMTP_SENDER_ADDRESS             ("mud@" +SMTP_SERVER_HOST)

// Define to override the default sender email display name ("Name%Mudname Mud")
// Recognises tokens '$N' (sender name), '$M' (mud name), '$$' '$' character.
//
//#define SMTP_SENDER_DISPLAY_NAME        "$N@$M Mud"

// Get a users name (object U user).
//#define GET_USER_NAME(U)                ((string)((U)->query_real_name()))

// Define to override the sender name (unless GET_USER_NAME(this_player()) is used), (object O).
//#define SMTP_SENDER_NAME_FUNC           (: capitalize((string)($1->query_mail_name()) || object_name($1)) :)
//#define SMTP_SENDER_NAME(O)             (funcall(SMTP_SENDER_NAME_FUNC, (O)))
////#define SMTP_SENDER_NAME(O)           (capitalize((string)((O)->query_mail_name()) || object_name((O))))

// Define to override the final default sender name o
//#define UNKNOWN_SENDER_NAME             "(unknown)"

// Define to override the default mud name
//#if !defined(MUD_NAME)
//#define MUD_NAME                        "LDMud"
//#endif

// Define to provide (or prevent) a "Reply-To" message header.
//#define SMTP_REPLY_TO_ADDRESS           SMTP_SENDER_ADDRESS
//#define NO_SMTP_REPLY_TO_ADDRESS

// Define to provide (or prevent) a "Return-Path" message header.
//#define SMTP_RETURN_PATH_ADDRESS        SMTP_SENDER_ADDRESS
//#define NO_SMTP_RETURN_PATH_ADDRESS

// Define to provide (or prevent) a "X-Mailer" message header.
//#define SMTP_X_MAILER_IDENTITY          (MAILER_NAME +"/" +MAILER_VERSION)
//#define NO_SMTP_X_MAILER_IDENTITY

// Define to provide (or prevent)  test harness method 'public mail_file(string address, string file, int flags)'
//#define TEST_SMTP_METHODS
//#define NO_TEST_SMTP_METHODS


#endif


/// mudlib equalisation layer.

#if !defined(TCP_SMTP_D)
#define TCP_SMTP_D                      "/secure/socket/tcp_smtp"
#endif

#if !defined(MAILER_NAME)
#define MAILER_NAME                     "ldmud/tcp_smtp"
#endif

#if !defined(MAILER_VERSION)
#define MAILER_VERSION                  "v0.7"
#endif

#if !defined(SMTP_MAIL_SAVE_FILE) && !defined(NO_SMTP_MAIL_SAVE_FILE)
#define SMTP_MAIL_SAVE_FILE             ("/" +__DIR__ +"tcp_smtp")
#endif

#if !defined(VALID_EMAIL_CHECK) && !defined(NO_VALID_EMAIL_CHECK)
#define VALID_EMAIL_CHECK(U, R)         (master->query_player_level("error messages") && 1)
//#define NO_VALID_EMAIL_CHECK
#endif

#if !defined(SMTP_SERVER_HOST)
#define SMTP_SERVER_HOST                "localhost"
#endif

#if !defined(SMTP_SERVER_PORT)
#define SMTP_SERVER_PORT                25
#endif

#if !defined(SLOW_SOCKET_WRITE) && !defined(NO_SLOW_SOCKET_WRITE)
//#define SLOW_SOCKET_WRITE               0
#define NO_SLOW_SOCKET_WRITE
#endif

#if !defined(TRACK_UNKNOWN_SMTP_CODES) && !defined(TRACK_UNKNOWN_SMTP_CODES)
//#define TRACK_UNKNOWN_SMTP_CODES
#define NO_TRACK_UNKNOWN_SMTP_CODES
#endif

#if !defined(SMTP_SENDER_ADDRESS)
#define SMTP_SENDER_ADDRESS             ("mud@" +SMTP_SERVER_HOST)
#endif

#if !defined(SMTP_SENDER_DISPLAY_NAME)
#define SMTP_SENDER_DISPLAY_NAME        "$N@$M Mud"
#endif

#if !defined(GET_USER_NAME)
#define GET_USER_NAME(U)                ((string)((U)->query_real_name()))
#endif

#if !defined(SMTP_SENDER_NAME)
//#define SMTP_SENDER_NAME(O)             (capitalize((string)((O)->query_mail_name()) || object_name((O))))
#define SMTP_SENDER_NAME_FUNC           (: capitalize((string)($1->query_mail_name()) || object_name($1)) :)
#define SMTP_SENDER_NAME(O)             (funcall(SMTP_SENDER_NAME_FUNC, (O)))
#endif

#if !defined(UNKNOWN_SENDER_NAME)
#define UNKNOWN_SENDER_NAME             "(unknown)"
#endif

#if !defined(MUD_NAME)
#define MUD_NAME                        "LDMud"
#endif

#if !defined(SMTP_REPLY_TO_ADDRESS) && !defined(NO_SMTP_REPLY_TO_ADDRESS)
//#define SMTP_REPLY_TO_ADDRESS           SMTP_SENDER_ADDRESS
#define NO_SMTP_REPLY_TO_ADDRESS
#endif

#if !defined(SMTP_RETURN_PATH_ADDRESS) && !defined(NO_SMTP_RETURN_PATH_ADDRESS)
//#define SMTP_RETURN_PATH_ADDRESS        SMTP_SENDER_ADDRESS
#define NO_SMTP_RETURN_PATH_ADDRESS
#endif

#if !defined(SMTP_X_MAILER_IDENTITY) && !defined(NO_SMTP_X_MAILER_IDENTITY)
#define SMTP_X_MAILER_IDENTITY          (MAILER_IDENTITY +"/" +MAILER_VERSION)
//#define NO_SMTP_X_MAILER_IDENTITY
#endif

#if !defined(TEST_SMTP_METHODS) && !defined(NO_TEST_SMTP_METHODS)
//#define TEST_SMTP_METHODS
#define NO_TEST_SMTP_METHODS
#endif


/// tcp_smtp definitions.

// message index and size definitions.
#define MESG_CALLBACK                   0       // closure;  Application API.
#define MESG_SENDER_DISPLAY_NAME        1       // string;   Sender display name.
#define MESG_RECIPIENT_ADDRESSES        2       // string*;  Destination email addresses.
#define MESG_COPY_ADDRESSES             3       // string*;  Copy-to email addresses.
#define MESG_BLIND_COPY_ADDRESSES       4       // string*;  Blind-copy-to email addresses.
#define MESG_MESSAGE_HEADERS            5       // string*;  Composed message headers.
#define MESG_MESSAGE_BODY               6       // string;   Message text.
#define MESG_EXTRA_ARGUMENTS            7       // mixed*;   Additional arguments to send_mail().
#define MESG_SEND_ATTEMPT_COUNT         8       // int;      Number of send attempts.
#define MESG_RCPT_INDEX                 9       // int;      Index of recipient last sent.

#define MESSAGE_WIDTH                   (MESG_RCPT_INDEX +1)

// SMTP Event Callback codes.

#define SMTP_DEBUG                      -5      // data: string debug_message;  return: void;  An SMTP debug message.
#define SMTP_FAILED                     00      // data: string error_message;  return: void;  The SMTP request failed.
//#define SMTP_PAUSED                     10      // data: int request_paused;    return: void;  Pause callbacks.  Not implemented.
#define SMTP_SENDING                    20      // data: void;                  return: void;  The mail is now being sent.
#define SMTP_SUCCESS                    30      // data: string sent_message;   return: void;  The mail was successfully sent.

// SMTP engine states.
#define SMTP_SERVER_ERROR               -1      // A server error occurred.
#define SMTP_NOT_CONNECTED              00      // Not connected.
#define SMTP_CONNECTING                 05      // Attempting to connect.
#define SMTP_CONNECTED                  10      // Connected.
#define SMTP_SAID_HELO                  20      // Said 'HELO'.
#define SMTP_SAID_MAIL                  30      // Said 'MAIL FROM'.
#define SMTP_SAID_RCPT                  40      // Said 'RCPT TO'.
#define SMTP_SAID_DATA                  50      // Said 'DATA'.
#define SMTP_SAID_COMPLETE              80      // Said '.'.
#define SMTP_SAID_RESET                 90      // Said 'RSET'.
#define SMTP_SAID_QUIT                  95      // Said 'QUIT'.

// Regular expression for a valid email address.
#if defined(__PCRE__)
//#define EMAIL_ADDRESS_REGEXP            "[a-zA-Z0-9._%-]+(\\+[a-zA-Z._-]|)@[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)+"
#else
#define EMAIL_ADDRESS_REGEXP            "[a-zA-Z0-9._%-]+(\\+[a-zA-Z._-]|)@[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)+"
#endif

#if defined(TEST_SMTP_METHODS) && !defined(NO_TEST_SMTP_METHODS)
/// tcp_smtp test harness flags.
#define MFOPT_DEBUG                     0x0001  // Show debug messages (verbose).
#define MFOPT_QUIET                     0x0002  // Don't show any messages at all.
#endif


#endif


