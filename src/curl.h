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

// callbacks
size_t funcs_write(void *buff, size_t size, size_t count, void *udata);
size_t funcs_read(char *buff, size_t size, size_t count, void *udata);
int funcs_progress(void *udata, double dlt, double dln, double ult, double uln);

// opts
void opts_gen_dict(void);
const JanetKV* opts_get(Janet* key);
void opts_set(CURL* curl, Janet* key, Janet* val);

// base module
curlw_t* curlw_init();
void curlw_deinit(curlw_t* curlw);
CURL* curlw_get_curl_arg(Janet *argv);

// sub modules
void submod_easy(JanetTable *env);