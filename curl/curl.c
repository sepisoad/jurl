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

    /* == JANET_BOOLEAN == */    
    setopt(curl, CURLOPT_VERBOSE, options, "verbose", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HEADER, options, "header", JANET_BOOLEAN);
    setopt(curl, CURLOPT_NOPROGRESS, options, "no-progress", JANET_BOOLEAN);
    setopt(curl, CURLOPT_NOSIGNAL, options, "no-signal", JANET_BOOLEAN);
    setopt(curl, CURLOPT_WILDCARDMATCH, options, "wildcard-match", JANET_BOOLEAN);
    setopt(curl, CURLOPT_NOBODY, options, "no-body", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FAILONERROR, options, "fail-on-error", JANET_BOOLEAN);
    setopt(curl, CURLOPT_UPLOAD, options, "upload", JANET_BOOLEAN);
    setopt(curl, CURLOPT_DIRLISTONLY, options, "dir-list-only", JANET_BOOLEAN);
    setopt(curl, CURLOPT_APPEND, options, "append", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FOLLOWLOCATION, options, "follow-location", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TRANSFERTEXT, options, "transfer-text", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PUT, options, "put", JANET_BOOLEAN);
    setopt(curl, CURLOPT_AUTOREFERER, options, "auto-referer", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HTTPPROXYTUNNEL, options, "http-proxy-tunnel", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_VERIFYPEER, options, "ssl-verify-peer", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FRESH_CONNECT, options, "fresh-connect", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FORBID_REUSE, options, "forbid-reuse", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HTTPGET, options, "http-get", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FTP_USE_EPSV, options, "ftp-use-epsv", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSLENGINE_DEFAULT, options, "ssl-engine_default", JANET_BOOLEAN);
    setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, options, "dns-use-global-cache", JANET_BOOLEAN);
    setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, options, "dns-use-global-cache", JANET_BOOLEAN);
    setopt(curl, CURLOPT_COOKIESESSION, options, "cookie-session", JANET_BOOLEAN);
    setopt(curl, CURLOPT_UNRESTRICTED_AUTH, options, "unrestricted-auth", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FTP_USE_EPRT, options, "ftp-use-eprt", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TCP_NODELAY, options, "tcp-no-delay", JANET_BOOLEAN);
    setopt(curl, CURLOPT_IGNORE_CONTENT_LENGTH, options, "ignore-content-length", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, options, "ftp-skip-pasv-ip", JANET_BOOLEAN);
    setopt(curl, CURLOPT_CONNECT_ONLY, options, "connect-only", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, options, "ssl-session-id-cache", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HTTP_TRANSFER_DECODING, options, "http-transfer-decoding", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HTTP_CONTENT_DECODING, options, "http-content-decoding", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PROXY_TRANSFER_MODE, options, "proxy-transfer-mode", JANET_BOOLEAN);
    setopt(curl, CURLOPT_CERTINFO, options, "cert-info", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SOCKS5_GSSAPI_NEC, options, "socks5-gssapi-nec", JANET_BOOLEAN);
    setopt(curl, CURLOPT_FTP_USE_PRET, options, "ftp-use-pret", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TRANSFER_ENCODING, options, "transfer-encoding", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TCP_KEEPALIVE, options, "tcp-keep-alive", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SASL_IR, options, "sasl-ir", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_ENABLE_NPN, options, "ssl-enable-npn", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_ENABLE_ALPN, options, "ssl-enable-alpn", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_VERIFYSTATUS, options, "ssl-verify-status", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSL_FALSESTART, options, "ssl-false-start", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PATH_AS_IS, options, "path-as-is", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PIPEWAIT, options, "pipe-wait", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TFTP_NO_OPTIONS, options, "tftp-no-options", JANET_BOOLEAN);
    setopt(curl, CURLOPT_TCP_FASTOPEN, options, "tcp-fast-open", JANET_BOOLEAN);
    setopt(curl, CURLOPT_KEEP_SENDING_ON_ERROR, options, "keep-sending-on-error", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PROXY_SSL_VERIFYPEER, options, "proxy-ssl-verify-peer", JANET_BOOLEAN);
    setopt(curl, CURLOPT_PROXY_SSL_VERIFYHOST, options, "proxy-ssl-verify-host", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SUPPRESS_CONNECT_HEADERS, options, "suppress-connect-headers", JANET_BOOLEAN);
    setopt(curl, CURLOPT_SSH_COMPRESSION, options, "ssh-compression", JANET_BOOLEAN);
    setopt(curl, CURLOPT_HAPROXYPROTOCOL, options, "ha-proxy-protocol", JANET_BOOLEAN);
    setopt(curl, CURLOPT_DNS_SHUFFLE_ADDRESSES, options, "dns-shuffle-addresses", JANET_BOOLEAN);
    setopt(curl, CURLOPT_DISALLOW_USERNAME_IN_URL, options, "disallow-username-in-url", JANET_BOOLEAN);
    /* == JANET_STRING == */
    setopt(curl, CURLOPT_URL, options, "url", JANET_STRING);
    setopt(curl, CURLOPT_PROXY, options, "proxy", JANET_STRING);
    setopt(curl, CURLOPT_USERPWD, options, "user-pass", JANET_STRING);
    setopt(curl, CURLOPT_PROXYUSERPWD, options, "proxy-user-pass", JANET_STRING);
    setopt(curl, CURLOPT_RANGE, options, "range", JANET_STRING);    
    setopt(curl, CURLOPT_POSTFIELDS, options, "post-fields", JANET_STRING);
    setopt(curl, CURLOPT_REFERER, options, "referer", JANET_STRING);
    setopt(curl, CURLOPT_USERAGENT, options, "user-agent", JANET_STRING);
    setopt(curl, CURLOPT_COOKIE, options, "cookie", JANET_STRING);
    setopt(curl, CURLOPT_SSLCERT, options, "ssl-cert", JANET_STRING);
    setopt(curl, CURLOPT_KEYPASSWD, options, "key-pass", JANET_STRING);
    setopt(curl, CURLOPT_COOKIEFILE, options, "cookie-file", JANET_STRING);
    setopt(curl, CURLOPT_CUSTOMREQUEST, options, "custom-request", JANET_STRING);
    setopt(curl, CURLOPT_INTERFACE, options, "interface", JANET_STRING);
    setopt(curl, CURLOPT_KRBLEVEL, options, "krb-level", JANET_STRING);
    setopt(curl, CURLOPT_CAINFO, options, "ca-info", JANET_STRING);
    setopt(curl, CURLOPT_RANDOM_FILE, options, "random-file", JANET_STRING);
    setopt(curl, CURLOPT_EGDSOCKET, options, "egd-socket", JANET_STRING);
    setopt(curl, CURLOPT_COOKIEJAR, options, "cookie-jar", JANET_STRING);
    setopt(curl, CURLOPT_SSL_CIPHER_LIST, options, "ssl-cipher-list", JANET_STRING);
    setopt(curl, CURLOPT_SSLCERTTYPE, options, "ssl-cert-type", JANET_STRING);
    setopt(curl, CURLOPT_SSLKEY, options, "ssl-key", JANET_STRING);
    setopt(curl, CURLOPT_SSLKEYTYPE, options, "ssl-key-type", JANET_STRING);
    setopt(curl, CURLOPT_SSLENGINE, options, "ssl-engine", JANET_STRING);
    setopt(curl, CURLOPT_CAPATH, options, "ca-path", JANET_STRING);
    setopt(curl, CURLOPT_ACCEPT_ENCODING, options, "accept-encoding", JANET_STRING);
    setopt(curl, CURLOPT_NETRC_FILE, options, "netrc-file", JANET_STRING);
    setopt(curl, CURLOPT_FTP_ACCOUNT, options, "ftp-account", JANET_STRING);
    setopt(curl, CURLOPT_COOKIELIST, options, "cookie-list", JANET_STRING);
    setopt(curl, CURLOPT_FTP_ALTERNATIVE_TO_USER, options, "ftp-alternative-to-user", JANET_STRING);
    setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, options, "ssh-public-key-file", JANET_STRING);
    setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, options, "ssh-private-key-file", JANET_STRING);
    setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, options, "ftp-response-timeout", JANET_STRING);
    setopt(curl, CURLOPT_MAXFILESIZE, options, "maxfilesize", JANET_STRING);
    setopt(curl, CURLOPT_INFILESIZE_LARGE, options, "infilesize-large", JANET_STRING);
    setopt(curl, CURLOPT_RESUME_FROM_LARGE, options, "resume-from-large", JANET_STRING);
    setopt(curl, CURLOPT_MAXFILESIZE_LARGE, options, "maxfilesize-large", JANET_STRING);
    setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, options, "postfieldsize-large", JANET_STRING);
    setopt(curl, CURLOPT_LOCALPORT, options, "localport", JANET_STRING);
    setopt(curl, CURLOPT_LOCALPORTRANGE, options, "localportrange", JANET_STRING);
    setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, options, "max-send-speed-large", JANET_STRING);
    setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, options, "max-recv-speed-large", JANET_STRING);
    setopt(curl, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, options, "ssh-host-public-key-md5", JANET_STRING);
    setopt(curl, CURLOPT_COPYPOSTFIELDS, options, "copy-post-fields", JANET_STRING);
    setopt(curl, CURLOPT_CRLFILE, options, "crl-file", JANET_STRING);
    setopt(curl, CURLOPT_ISSUERCERT, options, "issuer-cert", JANET_STRING);
    setopt(curl, CURLOPT_USERNAME, options, "username", JANET_STRING);
    setopt(curl, CURLOPT_PASSWORD, options, "password", JANET_STRING);
    setopt(curl, CURLOPT_PROXYUSERNAME, options, "proxy-username", JANET_STRING);
    setopt(curl, CURLOPT_PROXYPASSWORD, options, "proxy-password", JANET_STRING);
    setopt(curl, CURLOPT_NOPROXY, options, "no-proxy", JANET_STRING);
    setopt(curl, CURLOPT_SOCKS5_GSSAPI_SERVICE, options, "socks5-gss-api-service", JANET_STRING);
    setopt(curl, CURLOPT_SSH_KNOWNHOSTS, options, "ssh-known-hosts", JANET_STRING);
    setopt(curl, CURLOPT_MAIL_FROM, options, "mail-from", JANET_STRING);
    setopt(curl, CURLOPT_RTSP_SESSION_ID, options, "rtsp-session-id", JANET_STRING);
    setopt(curl, CURLOPT_RTSP_STREAM_URI, options, "rtsp-stream-uri", JANET_STRING);
    setopt(curl, CURLOPT_RTSP_TRANSPORT, options, "rtsp-transport", JANET_STRING);
    setopt(curl, CURLOPT_TLSAUTH_USERNAME, options, "tls-auth-username", JANET_STRING);
    setopt(curl, CURLOPT_TLSAUTH_PASSWORD, options, "tls-auth-password", JANET_STRING);
    setopt(curl, CURLOPT_TLSAUTH_TYPE, options, "tls-auth-type", JANET_STRING);
    setopt(curl, CURLOPT_DNS_SERVERS, options, "dns-servers", JANET_STRING);
    setopt(curl, CURLOPT_MAIL_AUTH, options, "mail-auth", JANET_STRING);
    setopt(curl, CURLOPT_XOAUTH2_BEARER, options, "xoauth2-bearer", JANET_STRING);
    setopt(curl, CURLOPT_DNS_INTERFACE, options, "dns-interface", JANET_STRING);
    setopt(curl, CURLOPT_DNS_LOCAL_IP4, options, "dns-local-ip4", JANET_STRING);
    setopt(curl, CURLOPT_DNS_LOCAL_IP6, options, "dns-local-ip6", JANET_STRING);
    setopt(curl, CURLOPT_LOGIN_OPTIONS, options, "login-options", JANET_STRING);
    setopt(curl, CURLOPT_PINNEDPUBLICKEY, options, "pinned-public-key", JANET_STRING);
    setopt(curl, CURLOPT_UNIX_SOCKET_PATH, options, "unix-socket-path", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_SERVICE_NAME, options, "proxy-service-name", JANET_STRING);
    setopt(curl, CURLOPT_SERVICE_NAME, options, "service-name", JANET_STRING);
    setopt(curl, CURLOPT_DEFAULT_PROTOCOL, options, "default-protocol", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_CAINFO, options, "proxy-ca-info", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_CAPATH, options, "proxy-ca-path", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_TLSAUTH_USERNAME, options, "proxy-tls-auth-username", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_TLSAUTH_PASSWORD, options, "proxy-tls-auth-password", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_TLSAUTH_TYPE, options, "proxy-tls-auth-type", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_SSLCERT, options, "proxy-ssl-cert", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_SSLCERTTYPE, options, "proxy-ssl-cert-type", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_SSLKEY, options, "proxy-ssl-key", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_SSLKEYTYPE, options, "proxy-ssl-key-type", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_KEYPASSWD, options, "proxy-key-passwd", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_SSL_CIPHER_LIST, options, "proxy-ssl-cipher-list", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_CRLFILE, options, "proxy-crl-file", JANET_STRING);
    setopt(curl, CURLOPT_PRE_PROXY, options, "pre-proxy", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_PINNEDPUBLICKEY, options, "proxy-pinned-public-key", JANET_STRING);
    setopt(curl, CURLOPT_ABSTRACT_UNIX_SOCKET, options, "abstract-unix-socket", JANET_STRING);
    setopt(curl, CURLOPT_REQUEST_TARGET, options, "request-target", JANET_STRING);
    setopt(curl, CURLOPT_TLS13_CIPHERS, options, "tls13-ciphers", JANET_STRING);
    setopt(curl, CURLOPT_PROXY_TLS13_CIPHERS, options, "proxy-tls13-ciphers", JANET_STRING);
    /* == JANET_NUMBER == */
    setopt(curl, CURLOPT_PORT, options, "port", JANET_NUMBER);
    setopt(curl, CURLOPT_TIMEOUT, options, "timeout", JANET_NUMBER);    
    setopt(curl, CURLOPT_INFILESIZE, options, "in-file-size", JANET_NUMBER);
    setopt(curl, CURLOPT_FTPPORT, options, "ftp-port", JANET_NUMBER);
    setopt(curl, CURLOPT_LOW_SPEED_LIMIT, options, "low-speed-limit", JANET_NUMBER);
    setopt(curl, CURLOPT_LOW_SPEED_TIME, options, "low-speed-time", JANET_NUMBER);
    setopt(curl, CURLOPT_RESUME_FROM, options, "resume-from", JANET_NUMBER);
    setopt(curl, CURLOPT_CRLF, options, "crlf", JANET_NUMBER);
    setopt(curl, CURLOPT_SSLVERSION, options, "ssl-version", JANET_NUMBER);
    setopt(curl, CURLOPT_TIMECONDITION, options, "time-condition", JANET_NUMBER);
    setopt(curl, CURLOPT_TIMEVALUE, options, "time-value", JANET_NUMBER);
    setopt(curl, CURLOPT_PROXYPORT, options, "proxy-port", JANET_NUMBER);
    setopt(curl, CURLOPT_POSTFIELDSIZE, options, "post-field-size", JANET_NUMBER);
    setopt(curl, CURLOPT_MAXREDIRS, options, "max-re-dirs", JANET_NUMBER);
    setopt(curl, CURLOPT_FILETIME, options, "file-time", JANET_NUMBER);
    setopt(curl, CURLOPT_MAXCONNECTS, options, "max-connects", JANET_NUMBER);
    setopt(curl, CURLOPT_CONNECTTIMEOUT, options, "connect-timeout", JANET_NUMBER);
    setopt(curl, CURLOPT_SSL_VERIFYHOST, options, "ssl-verify-host", JANET_NUMBER); // is number :)
    setopt(curl, CURLOPT_HTTP_VERSION, options, "http-version", JANET_NUMBER);
    setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, options, "dns-cache-timeout", JANET_NUMBER);
    setopt(curl, CURLOPT_BUFFERSIZE, options, "buffersize", JANET_NUMBER);  
    setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, options, "connect-timeout-ms", JANET_NUMBER);
    setopt(curl, CURLOPT_NEW_FILE_PERMS, options, "new-file-perms", JANET_NUMBER);
    setopt(curl, CURLOPT_NEW_DIRECTORY_PERMS, options, "new-directory-perms", JANET_NUMBER);
    setopt(curl, CURLOPT_ADDRESS_SCOPE, options, "address-scope", JANET_NUMBER);
    setopt(curl, CURLOPT_TFTP_BLKSIZE, options, "tftp-blk-size", JANET_NUMBER);
    setopt(curl, CURLOPT_RTSP_CLIENT_CSEQ, options, "rtsp-client-cseq", JANET_NUMBER);
    setopt(curl, CURLOPT_RTSP_SERVER_CSEQ, options, "rtsp-server-cseq", JANET_NUMBER);
    setopt(curl, CURLOPT_ACCEPTTIMEOUT_MS, options, "accept-timeout-ms", JANET_NUMBER);
    setopt(curl, CURLOPT_TCP_KEEPIDLE, options, "tcp-keep-idle", JANET_NUMBER);
    setopt(curl, CURLOPT_TCP_KEEPINTVL, options, "tcp-keep-intvl", JANET_NUMBER);
    setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, options, "expect-100-timeout-ms", JANET_NUMBER);
    setopt(curl, CURLOPT_STREAM_WEIGHT, options, "stream-weight", JANET_NUMBER);
    setopt(curl, CURLOPT_TIMEVALUE_LARGE, options, "time-value-large", JANET_NUMBER);
    setopt(curl, CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, options, "happy-eyeballs-timeout-ms", JANET_NUMBER);
    /* == JANET_XXX == */
    
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