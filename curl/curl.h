#pragma once

typedef size_t write_callback_t(char *ptr, size_t size, size_t nmemb, void *userdata);
typedef size_t read_callback_t(char *buffer, size_t size, size_t nitems, void *stream);
typedef struct write_data_t write_data_t;
typedef struct curl_wrapper_t curl_wrapper_t;

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);
size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);
void setopt(CURL *curl, int curlopt, const JanetKV* options, const char* optkey, JanetType type);

struct write_data_t {
  char *memory;
  size_t size;
};

struct curl_wrapper_t {
  CURL *curl;
  write_callback_t* write_callback;
  write_data_t write_data;
};