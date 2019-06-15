#include "curl.h"

curlw_t* curlw_init() {
  curlw_t* curlw = (curlw_t*) calloc(1, sizeof(curlw_t));
  if (NULL == curlw) {
    janet_panic("failed to allocate janet curl wrapper object");
  }

  curlw->curl = curl_easy_init();
  if(NULL == curlw->curl) {
    janet_panic("failed to allocate curl object");
  }

  return curlw;
}

void curlw_deinit(curlw_t* curlw){
  if (curlw) {
    if (curlw->curl) {
      curl_easy_cleanup(curlw->curl);
      curlw->curl = NULL;
    }

    free(curlw);
  }
}

CURL* curlw_get_curl_arg(Janet *argv) {
  curlw_t* curlw = janet_getpointer(argv, 0);
  
  if(NULL == curlw) {
    janet_panic("janet curl wrapper object is invalid or null");
  }

  if(NULL == curlw->curl) {
    janet_panic("curl object is invalid or null");
  }

  return curlw->curl;
}