//// httpclient.h
// v0.5.2.
//
// LDMud daemon and object providing http client services.
// See httpclient.txt for history and usage notes.
//
// By Bubbs (bubbs@eclipse.co.uk).


#if !defined(HTTPCLIENT_H)

#define HTTPCLIENT_H


/// mudlib specific definitions.

#if defined(TIMEWARP)
// Timewarp style mudlib.

// Path to httpclient installation directory.
#define HTTPCLIENT_DIR         "/secure/sockets/http/"

#if defined(MUD_NAME) && (MUD_NAME == "BrightSword")
#define HTTPCLIENT_LOG(L)      (log_file("@HTTPCLIENT", (L)))
#elif defined(TIMEWARP)
#define HTTPCLIENT_LOG(L)      (log_file("$HTTPCLIENT", (L)))
#endif

#if MUD_NAME == "BrightSword"
#define ENABLE_FILE_URLS
#else
#define NO_ENABLE_FILE_URLS
#endif


#elif defined(DUNE)
// Dune style mudlib.

// Path to httpclient installation directory.
#define HTTPCLIENT_DIR         "/secure/socketd/httpclient/"

#define HTTPCLIENT_LOG(L)      (log_file("@HTTPCLIENT", (L)))
//#define ADVANCED_LOG(T, H, A, C, M) \
//  (write_file("/httpclient_log.csv", (sprintf("%d,%O,%O,%s,%s\n", (T), (H), (A), (C), (M)))
//  (log_file("@httpclient.csv", (sprintf("%d,%O,%O,%s,%s\n", (T), (H), (A), (C), (M)))
//  (SQL_D->sprintf("INSERT INTO httpclient_log ( log_time, client_obj, app_obj, context, message ) VALUES ( %d, '%s', '%s', '%s, '%s' );", (T), db_conv_string((H)), db_conv_string((A)), db_conv_string((C)), db_conv_string((M))))

#define NO_ENABLE_FILE_URLS


#else
// Unknown mudlib.

#error "Unable to locate httpclient installation directory.\n"

// Path to httpclient installation directory.
//#define HTTPCLIENT_DIR         "/secure/socketd/httpclient/"

// Object name of client daemon/object.
//#define HTTPCLIENT             HTTPCLIENT_DIR "httpclient"

// Path to 'about:httpclient' text file.
//#define ABOUT_HTTP_FILE        HTTPCLIENT_DIR "httpclient.txt"

// Define HTTPCLIENT_LOG(L) to enable basic logging.
// L: log_message_string
//#define HTTPCLIENT_LOG(L)      (log_file("HTTPCLIENT", (L)))
//#define HTTPCLIENT_LOG(L)      (log_file("secure/HTTPCLIENT", (L)))
//#define HTTPCLIENT_LOG(L)      (write_file("/secure/log/HTTPCLIENT", (L)))

// Define ADVANCED_LOG(T, H, A, C, M) to enable advanced logging.
// T: time_int
// H: object_name(httpclient_object)
// A: object_name(application_object)
// C: context_string
// M: message_string
//#define ADVANCED_LOG(T, H, A, C, M) \
//  (write_file("/httpclient_log.csv", (sprintf("%d,%O,%O,%s,%s\n", (T), (H), (A), (C), (M)))
//  (log_file("@httpclient.csv", (sprintf("%d,%O,%O,%s,%s\n", (T), (H), (A), (C), (M)))
//  (SQL_D->sprintf("INSERT INTO httpclient_log ( log_time, client_obj, app_obj, context, message ) VALUES ( %d, '%s', '%s', '%s, '%s' );", (T), db_conv_string((H)), db_conv_string((A)), db_conv_string((C)), db_conv_string((M))))

// Used to determine user name, for logging purposes.
//#define HTTP_USER_NAME(U)      (to_string((U)->query_real_name() || object_name((U))))

// Define one of these to log (or not log) 'http://' requests (default = log).
//#define LOG_HTTP_REQUESTS
//#define NO_LOG_HTTP_REQUESTS

// Define this to enable 'file:' urls.
//#define ENABLE_FILE_URLS
#define NO_ENABLE_FILE_URLS

// When file urls are enabled, master()->valid_read().
// The 'func' used when
//#define MASTER_VALID_READ_FUNC "read_file"

// Define one of these to log (or not log) 'file://' requests (default = log).
//#define LOG_FILE_REQUESTS
//#define NO_LOG_FILE_REQUESTS

#endif


/// mudlib harmonisation definitions.

#if !defined(HTTPCLIENT)
#define HTTPCLIENT             HTTPCLIENT_DIR "httpclient"
#endif

#if !defined(ABOUT_HTTP_FILE)
#define ABOUT_HTTP_FILE        HTTPCLIENT_DIR "httpclient.txt"
#endif

#if !defined(HTTPCLIENT_LOG)
#define HTTPCLIENT_LOG(L)      (log_file("HTTPCLIENT", (L)))
#endif

//#if !defined(ADVANCED_LOG)
//#define ADVANCED_LOG(T, H, A, C, M) \
//  (write_file("/httpclient_log.csv", (sprintf("%d,%O,%O,%s,%s\n", (T), (H), (A), (C), (M)))
//  (log_file("@httpclient.csv", (sprintf("%d,%O,%O,%s,%s\n", (T), (H), (A), (C), (M)))
//  (SQL_D->sprintf("INSERT INTO httpclient_log ( log_time, client_obj, app_obj, context, message ) VALUES ( %d, '%s', '%s', '%s, '%s' );", (T), db_conv_string((H)), db_conv_string((A)), db_conv_string((C)), db_conv_string((M))))
//#endif

#if !defined(HTTP_USER_NAME)
#define HTTP_USER_NAME(U)      (to_string((U)->query_real_name() || object_name((U))))
#endif

#if !defined(LOG_HTTP_REQUESTS) && !defined(NO_LOG_HTTP_REQUESTS)
#define LOG_HTTP_REQUESTS
#endif

#if !defined(ENABLE_FILE_URLS) && !defined(NO_ENABLE_FILE_URLS)
#define ENABLE_FILE_URLS
#endif

#if !defined(MASTER_VALID_READ_FUNC)
#define MASTER_VALID_READ_FUNC "read_file"
#endif

#if !defined(LOG_FILE_REQUESTS) && !defined(NO_LOG_FILE_REQUESTS)
#define LOG_FILE_REQUESTS
#endif


/// httpclient static definitions.

// Client version.
#define CLIENT_VERSION         "0.5.2"

// Supported HTTP version.
#define HTTP_VERSION           "1.1"

// If defined, provide a default protocol.
//#define DEFAULT_PROTOCOL       "file"

// Used by parse_url() to identify URL components.
//#define HTTP_URL_REGEXP        "^([^:]*)://([^:/?]*)(:([0-9]*)|)(/[^?]*|)(?(.*)|)$"
#define HTTP_URL_REGEXP        "^([^:]*):+//([^:/?#]*)(:([0-9]*)|)(/[^?#]*|)(?([^#]*)|)(#.*|)$"

#define FILE_URL_REGEXP        "^([^:]*):+(/[^?#]*)(?([^#]*)|)(#.*|)$"

// Default HTTP method.
#define DEFAULT_HTTP_METHOD    "GET"

// Default HTTP port.
#define DEFAULT_HTTP_PORT      80

// Default accepted media types.
#define DEFAULT_MEDIA_TYPES    "text/*"
//#define DEFAULT_MEDIA_TYPES    "text/html text/plain"

#if !defined(DEFAULT_USER_AGENT)
// Default user agent.
// This one seems to work pretty well.
#define DEFAULT_USER_AGENT     "Lynx/2.8.5rel.5 libwww-FM/2.14"
//#define DEFAULT_USER_AGENT     "Lynx/2.8.5rel.5 libwww-FM/2.14 SSL-MM/1.4.1 OpenSSL/0.9.8b"
//#define DEFAULT_USER_AGENT     "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.1)"
//#define DEFAULT_USER_AGENT     "Mozilla/4.0 (lpc-httpclient/" CLIENT_VERSION "; " MUDLIB_VERSION "; en-UK)"
//#define DEFAULT_USER_AGENT     "Mozilla/4.0"
//#define DEFAULT_USER_AGENT     "Wget/1.10.2"
//#define DEFAULT_USER_AGENT     "none"
#endif

#if !defined(COOKIE_DATE_FORMATS)
// Cookie date formats.

#define CTIME_DATE_FORMAT        "WEEKDAY +MONTH +DATE +HOUR:MINUTE:SECOND +YEAR"
#define ATOM_DATE_FORMAT         "YEAR-MONTH-DATEtHOUR:MINUTE:SECOND"
#define ISO8601_DATE_FORMAT      "YEAR-MONTH-DATE[ tT]HOUR:MINUTE:SECOND *ZONE"

#define COOKIE_DATE_FORMATS      \
  ({ \
    "WEEKDAY[, ]+DATE[-/ ]+MONTH[-/ ]+YEAR +HOUR:MINUTE:SECOND +ZONE", \
    "WEEKDAY[, ]+MONTH +DATE +HOUR:MINUTE:SECOND +YEAR", \
    "WEEKDAY[, ]+DATE[-/ ]+MONTH[-/ ]+YEAR +HOUR:MINUTE:SECOND",\
    CTIME_DATE_FORMAT,   \
    ATOM_DATE_FORMAT ,   \
    ISO8601_DATE_FORMAT, \
  })
//    "(|HOUR:MINUTE(|:SECOND)) +(|WEEKDAY +)(MONTH[-/]+DATE|DATE[-/ ]+MONTH)[-/ ]+YEAR", \
//    "(|WEEKDAY +)MONTH +DATE +HOUR:MINUTE(|:SECOND) +YEAR(| +ZONE)", \

#endif

// Content type for www-form-encode encoding.
#define WWW_FORM_CONTENT_TYPE  "application/x-www-form-urlencoded"

// Content type for plain text.
#define TEXT_CONTENT_TYPE      "text/plain"

// Base64 Alphabet; used for base-64 encoding (Basic WWW-Authenticate support).
#define BASE64_ALPHABET        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

// Callback events.
#define HTTP_DEBUG             -5   // data: mixed value.             return: void.
#define HTTP_BUSY              -1   // data: void.                    return: void.
#define HTTP_FAILED            00   // data: string message.          return: void.
#define HTTP_SUCCESS           10   // data: string content.          return: void.
#define HTTP_REDIRECT          20   // data: string redirect_url.     return: (follow == 1).
#define HTTP_GOT_STATUS        30   // data: ({ ver, code, mnem, }).  return: void.
#define HTTP_GOT_HEADER        40   // data: ({ header, value, }).    return: void.
#define HTTP_GET_COOKIES       50   // data: void.                    return: cookies.
#define HTTP_SEND_COOKIE       60   // data: mixed* cookie.           return: (send_cookie == 1).
#define HTTP_KEEP_COOKIE       70   // data: mixed* cookie.           return: (keep_cookie == 1).
#define HTTP_SET_COOKIES       80   // data: mixed* cookies.          return: void.
#define HTTP_AUTHENTICATE      90   // data: ({ host, realm, }).      return: ([ user : password ]).

// Split URL data indices.
#define URD_PROTOCOL           0    // string protocol;  "http"
#define URD_HOST               1    // string host;      e.g. "www.google.co.uk"
#define URD_PORT               2    // int    port;      e.g. 80
#define URD_PATH               3    // string path;      e.g. "/search.html"
#define URD_ARGS               4    // string args;      e.g. "q=ldmud"

#define URL_DATA_SIZE          (URD_ARGS +1)

// Cookie array indices, and size.
#define CK_NAME                0    // string name;      cookie name.
#define CK_VALUE               1    // string value;     cookie value.
#define CK_EXPIRES             2    // int expiry;       expiration time (or -1 for session cookie).
#define CK_SECURE              3    // int secure;       'https:' is not supported, so always 0.
#define CK_DOMAIN              4    // string domain;    cookie domain.
#define CK_PATH                5    // string path;      cookie path.

#define COOKIE_SIZE            (CK_PATH +1)

#define VALID_COOKIE(C)        \
   (pointerp((C))           && (sizeof((C)) == COOKIE_SIZE) && \
    stringp((C)[CK_NAME])   && stringp((C)[CK_VALUE]) && \
    intp((C)[CK_EXPIRES])   && intp((C)[CK_SECURE]) && \
    stringp((C)[CK_DOMAIN]) && stringp((C)[CK_PATH]) \
   )

// source_types index and width definitions.
#define SRCTYP_DESCRIPTION                0     // string   description of source type.
#define SRCTYP_FILE_EXTENSIONS            1     // string*  matching file extensions.
#define SRCTYP_MATCH_REGEXP               2     // string   optional matching regular expression.

#define SOURCE_TYPES_WIDTH                (SRCTYP_MATCH_REGEXP +1)

// Default source type.
#define DEFAULT_SOURCE_TYPE               TEXT_CONTENT_TYPE

// Default source types file.
#if !defined(DEFAULT_SOURCE_TYPES_FILE)
#define DEFAULT_SOURCE_TYPES_FILE         HTTPCLIENT_DIR "source_types.ini"
#endif


#endif    // #if !defined(HTTPCLIENT_H)


