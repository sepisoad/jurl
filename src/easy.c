#include "curl.h"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return Janet 
 */
static Janet easy_init(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 0);
  (void)argv;

  curlw_t* curlw = curlw_init();
  return janet_wrap_pointer(curlw);
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return Janet 
 */
static Janet easy_cleanup(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 1);

  curlw_t* curlw = janet_getpointer(argv, 0);
  curlw_deinit(curlw);

  return janet_wrap_nil();
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return Janet 
 */
static Janet easy_escape(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);

  CURL* curl = curlw_get_curl_arg(argv);
  const char* iurl = janet_getcstring(argv, 1);
  if (NULL == iurl) {
    janet_panic("input url string is invalid or null");
  }

  int len = strlen(iurl);
  if (0 >= len) {
    janet_panic("input url cannot be empty");
  }

  char* ourl = curl_easy_escape(curl, iurl, len);
  if (NULL == ourl) {
    return janet_wrap_false();
  }

  Janet res = janet_cstringv(ourl);
  curl_free(ourl);

  return res;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return Janet 
 */
static Janet easy_unescape(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);

  CURL* curl = curlw_get_curl_arg(argv);
  const char* iurl = janet_getcstring(argv, 1);
  if (NULL == iurl) {
    janet_panic("input url string is invalid or null");
  }

  int ilen = strlen(iurl);
  if (0 >= ilen) {
    janet_panic("input url cannot be empty");
  }

  int olen = 0;

  char* ourl = curl_easy_unescape(curl, iurl, ilen, &olen);
  if (NULL == ourl || 0 >= olen) {
    return janet_wrap_false();
  }

  Janet res = janet_cstringv(ourl);
  curl_free(ourl);

  return res;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return Janet 
 */
static Janet easy_strerror(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 1);
  
  int code = janet_getinteger(argv, 0);
  
  const char* errstr = curl_easy_strerror(code);
  Janet res = janet_cstringv(errstr);
  
  return res;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return Janet 
 */
static Janet easy_setopt(int32_t argc, Janet *argv) {
  janet_arity(argc, 2, 1000); // TODO: 1000 WTF??? find a better solution

  if ((argc - 1) % 2 != 0) {
    janet_panic("options count must be even, options are regarded as tuple");
  }

  CURL* curl = curlw_get_curl_arg(argv);
  
  for (int32_t idx = 1; idx < argc; idx+=2){    
    opts_set(curl, argv + idx, argv + idx + 1);
  }

  return janet_wrap_true();
}

static Janet easy_pause(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);

  CURL* curl = curlw_get_curl_arg(argv);
  const char* modestr = (const char*) janet_getkeyword(argv, 1);
  
  int mode = -1;
  if (0 == strcmp(modestr, "pause-mode-recv")){
    mode = CURLPAUSE_RECV;
  } else if (0 == strcmp(modestr, "pause-mode-send")){
    mode = CURLPAUSE_SEND;
  } else if (0 == strcmp(modestr,"paus-modee-all")){
    mode = CURLPAUSE_ALL;
  } else if (0 == strcmp(modestr, "pause-mode-cont")){
    mode = CURLPAUSE_CONT;
  } else {
    janet_panic("invalid pause mode");
  }

  curl_easy_pause(curl, mode);
  return janet_wrap_true();
}

static Janet easy_reset(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 1);

  CURL* curl = curlw_get_curl_arg(argv);
  
  curl_easy_reset(curl);
  return janet_wrap_nil();
}

static Janet easy_upkeep(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 1);

  CURL* curl = curlw_get_curl_arg(argv);
  
  curl_easy_upkeep(curl);
  return janet_wrap_nil();
}

static Janet easy_recv(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);

  CURL* curl = curlw_get_curl_arg(argv);

  size_t recvlen = -1;
  const size_t buflen = (size_t) janet_getinteger(argv, 1);
  char* buffer = (char*) calloc(buflen, sizeof(char));

  int res = curl_easy_recv(curl, buffer, buflen, &recvlen);

  Janet retval;

  switch (res)
  {
  case CURLE_OK:
    retval = janet_wrap_string(buffer);
    break;
  case CURLE_AGAIN:
    retval = janet_ckeywordv("more");
    break;
  case CURLE_UNSUPPORTED_PROTOCOL:
    retval = janet_ckeywordv("unsopported");
    break;
  default:
    retval = janet_wrap_false();
  }

  return retval;
}

static Janet easy_send(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 2);

  CURL* curl = curlw_get_curl_arg(argv);

  size_t recvlen = -1;
  const uint8_t* buffer = janet_getstring(argv, 1);
  const size_t buflen = (size_t) janet_string_length(argv + 1);
   
  int res = curl_easy_send(curl, buffer, buflen, &recvlen);

  switch (res)
  {
  case CURLE_OK:
    return janet_wrap_true();
    break;
  case CURLE_AGAIN:
    return janet_ckeywordv("more");
    break;
  case CURLE_UNSUPPORTED_PROTOCOL:
    return janet_ckeywordv("unsopported");
    break;
  default:
    return janet_wrap_false();
  }
}

static Janet easy_perform(int32_t argc, Janet *argv) {
  janet_fixarity(argc, 1);

  CURL* curl = curlw_get_curl_arg(argv);
  
  curl_easy_perform(curl);
  return janet_wrap_nil();
}

static const JanetReg cfuns[] = {
    {
      "easy/init", easy_init,
      "(def curl (easy/init)) \n\n"
      "create a new curl handler"
    },
    {
      "easy/cleanup", easy_cleanup,
      "(easy/cleanup curl) \n\n"
      "delete an already initialized curl handler"
    },
    {
      "easy/escape", easy_escape,
      "(easy/escape curl \"https://janet-lang.org\") \n\n"
      "encode the input string as an url escaped string"
    },
    {
      "easy/unescape", easy_unescape,
      "(easy/unescape curl \"https\%3A\%2F\%2Fjanet-lang.org\") \n\n"
      "decode the input url string as an unescape string"
    },
    {
      "easy/strerror", easy_strerror,
      "(easy/strerror code) \n\n"
      "return error string of an error code"
    },
    {
      "easy/setopt", easy_setopt,
      "(easy/setopt code :url \"https://janet-lang.org\") \n\n"
      "set options for a curl easy handle"
    },
    {
      "easy/pause", easy_pause,
      "(easy/pause curl :pause_mode_all) \n\n"
      "pause and unpause a connection"
    },
    {
      "easy/reset", easy_reset,
      "(easy/reset curl) \n\n"
      "reset all options of a libcurl session handle"
    },
    {
      "easy/upkeep", easy_upkeep,
      "(easy/upkeep curl) \n\n"
      "perform any connection upkeep checks"
    },
    {
      "easy/recv", easy_recv,
      "(easy/recv curl) \n\n"
      "receives raw data on an \"easy\" connection"
    },
    {
      "easy/send", easy_send,
      "(easy/send curl) \n\n"
      "sends raw data over an \"easy\" connection"
    },
    {
      "easy/perform", easy_perform,
      "(easy/perform curl) \n\n"
      "perform a blocking file transfer"
    },    
    {NULL, NULL, NULL}
};

void submod_easy(JanetTable *env) {
    janet_cfuns(env, "curl", cfuns);
}