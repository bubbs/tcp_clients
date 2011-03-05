//// tcp_gopher.h
//
// tcp_gopher definitions.

#if !defined(TCP_GOPHER_H)

#define TCP_GOPHER_H


#if defined(TIMEWARP)
// Timewarp style mudlib.

#include <strings.h>

#define TCP_GOPHER_D                    "/secure/sockets/http/tcp_gopher"
//#define TEST_GOPHER_URL                 "gopher://gopher.floodgap.com/0/v2/vstat"
#define TEST_GOPHER_URL                 "gopher://gopher.floodgap.com/fun/phoongwfun/phoongw"

#elif defined(DUNE)
// Dune style mudlib.

#include <sys/strings.h>

#define TCP_GOPHER_D                    "/secure/socketd/httpclient/tcp_gopher"

#else
// Unknown mudlib.

#error "tcp_gopher(): Unknown mudlib - configuration required.\n"

#include <sys/strings.h>

// define to override default location of tcp_gopher request object.
//#define TCP_GOPHER_D                    "/secure/sockets/http/tcp_gopher"

// define to provide (or suppress) simple gopher() test method, with default gopher URL.
////#define TEST_GOPHER_URL                 "gopher://gopher.floodgap.com/"
//#define TEST_GOPHER_URL                 "gopher://gopher.floodgap.com/fun/phoongwfun/phoongw"
//#define NO_TEST_GOPHER_URL

#endif


// location of tcp_gopher request object.
#if !defined(TCP_GOPHER_D)
#define TCP_GOPHER_D                    "/secure/sockets/tcp_gopher"
#endif

// define to provide (or suppress) simple gopher() test method, with default gopher URL.
#if !defined(TEST_GOPHER_URL) && !defined(NO_TEST_GOPHER_URL)
////#define TEST_GOPHER_URL                 "gopher://gopher.floodgap.com/"
//#define TEST_GOPHER_URL                 "gopher://gopher.floodgap.com/fun/phoongwfun/phoongw"
#define NO_TEST_GOPHER_URL
#endif


// default gopher port.
#define DEFAULT_GOPHER_PORT             70

// gopher request data index and size definitions.
#define GREQ_PROTOCOL                   0    // string - "gopher"
#define GREQ_HOST                       1    // string - request host
#define GREQ_PORT                       2    // int    - request port
#define GREQ_SELECTOR                   3    // string - request selector

#define GOPHER_REQUEST_DATA_SIZE        (GREQ_SELECTOR +1)

// gopher item regular expression.
#define GOPHER_ITEM_REGEXP              "(.)([^\t]*)\t([^t]*)\t([^\t]+)\t([0-9]*)"

// gopher item data index and size definitions.
#define GITEM_ITEM_TYPE                 0    // int;     The item type (see below).
#define GITEM_DISPLAY                   1    // string;  The item display name.
#define GITEM_SELECTOR                  2    // string;  The item selector.
#define GITEM_HOST_NAME                 3    // string;  The host of item.
#define GITEM_PORT                      4    // string;  The port of item.

#define GOPHER_ITEM_SIZE                (GITEM_PORT +1)

// gopher item types.
#define GITTY_PLAIN_TEXT                '0'  // plain text file.
#define GITTY_DIRECTORY_MENU            '1'  // directory menu listing.
#define GITTY_CSO_SEARCH_QUERY          '2'  // CSO search query.
#define GITTY_ERROR_MESSAGE             '3'  // error message.
#define GITTY_BINHEX_ENCODED            '4'  // BinHex encoded text file.
#define GITTY_BINARY_ARCHIVE            '5'  // binary archive file.
#define GITTY_UUNENCODED                '6'  // UUEncoded text file.
#define GITTY_SEARCH_ENGINE_QUERY       '7'  // search engine query.
#define GITTY_TELNET_SESSION            '8'  // telnet session pointer.
#define GITTY_BINARY_FILE               '9'  // binary file.
#define GITTY_GIF_IMAGE                 'g'  // GIF image.
#define GITTY_HTML_FILE                 'h'  // HTML file.
#define GITTY_INFORMATIONAL             'i'  // informational message.
#define GITTY_OTHER_IMAGE               'I'  // Image file on unspecified format; client decides how to display; often used for JPEG images.
#define GITTY_AUTO_FORMAT               's'  // Audio file format, primarily a WAV file.
#define GITTY_TN3270_SESSION            'T'  // tn3270 session pointer.

// gopher event codes.
#define GOPHER_DEBUG                    -5   // data: string debug_message;  return: void;  A gopher debug message.
#define GOPHER_BUSY                     -1   // data: void;                  return: void;  The gopher client object is busy.
#define GOPHER_FAILED                   00   // data: string error_message;  return: void;  The gopher request failed.
#define GOPHER_SUCCESS                  10   // data: string response_text;  return: void;  The gopher request succeeded.


#endif
