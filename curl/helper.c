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