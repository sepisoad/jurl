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

// static Janet easy_cleanup(int32_t argc, Janet *argv) {
//   janet_fixarity(argc, 1);
//   return janet_wrap_nil();
// }

static const JanetReg cfuns[] = {
    {
      "easy/init", easy_init,
      "(def curl (easy/init)) \n\n"
      "creates a new curl handler"
    },
    {
      "easy/cleanup", easy_cleanup,
      "(easy/cleanup curl) \n\n"
      "deletes an already initialized curl handler"
    },
    {
      "easy/escape", easy_escape,
      "(easy/escape curl \"https://janet-lang.org\") \n\n"
      "encodes the input string as an url escaped string"
    },
    {
      "easy/unescape", easy_unescape,
      "(easy/unescape curl \"https\%3A\%2F\%2Fjanet-lang.org\") \n\n"
      "decodes the input url string as an unescape string"
    },
    {
      "easy/strerror", easy_strerror,
      "(easy/strerror code) \n\n"
      "returns error string of an error code"
    },
    {
      "easy/setopt", easy_setopt,
      "(easy/setopt code :url \"https://janet-lang.org\") \n\n"
      "sets options for a curl easy handle"
    },    
    {NULL, NULL, NULL}
};

void submod_easy(JanetTable *env) {
    janet_cfuns(env, "curl", cfuns);
}