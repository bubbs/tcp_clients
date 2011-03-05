// httpclient test.
//
// basic functionality test of httpclient.


#include <httpclient.h>


//#define DEFAULT_URL "about:httpclient"
#define DEFAULT_URL "http://www.google.com/index.html"


object user    = 0;
string url     = 0;
string content = 0;
string failed  = 0;


public mixed http_callback(object client, int event, mixed data);


public int main(string u) {
  if (!u || user)
    return 0;

  user    = this_player();
  url     = u || DEFAULT_URL;
  content = 0;
  failed  = 0;

  object client = clone_object(HTTPCLIENT);
  client->set_media_types("text/*");
  //client->set_user_agent("Lynx/2.8.5rel.5 libwww-FM/2.14");
  //client->set_authorization("username", "password");
  //mixed *cookie = allocate(COOKIE_SIZE));
  //cookie[CK_NAME]    = "PREF";
  //cookie[CK_VALUE]   = ""
  //cookie[CK_EXPIRES] = time() +3600;
  //cookie[CK_SECURE]  = 0;
  //cookie[CK_DOMAIN]  = ".google.com";
  //cookie[CK_PATH]]   = "/";
  //client->set_cookies(({ cookie, }));

  int requested = client->request_get(url, #'http_callback);
  printf(requested ? "[%#Q] Requesting %s.\n" : "[%#Q] Request failed %s.\n", client, url);

  return requested;
}


public mixed http_callback(object client, int event, mixed data) {
  mixed  return_value = 0;
  string message      = 0;
  int    dest_client  = 0;

  switch(event) {
    case HTTP_BUSY:
      failed       = "client busy.";
      message      = failed;
      dest_client  = 1;
    case HTTP_FAILED:
      failed       = data;
      message      = failed;
      dest_client  = 1;
    break;
    case HTTP_SUCCESS:
      message      = "URL received: " +url +".";
      content      = data;
      dest_client  = 1;
    break;
    case HTTP_REDIRECT:
      message      = "URL redirected: " +data +".";
#if 1
      dest_client  = 1;
#else
      url          = data;
      return_value = 1;
#endif
    break;
  }

  if (user && message)
    tell_object(user, sprintf("[%#Q] %s\n", client, message));

  if (dest_client) {
    destruct(client);
    user = 0;
  }

  return return_value;
}


public object query_user()    { return user; }
public string query_url()     { return url; }
public string query_content() { return content; }
public string query_failed()  { return failed; }


public int clean_up(int arg) { destruct(this_object()); }

