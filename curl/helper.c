#include <janet.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "curl.h"

size_t write_callback(
  void *contents, 
  size_t size, 
  size_t nmemb, 
  void *userdata)
{
  size_t realsize = size * nmemb;
  struct write_data_t *mem = (struct write_data_t *)userdata;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

size_t read_callback(
  void *ptr, 
  size_t size, 
  size_t nmemb, 
  void *stream)
{
  curl_off_t nread;  
  size_t retcode = fread(ptr, size, nmemb, stream);
 
  nread = (curl_off_t)retcode;
 
  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
          " bytes from file\n", nread);
  return retcode;
}

void setopt(CURL *curl, int curlopt, const JanetKV* options, const char* optkey, JanetType type) {
  const Janet key = janet_ckeywordv(optkey);
  const Janet val = janet_struct_get(options, key);  
  CURLcode res = -1;

  if (janet_checktype(val, type)) {
    switch (type) {
    case JANET_NUMBER: 
      {
        const int32_t value = janet_unwrap_integer(val);
        res = curl_easy_setopt(curl, curlopt, value);
      }
      break;
    case JANET_NIL: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_BOOLEAN: 
      {
        const int value = janet_unwrap_boolean(val);
        res = curl_easy_setopt(curl, curlopt, value? 1L : 0L);
      }
      break;
    case JANET_FIBER: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_STRING: 
      {
        const uint8_t *value = janet_unwrap_string(val);        
        res = curl_easy_setopt(curl, curlopt, (char*)value);
      }
      break;
    case JANET_SYMBOL: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_KEYWORD: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_ARRAY: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_TUPLE: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_TABLE: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_STRUCT: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_BUFFER: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_FUNCTION: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_CFUNCTION: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_ABSTRACT: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    case JANET_POINTER: 
      {
        janet_panicf("this type is not supposed to be used");
      }
      break;
    default:
      break;
  }
    
    if (CURLE_OK != res) {
      janet_panicf(curl_easy_strerror(res));
    }
  }
}