#include "curl.h"

JANET_MODULE_ENTRY(JanetTable *env) {
  opts_gen_dict();

  janet_def(
    env, 
    "version", 
    janet_cstringv(LIBCURL_VERSION),
    "underlying libcurl value string");

  submod_easy(env);
}