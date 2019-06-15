#pragma once

#include <janet/janet.h>
#include <curl/curl.h>

// types
struct curlw
{
  CURL* curl;
};

// typedefs
typedef struct curlw curlw_t;

// opts
void opts_gen_dict(void);
const JanetKV* opts_get(Janet* key);
int opts_set(CURL* curl, Janet* key, Janet* val);

// base module
curlw_t* curlw_init();
void curlw_deinit(curlw_t* curlw);
CURL* curlw_get_curl_arg(Janet *argv);

// sub modules
void submod_easy(JanetTable *env);