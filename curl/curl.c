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

    { // CURLOPT_VERBOSE
      const Janet key = janet_ckeywordv("verbose");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_BOOLEAN)) {
        const int value = janet_unwrap_boolean(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_VERBOSE, value? 1L : 0L);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }
    
    { // CURLOPT_HEADER
      const Janet key = janet_ckeywordv("header");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_BOOLEAN)) {
        const int value = janet_unwrap_boolean(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_HEADER, value? 1L : 0L);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }

    { // CURLOPT_NOPROGRESS
      const Janet key = janet_ckeywordv("no-progress");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_BOOLEAN)) {
        const int value = janet_unwrap_boolean(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, value? 1L : 0L);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }

    { // CURLOPT_NOSIGNAL
      const Janet key = janet_ckeywordv("no-signal");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_BOOLEAN)) {
        const int value = janet_unwrap_boolean(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, value? 1L : 0L);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }

    { // CURLOPT_WILDCARDMATCH
      const Janet key = janet_ckeywordv("wildcard-match");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_BOOLEAN)) {
        const int value = janet_unwrap_boolean(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_WILDCARDMATCH, value? 1L : 0L);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }

    /* == CALLBACK OPTIONS == */

    { // CURLOPT_WRITEFUNCTION
      const Janet key = janet_ckeywordv("write-function");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_FUNCTION)) {
        const JanetFunction *value = janet_unwrap_function(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, value);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }

    /* == XXX == */
  
    { // CURLOPT_URL
      const Janet key = janet_ckeywordv("url");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_STRING)) {
        const uint8_t *value = janet_unwrap_string(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, (char*)value);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }
    }

    { // CURLOPT_TIMEOUT
      const Janet key = janet_ckeywordv("timeout");
      const Janet val = janet_struct_get(options, key);
      if (janet_checktype(val, JANET_NUMBER)) {
        const int32_t value = janet_unwrap_integer(val);
        CURLcode res = curl_easy_setopt(curl, CURLOPT_TIMEOUT, value);
        if (CURLE_OK != res) {
          janet_panicf(curl_easy_strerror(res));
        }
      }  
    }
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