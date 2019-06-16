#include "curl.h"

size_t funcs_write(void *buff, size_t size, size_t count, void *udata) {
  const size_t len = size * count;

  if (len > 0 && buff != NULL) {
    JanetBuffer* jbuff = janet_buffer(len);
    janet_buffer_push_bytes(jbuff, buff, len);
    Janet arg = janet_wrap_buffer(jbuff);

    JanetFunction* jfunc = janet_unwrap_function(*(Janet*)udata);
    janet_call(jfunc, 1, &arg);
  }

  return len;
}

// size_t header_callback(char *buff, size_t size, size_t count, void *udata) {
//   const size_t len = size * count;

//   if (len > 0 && buff != NULL) {
//     JanetBuffer* jbuff = janet_buffer(len);
//     janet_buffer_push_bytes(jbuff, buff, len);
//     Janet arg = janet_wrap_buffer(jbuff);

//     JanetFunction* jfunc = janet_unwrap_function(*(Janet*)udata);
//     janet_call(jfunc, 1, &arg);
//   }

//   return len;
// }

size_t funcs_read(char *buff, size_t size, size_t count, void *udata) {
  Janet len = janet_wrap_integer(size * count);
  JanetFunction* jfunc = janet_unwrap_function((*(Janet*)udata));
  Janet jbuff = janet_call(jfunc, 1, &len);
  buff = (char*) janet_unwrap_string(jbuff);

  printf("DEBUF\n%s\n", buff);

  return (size_t) strlen(buff);
}

int funcs_progress(void *udata, double dltotal, double dlnow, double ultotal, double ulnow) {
  Janet dlt = janet_wrap_number(dltotal);
  Janet dln = janet_wrap_number(dlnow);
  Janet ult = janet_wrap_number(ultotal);
  Janet uln = janet_wrap_number(ulnow);
  JanetFunction* jfunc = janet_unwrap_function((*(Janet*)udata));
  
  Janet* args = calloc(4, sizeof(Janet));  
  args[0] = dlt;
  args[1] = dln; 
  args[2] = ult; 
  args[3] = uln;

  Janet res = janet_call(jfunc, 4, args);
  if (janet_unwrap_boolean(res)) {
    return 0; // stop
  }

  free(args);

  return 1; // continue
}