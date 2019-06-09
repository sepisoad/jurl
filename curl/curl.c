#include <janet.h>
#include <curl/curl.h>
#include "curl.h"

/****************
 * easy_init
 ****************/

static Janet easy_init(int32_t argc, Janet *argv) {
  (void) argv;
  janet_arity(argc, 0, 0);

  curl_wrapper_t* curlw = calloc(1, sizeof(curl_wrapper_t));
  if(NULL == curlw) {
    janet_panicf("failed to allocate curl wrapper object");
  }

  curlw->write_data.memory = malloc(1);
  curlw->write_data.size = 0;
  if(NULL == curlw->write_data.memory) {
    janet_panicf("failed to allocate curl wrapper write data object");
  }

  curlw->curl = curl_easy_init();
  if(curlw->curl == NULL) {
    janet_panicf("failed to init curl object");
  }

  curl_easy_setopt(curlw->curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curlw->curl, CURLOPT_WRITEDATA, (void *)&curlw->write_data);

  return janet_wrap_pointer(curlw);
}

/****************
 * easy_cleanup
 ****************/

static Janet easy_cleanup(int32_t argc, Janet *argv) {
  janet_arity(argc, 1, 1);

  curl_wrapper_t* curlw = (curl_wrapper_t*) janet_getpointer(argv, 0);  
  if(curlw == NULL) {
    janet_panicf("curl wrapper object is not valid");
  }  

  CURL *curl = curlw->curl;
  if(curl == NULL) {
    janet_panicf("curl object is not valid");
  }

  curl_easy_cleanup(curl);
  free(curlw->write_data.memory);
  free(curlw);

  return janet_wrap_nil();
}

/****************
 * easy_setopt
 ****************/

static Janet easy_setopt(int32_t argc, Janet *argv) {
  janet_arity(argc, 1, 2);

  curl_wrapper_t* curlw = (curl_wrapper_t*) janet_getpointer(argv, 0);  
  if(curlw == NULL) {
    janet_panicf("curl wrapper object is not valid");
  }

  CURL *curl = curlw->curl;
  if(curl == NULL) {
    janet_panicf("curl object is not valid");
  }

  //TODO:  
  if (argc == 2) {
    const JanetKV* options = janet_getstruct(argv, 1);

    /* == BEHAVIOR OPTIONS == */
    setopt(curl, CURLOPT_VERBOSE, options, "verbose", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HEADER, options, "header", JANET_BOOLEAN);
    setopt(curl, CURLOPT_NOPROGRESS, options, "no-progress", JANET_BOOLEAN);
    setopt(curl, CURLOPT_NOSIGNAL, options, "no-signal", JANET_BOOLEAN);
    setopt(curl, CURLOPT_WILDCARDMATCH, options, "wildcard-match", JANET_BOOLEAN);
    setopt(curl, CURLOPT_URL, options, "url", JANET_STRING);
    setopt(curl, CURLOPT_PORT, options, "port", JANET_NUMBER);
    setopt(curl, CURLOPT_PROXY, options, "proxy", JANET_STRING);
    setopt(curl, CURLOPT_USERPWD, options, "user-pass", JANET_STRING);
    setopt(curl, CURLOPT_PROXYUSERPWD, options, "proxy-user-pass", JANET_STRING);
    setopt(curl, CURLOPT_RANGE, options, "range", JANET_STRING);    
    setopt(curl, CURLOPT_TIMEOUT, options, "timeout", JANET_NUMBER);    
    setopt(curl, CURLOPT_INFILESIZE, options, "in-file-size", JANET_NUMBER);
    setopt(curl, CURLOPT_POSTFIELDS, options, "post-fields", JANET_STRING);
    setopt(curl, CURLOPT_REFERER, options, "referer", JANET_STRING);
    setopt(curl, CURLOPT_FTPPORT, options, "ftp-port", JANET_NUMBER);
    setopt(curl, CURLOPT_USERAGENT, options, "user-agent", JANET_STRING);
    setopt(curl, CURLOPT_LOW_SPEED_LIMIT, options, "low-speed-limit", JANET_NUMBER);
    setopt(curl, CURLOPT_LOW_SPEED_TIME, options, "low-speed-time", JANET_NUMBER);
    setopt(curl, CURLOPT_RESUME_FROM, options, "resume-from", JANET_NUMBER);
    setopt(curl, CURLOPT_COOKIE, options, "cookie", JANET_STRING);
    setopt(curl, CURLOPT_SSLCERT, options, "ssl-cert", JANET_STRING);
    setopt(curl, CURLOPT_KEYPASSWD, options, "key-pass", JANET_STRING);
    setopt(curl, CURLOPT_CRLF, options, "crlf", JANET_NUMBER);
    setopt(curl, CURLOPT_COOKIEFILE, options, "cookie-file", JANET_STRING);
    setopt(curl, CURLOPT_SSLVERSION, options, "ssl-version", JANET_NUMBER);
    setopt(curl, CURLOPT_TIMECONDITION, options, "time-condition", JANET_NUMBER);
    setopt(curl, CURLOPT_TIMEVALUE, options, "time-value", JANET_NUMBER);
    setopt(curl, CURLOPT_CUSTOMREQUEST, options, "custom-request", JANET_STRING);
    setopt(curl, CURLOPT_NOBODY, options, "no-body", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FAILONERROR, options, "fail-on-error", JANET_BOOLEAN);
    setopt(curl, CURLOPT_UPLOAD, options, "upload", JANET_BOOLEAN);
    setopt(curl, CURLOPT_DIRLISTONLY, options, "dir-list-only", JANET_BOOLEAN);
    setopt(curl, CURLOPT_APPEND, options, "append", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FOLLOWLOCATION, options, "follow-location", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TRANSFERTEXT, options, "transfer-text", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PUT, options, "put", JANET_BOOLEAN);
    setopt(curl, CURLOPT_AUTOREFERER, options, "auto-referer", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PROXYPORT, options, "proxy-port", JANET_NUMBER);
    setopt(curl, CURLOPT_POSTFIELDSIZE, options, "post-field-size", JANET_NUMBER);
    setopt(curl, CURLOPT_HTTPPROXYTUNNEL, options, "http-proxy-tunnel", JANET_BOOLEAN);
    setopt(curl, CURLOPT_INTERFACE, options, "interface", JANET_STRING);
    setopt(curl, CURLOPT_KRBLEVEL, options, "krb-level", JANET_STRING);
    setopt(curl, CURLOPT_SSL_VERIFYPEER, options, "ssl-verify-peer", JANET_BOOLEAN);
    setopt(curl, CURLOPT_CAINFO, options, "ca-info", JANET_STRING);
    setopt(curl, CURLOPT_MAXREDIRS, options, "max-re-dirs", JANET_NUMBER);
    setopt(curl, CURLOPT_FILETIME, options, "file-time", JANET_NUMBER);
    setopt(curl, CURLOPT_MAXCONNECTS, options, "max-connects", JANET_NUMBER);
    setopt(curl, CURLOPT_FRESH_CONNECT, options, "fresh-connect", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FORBID_REUSE, options, "forbid-reuse", JANET_BOOLEAN);
    setopt(curl, CURLOPT_RANDOM_FILE, options, "random-file", JANET_STRING);
    setopt(curl, CURLOPT_EGDSOCKET, options, "egd-socket", JANET_STRING);
    setopt(curl, CURLOPT_CONNECTTIMEOUT, options, "connect-timeout", JANET_NUMBER);
    setopt(curl, CURLOPT_HTTPGET, options, "http-get", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_VERIFYHOST, options, "ssl-verify-host", JANET_NUMBER); // is number :)
    setopt(curl, CURLOPT_COOKIEJAR, options, "cookie-jar", JANET_STRING);
    setopt(curl, CURLOPT_SSL_CIPHER_LIST, options, "ssl-cipher-list", JANET_STRING);
    setopt(curl, CURLOPT_HTTP_VERSION, options, "http-version", JANET_NUMBER);
    setopt(curl, CURLOPT_FTP_USE_EPSV, options, "ftp-use-epsv", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSLCERTTYPE, options, "ssl-cert-type", JANET_STRING);
    setopt(curl, CURLOPT_SSLKEY, options, "ssl-key", JANET_STRING);
    setopt(curl, CURLOPT_SSLKEYTYPE, options, "ssl-key-type", JANET_STRING);
    setopt(curl, CURLOPT_SSLENGINE, options, "ssl-engine", JANET_STRING);
    setopt(curl, CURLOPT_SSLENGINE_DEFAULT, options, "ssl-engine_default", JANET_BOOLEAN);
    
  }

  return janet_wrap_nil();
}

/****************
 * easy_perform
 ****************/

static Janet easy_perform(int32_t argc, Janet *argv) {
  janet_arity(argc, 1, 1);

  curl_wrapper_t* curlw = (curl_wrapper_t*) janet_getpointer(argv, 0);  
  if(curlw == NULL) {
    janet_panicf("curl wrapper object is not valid");
  }

  CURL *curl = curlw->curl;
  if(curl == NULL) {
    janet_panicf("curl object is not valid");
  }

  const CURLcode res = curl_easy_perform(curl);
  if (CURLE_OK != res) {
    printf("%s\n", curl_easy_strerror(res));
    janet_wrap_false();
  }

  JanetKV* kv = janet_struct_begin(2);
  janet_struct_put(
    kv, 
    janet_ckeywordv("size"), 
    janet_wrap_integer(curlw->write_data.size));
  janet_struct_put(
    kv, 
    janet_ckeywordv("buffer"), 
    janet_wrap_string(
      (const uint8_t*)curlw->write_data.memory));
  return janet_wrap_struct(kv);  
}

/****************/
/* Module Entry */
/****************/

static const JanetReg cfuns[] = {
    {
      "easy-init", easy_init,
      "TODO: interface \n\n"
      "TODO: description"
    },
    {
      "easy-cleanup", easy_cleanup,
      "TODO: interface \n\n"
      "TODO: description"
    },
    {
      "easy-setopt", easy_setopt,
      "TODO: interface \n\n"
      "TODO: description"
    },
    {
      "easy-perform", easy_perform,
      "TODO: interface \n\n"
      "TODO: description"
    },
    {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable *env) {
    janet_cfuns(env, "curl", cfuns);
}