#include "curl.h"
#include "khash.h"

typedef struct _curl_janet_map _curl_janet_map_t;

struct _curl_janet_map {
  int copt;
  JanetType jtype;
};

KHASH_MAP_INIT_STR(_curl_janet_hashmap_t, _curl_janet_map_t*);
khash_t(_curl_janet_hashmap_t) *_opt_hash_map = NULL;

// ! => mask
static struct  {
  char name[128];
  _curl_janet_map_t kjm;  
} _key_opt_type_arr[] = {  
  {"url", {CURLOPT_URL, JANET_STRING}},
  {"proxy", {CURLOPT_PROXY, JANET_STRING}},
  {"user-password", {CURLOPT_USERPWD, JANET_STRING}},
  {"proxy-username-password", {CURLOPT_PROXYUSERPWD, JANET_STRING}},
  {"range", {CURLOPT_RANGE, JANET_STRING}},
  {"referer", {CURLOPT_REFERER, JANET_STRING}},
  {"ftp-port", {CURLOPT_FTPPORT, JANET_STRING}},
  {"user-agent", {CURLOPT_USERAGENT, JANET_STRING}},
  {"cookie", {CURLOPT_COOKIE, JANET_STRING}},
  {"ssl-cert", {CURLOPT_SSLCERT, JANET_STRING}},
  {"key-pass", {CURLOPT_KEYPASSWD, JANET_STRING}},
  {"cookie-file", {CURLOPT_COOKIEFILE, JANET_STRING}},
  {"custom-request", {CURLOPT_CUSTOMREQUEST, JANET_STRING}},
  {"interface", {CURLOPT_INTERFACE, JANET_STRING}},
  {"krb-level", {CURLOPT_KRBLEVEL, JANET_STRING}},
  {"ca-info", {CURLOPT_CAINFO, JANET_STRING}},
  {"random-file", {CURLOPT_RANDOM_FILE, JANET_STRING}},
  {"egd-socket", {CURLOPT_EGDSOCKET, JANET_STRING}},
  {"cookie-jar", {CURLOPT_COOKIEJAR, JANET_STRING}},
  {"ssl-cipher-list", {CURLOPT_SSL_CIPHER_LIST, JANET_STRING}},
  {"ssl-cert-type", {CURLOPT_SSLCERTTYPE, JANET_STRING}},
  {"ssl-key", {CURLOPT_SSLKEY, JANET_STRING}},
  {"ssl-key-type", {CURLOPT_SSLKEYTYPE, JANET_STRING}},
  {"ssl-engine", {CURLOPT_SSLENGINE, JANET_STRING}},
  {"ca-path", {CURLOPT_CAPATH, JANET_STRING}},
  {"accept-encoding", {CURLOPT_ACCEPT_ENCODING, JANET_STRING}},
  {"net-rc-file", {CURLOPT_NETRC_FILE, JANET_STRING}},
  {"ftp-account", {CURLOPT_FTP_ACCOUNT, JANET_STRING}},
  {"cookie-list", {CURLOPT_COOKIELIST, JANET_STRING}},
  {"ftp-alternative-to-user", {CURLOPT_FTP_ALTERNATIVE_TO_USER, JANET_STRING}},
  {"ssh-public-key-file", {CURLOPT_SSH_PUBLIC_KEYFILE, JANET_STRING}},
  {"ssh-private-key-file", {CURLOPT_SSH_PRIVATE_KEYFILE, JANET_STRING}},
  {"ssh-host-public-key-md5", {CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, JANET_STRING}},
  {"crl-file", {CURLOPT_CRLFILE, JANET_STRING}},
  {"issuer-cert", {CURLOPT_ISSUERCERT, JANET_STRING}},
  {"username", {CURLOPT_USERNAME, JANET_STRING}},
  {"password", {CURLOPT_PASSWORD, JANET_STRING}},
  {"proxy-username", {CURLOPT_PROXYUSERNAME, JANET_STRING}},
  {"proxy-password", {CURLOPT_PROXYPASSWORD, JANET_STRING}},
  {"no-proxy", {CURLOPT_NOPROXY, JANET_STRING}},
  {"socks5-gss-api-service", {CURLOPT_SOCKS5_GSSAPI_SERVICE, JANET_STRING}},
  {"ssh-known-hosts", {CURLOPT_SSH_KNOWNHOSTS, JANET_STRING}},
  {"mail-from", {CURLOPT_MAIL_FROM, JANET_STRING}},
  {"rtsp-session-id", {CURLOPT_RTSP_SESSION_ID, JANET_STRING}},
  {"rtsp-stream-uri", {CURLOPT_RTSP_STREAM_URI, JANET_STRING}},
  {"rtsp-transport", {CURLOPT_RTSP_TRANSPORT, JANET_STRING}},
  {"tls-auth-username", {CURLOPT_TLSAUTH_USERNAME, JANET_STRING}},
  {"tls-auth-password", {CURLOPT_TLSAUTH_PASSWORD, JANET_STRING}},
  {"tls-auth-type", {CURLOPT_TLSAUTH_TYPE, JANET_STRING}},
  {"dns-servers", {CURLOPT_DNS_SERVERS, JANET_STRING}},
  {"mail-auth", {CURLOPT_MAIL_AUTH, JANET_STRING}},
  {"xoauth2-bearer", {CURLOPT_XOAUTH2_BEARER, JANET_STRING}},
  {"dns-interface", {CURLOPT_DNS_INTERFACE, JANET_STRING}},
  {"dns-local-ip4", {CURLOPT_DNS_LOCAL_IP4, JANET_STRING}},
  {"dns-local-ip6", {CURLOPT_DNS_LOCAL_IP6, JANET_STRING}},
  {"login-options", {CURLOPT_LOGIN_OPTIONS, JANET_STRING}},
  {"pinned-public-key", {CURLOPT_PINNEDPUBLICKEY, JANET_STRING}},
  {"unix-socket-path", {CURLOPT_UNIX_SOCKET_PATH, JANET_STRING}},
  {"proxy-service-name", {CURLOPT_PROXY_SERVICE_NAME, JANET_STRING}},
  {"service-name", {CURLOPT_SERVICE_NAME, JANET_STRING}},
  {"default-protocol", {CURLOPT_DEFAULT_PROTOCOL, JANET_STRING}},
  {"proxy-ca-info", {CURLOPT_PROXY_CAINFO, JANET_STRING}},
  {"proxy-ca-path", {CURLOPT_PROXY_CAPATH, JANET_STRING}},
  {"proxy-tls-auth-username", {CURLOPT_PROXY_TLSAUTH_USERNAME, JANET_STRING}},
  {"proxy-tls-auth-password", {CURLOPT_PROXY_TLSAUTH_PASSWORD, JANET_STRING}},
  {"proxy-tls-auth-type", {CURLOPT_PROXY_TLSAUTH_TYPE, JANET_STRING}},
  {"proxy-ssl-cert", {CURLOPT_PROXY_SSLCERT, JANET_STRING}},
  {"proxy-ssl-cert-type", {CURLOPT_PROXY_SSLCERTTYPE, JANET_STRING}},
  {"proxy-ssl-key", {CURLOPT_PROXY_SSLKEY, JANET_STRING}},
  {"proxy-ssl-key-type", {CURLOPT_PROXY_SSLKEYTYPE, JANET_STRING}},
  {"proxy-key-password", {CURLOPT_PROXY_KEYPASSWD, JANET_STRING}},
  {"proxy-ssl-cipher-list", {CURLOPT_PROXY_SSL_CIPHER_LIST, JANET_STRING}},
  {"proxy-crl-file", {CURLOPT_PROXY_CRLFILE, JANET_STRING}},
  {"pre-proxy", {CURLOPT_PRE_PROXY, JANET_STRING}},
  {"proxy-pinned-public-key", {CURLOPT_PROXY_PINNEDPUBLICKEY, JANET_STRING}},
  {"abstract-unix-socket", {CURLOPT_ABSTRACT_UNIX_SOCKET, JANET_STRING}},
  {"request-target", {CURLOPT_REQUEST_TARGET, JANET_STRING}},
  {"tls13-ciphers", {CURLOPT_TLS13_CIPHERS, JANET_STRING}},
  {"proxy-tls13-ciphers", {CURLOPT_PROXY_TLS13_CIPHERS, JANET_STRING}},
  {"doh-url", {CURLOPT_DOH_URL, JANET_STRING}},

  {"verbose?", {CURLOPT_VERBOSE, JANET_BOOLEAN}},
  {"header?", {CURLOPT_HEADER, JANET_BOOLEAN}},
  {"no-progress?", {CURLOPT_NOPROGRESS, JANET_BOOLEAN}},
  {"no-body?", {CURLOPT_NOBODY, JANET_BOOLEAN}},
  {"fail-on-error?", {CURLOPT_FAILONERROR, JANET_BOOLEAN}},
  {"upload?", {CURLOPT_UPLOAD, JANET_BOOLEAN}},
  {"post?", {CURLOPT_POST, JANET_BOOLEAN}},
  {"dir-list-only?", {CURLOPT_DIRLISTONLY, JANET_BOOLEAN}},
  {"append?", {CURLOPT_APPEND, JANET_BOOLEAN}},
  {"follow-location?", {CURLOPT_FOLLOWLOCATION, JANET_BOOLEAN}},
  {"transfer-text?", {CURLOPT_TRANSFERTEXT, JANET_BOOLEAN}},
  {"put?", {CURLOPT_PUT, JANET_BOOLEAN}},
  {"auto-referer?", {CURLOPT_AUTOREFERER, JANET_BOOLEAN}},
  {"http-proxy-tunnel?", {CURLOPT_HTTPPROXYTUNNEL, JANET_BOOLEAN}},
  {"ssl-verify-peer?", {CURLOPT_SSL_VERIFYPEER, JANET_BOOLEAN}},
  {"file-time?", {CURLOPT_FILETIME, JANET_BOOLEAN}},
  {"fresh-connect?", {CURLOPT_FRESH_CONNECT, JANET_BOOLEAN}},
  {"forbid-reuse?", {CURLOPT_FORBID_REUSE, JANET_BOOLEAN}},
  {"http-get?", {CURLOPT_HTTPGET, JANET_BOOLEAN}},
  {"ftp-use-epsv?", {CURLOPT_FTP_USE_EPSV, JANET_BOOLEAN}},
  {"ssl-engine-default?", {CURLOPT_SSLENGINE_DEFAULT, JANET_BOOLEAN}},
  {"dns-use-global-cache?", {CURLOPT_DNS_USE_GLOBAL_CACHE, JANET_BOOLEAN}},
  {"cookie-session?", {CURLOPT_COOKIESESSION, JANET_BOOLEAN}},
  {"no-signal?", {CURLOPT_NOSIGNAL, JANET_BOOLEAN}},
  {"unrestricted-auth?", {CURLOPT_UNRESTRICTED_AUTH, JANET_BOOLEAN}},
  {"ftp-use-eprt?", {CURLOPT_FTP_USE_EPRT, JANET_BOOLEAN}},
  {"tcp-no-delay?", {CURLOPT_TCP_NODELAY, JANET_BOOLEAN}},
  {"ignore-content-length?", {CURLOPT_IGNORE_CONTENT_LENGTH, JANET_BOOLEAN}},
  {"ftp-skip-pasv-ip?", {CURLOPT_FTP_SKIP_PASV_IP, JANET_BOOLEAN}},
  {"connect-only?", {CURLOPT_CONNECT_ONLY, JANET_BOOLEAN}},
  {"ssl-sessionid-cache?", {CURLOPT_SSL_SESSIONID_CACHE, JANET_BOOLEAN}},
  {"http-transfer-decoding?", {CURLOPT_HTTP_TRANSFER_DECODING, JANET_BOOLEAN}},
  {"http-content-decoding?", {CURLOPT_HTTP_CONTENT_DECODING, JANET_BOOLEAN}},
  {"proxy-transfer-mode?", {CURLOPT_PROXY_TRANSFER_MODE, JANET_BOOLEAN}},
  {"cert-info?", {CURLOPT_CERTINFO, JANET_BOOLEAN}},
  {"socks5-gss-api-nec?", {CURLOPT_SOCKS5_GSSAPI_NEC, JANET_BOOLEAN}},
  {"ftp-use-pret?", {CURLOPT_FTP_USE_PRET, JANET_BOOLEAN}},
  {"wildcard-match?", {CURLOPT_WILDCARDMATCH, JANET_BOOLEAN}},
  {"transfer-encoding?", {CURLOPT_TRANSFER_ENCODING, JANET_BOOLEAN}},
  {"sasl-ir?", {CURLOPT_SASL_IR, JANET_BOOLEAN}},
  {"ssl-enable-npn?", {CURLOPT_SSL_ENABLE_NPN, JANET_BOOLEAN}},
  {"ssl-enable-alpn?", {CURLOPT_SSL_ENABLE_ALPN, JANET_BOOLEAN}},
  {"ssl-verify-status?", {CURLOPT_SSL_VERIFYSTATUS, JANET_BOOLEAN}},
  {"ssl-false-start?", {CURLOPT_SSL_FALSESTART, JANET_BOOLEAN}},
  {"path-as-is?", {CURLOPT_PATH_AS_IS, JANET_BOOLEAN}},
  {"pipe-wait?", {CURLOPT_PIPEWAIT, JANET_BOOLEAN}},
  {"tftp-no-options?", {CURLOPT_TFTP_NO_OPTIONS, JANET_BOOLEAN}},
  {"tcp-fastopen?", {CURLOPT_TCP_FASTOPEN, JANET_BOOLEAN}},
  {"keep-sending-on-error?", {CURLOPT_KEEP_SENDING_ON_ERROR, JANET_BOOLEAN}},
  {"proxy-ssl-verify-peer?", {CURLOPT_PROXY_SSL_VERIFYPEER, JANET_BOOLEAN}},
  {"suppress-connect-headers?", {CURLOPT_SUPPRESS_CONNECT_HEADERS, JANET_BOOLEAN}},
  {"ssh-compression?", {CURLOPT_SSH_COMPRESSION, JANET_BOOLEAN}},
  {"ha-proxy-protocol?", {CURLOPT_HAPROXYPROTOCOL, JANET_BOOLEAN}},
  {"dns-shuffle-addresses?", {CURLOPT_DNS_SHUFFLE_ADDRESSES, JANET_BOOLEAN}},
  {"disallow-username-in-url?", {CURLOPT_DISALLOW_USERNAME_IN_URL, JANET_BOOLEAN}},
  {"http09-allowed?", {CURLOPT_HTTP09_ALLOWED, JANET_BOOLEAN}},
  {"ssl-verify-host?", {CURLOPT_SSL_VERIFYHOST, JANET_NUMBER}},

  // ENUMS
  {"proxy-type", {CURLOPT_PROXYTYPE, JANET_NUMBER}},  
  {"ftp-create-missing-dirs", {CURLOPT_FTP_CREATE_MISSING_DIRS, JANET_NUMBER}},
  {"ip-resolve", {CURLOPT_IPRESOLVE, JANET_NUMBER}},
  {"use-ssl", {CURLOPT_USE_SSL, JANET_NUMBER}},
  {"ftp-ssl-auth", {CURLOPT_FTPSSLAUTH, JANET_NUMBER}},
  {"ftp-file-method", {CURLOPT_FTP_FILEMETHOD, JANET_NUMBER}},
  {"ftp-ssl-ccc", {CURLOPT_FTP_SSL_CCC, JANET_NUMBER}},
  {"rtsp-request", {CURLOPT_RTSP_REQUEST, JANET_NUMBER}},
  {"gss-api-delegation", {CURLOPT_GSSAPI_DELEGATION, JANET_NUMBER}},
  {"proxy-ssl-verify-host", {CURLOPT_PROXY_SSL_VERIFYHOST, JANET_NUMBER}},
  {"proxy-ssl-version", {CURLOPT_PROXY_SSLVERSION, JANET_NUMBER}},
  {"netrc", {CURLOPT_NETRC, JANET_NUMBER}},  

  // MASKS    
  {"ssh-auth-types", {CURLOPT_SSH_AUTH_TYPES, JANET_NUMBER}},
  {"post-redir", {CURLOPT_POSTREDIR, JANET_NUMBER}},
  {"protocols", {CURLOPT_PROTOCOLS, JANET_NUMBER}},
  {"redir-protocols", {CURLOPT_REDIR_PROTOCOLS, JANET_NUMBER}},
  {"ssl-options", {CURLOPT_SSL_OPTIONS, JANET_NUMBER}},
  {"header-opt", {CURLOPT_HEADEROPT, JANET_NUMBER}},
  {"proxy-ssl-options", {CURLOPT_PROXY_SSL_OPTIONS, JANET_NUMBER}},
  {"http-auth", {CURLOPT_HTTPAUTH, JANET_NUMBER}},
  {"proxy-auth", {CURLOPT_PROXYAUTH, JANET_NUMBER}},  
  {"socks5-auth", {CURLOPT_SOCKS5_AUTH, JANET_NUMBER}},

  {"port", {CURLOPT_PORT, JANET_NUMBER}},
  {"timeout", {CURLOPT_TIMEOUT, JANET_NUMBER}},
  {"infilesize", {CURLOPT_INFILESIZE, JANET_NUMBER}},
  {"low-speed-limit", {CURLOPT_LOW_SPEED_LIMIT, JANET_NUMBER}},
  {"low-speed-time", {CURLOPT_LOW_SPEED_TIME, JANET_NUMBER}},
  {"resume-from", {CURLOPT_RESUME_FROM, JANET_NUMBER}},
  {"crlf", {CURLOPT_CRLF, JANET_NUMBER}},
  {"ssl-version", {CURLOPT_SSLVERSION, JANET_NUMBER}},
  {"time-condition", {CURLOPT_TIMECONDITION, JANET_NUMBER}},
  {"time-value", {CURLOPT_TIMEVALUE, JANET_NUMBER}},
  {"proxy-port", {CURLOPT_PROXYPORT, JANET_NUMBER}},
  {"post-field-size", {CURLOPT_POSTFIELDSIZE, JANET_NUMBER}},
  {"max-redirs", {CURLOPT_MAXREDIRS, JANET_NUMBER}},
  {"max-connects", {CURLOPT_MAXCONNECTS, JANET_NUMBER}},
  {"obsolete72", {CURLOPT_OBSOLETE72, JANET_NUMBER}},
  {"connect-timeout", {CURLOPT_CONNECTTIMEOUT, JANET_NUMBER}},
  {"http-version", {CURLOPT_HTTP_VERSION, JANET_NUMBER}},
  {"dns-cache-timeout", {CURLOPT_DNS_CACHE_TIMEOUT, JANET_NUMBER}},
  {"buffer-size", {CURLOPT_BUFFERSIZE, JANET_NUMBER}},
  {"ftp-response-timeout", {CURLOPT_FTP_RESPONSE_TIMEOUT, JANET_NUMBER}},
  {"max-filesize", {CURLOPT_MAXFILESIZE, JANET_NUMBER}},
  {"local-port", {CURLOPT_LOCALPORT, JANET_NUMBER}},
  {"local-portrange", {CURLOPT_LOCALPORTRANGE, JANET_NUMBER}},
  {"timeout-ms", {CURLOPT_TIMEOUT_MS, JANET_NUMBER}},
  {"connect-timeout-ms", {CURLOPT_CONNECTTIMEOUT_MS, JANET_NUMBER}},
  {"new-file-perms", {CURLOPT_NEW_FILE_PERMS, JANET_NUMBER}},
  {"new-directory-perms", {CURLOPT_NEW_DIRECTORY_PERMS, JANET_NUMBER}},
  {"address-scope", {CURLOPT_ADDRESS_SCOPE, JANET_NUMBER}},
  {"tftp-blk-size", {CURLOPT_TFTP_BLKSIZE, JANET_NUMBER}},
  {"rtsp-client-cseq", {CURLOPT_RTSP_CLIENT_CSEQ, JANET_NUMBER}},
  {"rtsp-server-cseq", {CURLOPT_RTSP_SERVER_CSEQ, JANET_NUMBER}},
  {"accept-timeout-ms", {CURLOPT_ACCEPTTIMEOUT_MS, JANET_NUMBER}},
  {"tcp-keep-alive", {CURLOPT_TCP_KEEPALIVE, JANET_NUMBER}},
  {"tcp-keep-idle", {CURLOPT_TCP_KEEPIDLE, JANET_NUMBER}},
  {"tcp-keep-intvl", {CURLOPT_TCP_KEEPINTVL, JANET_NUMBER}},
  {"expect-100-timeout-ms", {CURLOPT_EXPECT_100_TIMEOUT_MS, JANET_NUMBER}},
  {"stream-weight", {CURLOPT_STREAM_WEIGHT, JANET_NUMBER}},
  {"happy-eyeballs-timeout-ms", {CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, JANET_NUMBER}},
  {"upload-buffer-size", {CURLOPT_UPLOAD_BUFFERSIZE, JANET_NUMBER}},
  {"upkeep-interval-ms", {CURLOPT_UPKEEP_INTERVAL_MS, JANET_NUMBER}},

  {"in-file-size-large", {CURLOPT_INFILESIZE_LARGE, JANET_NUMBER}},
  {"resume-from-large", {CURLOPT_RESUME_FROM_LARGE, JANET_NUMBER}},
  {"max-file-size-large", {CURLOPT_MAXFILESIZE_LARGE, JANET_NUMBER}},
  {"post-field-size-large", {CURLOPT_POSTFIELDSIZE_LARGE, JANET_NUMBER}},
  {"max-send-speed-large", {CURLOPT_MAX_SEND_SPEED_LARGE, JANET_NUMBER}},
  {"max-recv-speed-large", {CURLOPT_MAX_RECV_SPEED_LARGE, JANET_NUMBER}},
  {"time-value-large", {CURLOPT_TIMEVALUE_LARGE, JANET_NUMBER}},

  {"write-function", {CURLOPT_WRITEFUNCTION, JANET_FUNCTION}},
  {"header-function", {CURLOPT_HEADERFUNCTION, JANET_FUNCTION}},
  {"read-function", {CURLOPT_READFUNCTION, JANET_FUNCTION}},
  {"progress-function", {CURLOPT_PROGRESSFUNCTION, JANET_FUNCTION}},  
  {"debug-function#TODO", {CURLOPT_DEBUGFUNCTION, JANET_FUNCTION}}, // TODO
  {"ssl-ctx-function#TODO", {CURLOPT_SSL_CTX_FUNCTION, JANET_FUNCTION}}, // TODO
  {"ioctl-function#TODO", {CURLOPT_IOCTLFUNCTION, JANET_FUNCTION}}, // TODO
  {"conv-from-network-function#TODO", {CURLOPT_CONV_FROM_NETWORK_FUNCTION, JANET_FUNCTION}}, // TODO
  {"conv-to-network-function#TODO", {CURLOPT_CONV_TO_NETWORK_FUNCTION, JANET_FUNCTION}}, // TODO
  {"conv-from-utf8-function#TODO", {CURLOPT_CONV_FROM_UTF8_FUNCTION, JANET_FUNCTION}}, // TODO
  {"sockopt-function#TODO", {CURLOPT_SOCKOPTFUNCTION, JANET_FUNCTION}}, // TODO
  {"open-socket-function#TODO", {CURLOPT_OPENSOCKETFUNCTION, JANET_FUNCTION}}, // TODO
  {"seek-function#TODO", {CURLOPT_SEEKFUNCTION, JANET_FUNCTION}}, // TODO
  {"ssh-key-function#TODO", {CURLOPT_SSH_KEYFUNCTION, JANET_FUNCTION}}, // TODO
  {"interleave-function#TODO", {CURLOPT_INTERLEAVEFUNCTION, JANET_FUNCTION}}, // TODO
  {"chunk-bgn-function#TODO", {CURLOPT_CHUNK_BGN_FUNCTION, JANET_FUNCTION}}, // TODO
  {"chunk-end-function#TODO", {CURLOPT_CHUNK_END_FUNCTION, JANET_FUNCTION}}, // TODO
  {"fnmatch-function#TODO", {CURLOPT_FNMATCH_FUNCTION, JANET_FUNCTION}}, // TODO
  {"close-socket-function#TODO", {CURLOPT_CLOSESOCKETFUNCTION, JANET_FUNCTION}}, // TODO
  {"xfer-info-function#TODO", {CURLOPT_XFERINFOFUNCTION, JANET_FUNCTION}}, // TODO
  {"resolver-start-function#TODO", {CURLOPT_RESOLVER_START_FUNCTION, JANET_FUNCTION}}, // TODO
  {"trailer-function#TODO", {CURLOPT_TRAILERFUNCTION, JANET_FUNCTION}}, // TODO

  {"write-data", {CURLOPT_WRITEDATA, JANET_POINTER}},
  {"header-data", {CURLOPT_HEADERDATA, JANET_POINTER}},
  {"read-data", {CURLOPT_READDATA, JANET_POINTER}},
  {"progress-data", {CURLOPT_PROGRESSDATA, JANET_POINTER}},
  {"error-buffer@TODO", {CURLOPT_ERRORBUFFER, JANET_POINTER}}, // TODO
  {"post-fields@TODO", {CURLOPT_POSTFIELDS, JANET_POINTER}}, // TODO
  {"http-header@TODO", {CURLOPT_HTTPHEADER, JANET_POINTER}}, // TODO
  {"http-post@TODO", {CURLOPT_HTTPPOST, JANET_POINTER}}, // TODO
  {"quote@TODO", {CURLOPT_QUOTE, JANET_POINTER}},   // TODO
  {"stderr@TODO", {CURLOPT_STDERR, JANET_POINTER}}, // TODO
  {"post-quote@TODO", {CURLOPT_POSTQUOTE, JANET_POINTER}}, // TODO
  {"obsolete40@TODO", {CURLOPT_OBSOLETE40, JANET_POINTER}},   // TODO
  {"telnet-options@TODO", {CURLOPT_TELNETOPTIONS, JANET_POINTER}}, // TODO
  {"prequote@TODO", {CURLOPT_PREQUOTE, JANET_POINTER}}, // TODO
  {"debug-data@TODO", {CURLOPT_DEBUGDATA, JANET_POINTER}}, // TODO
  {"share@TODO", {CURLOPT_SHARE, JANET_POINTER}}, // TODO
  {"private@TODO", {CURLOPT_PRIVATE, JANET_POINTER}}, // TODO
  {"http200-aliases@TODO", {CURLOPT_HTTP200ALIASES, JANET_POINTER}}, // TODO
  {"ssl-ctx-data@TODO", {CURLOPT_SSL_CTX_DATA, JANET_POINTER}}, // TODO
  {"ioctl-data@TODO", {CURLOPT_IOCTLDATA, JANET_POINTER}}, // TODO
  {"sockopt-data@TODO", {CURLOPT_SOCKOPTDATA, JANET_POINTER}}, // TODO
  {"open-socke-tdata@TODO", {CURLOPT_OPENSOCKETDATA, JANET_POINTER}}, // TODO
  {"copy-post-fields@TODO", {CURLOPT_COPYPOSTFIELDS, JANET_POINTER}}, // TODO
  {"seek-data@TODO", {CURLOPT_SEEKDATA, JANET_POINTER}}, // TODO
  {"ssh-key-data@TODO", {CURLOPT_SSH_KEYDATA, JANET_POINTER}}, // TODO
  {"mail-rcpt@TODO", {CURLOPT_MAIL_RCPT, JANET_POINTER}}, // TODO
  {"interleave-data@TODO", {CURLOPT_INTERLEAVEDATA, JANET_POINTER}}, // TODO
  {"chunk-data@TODO", {CURLOPT_CHUNK_DATA, JANET_POINTER}}, // TODO
  {"fnmatch-data@TODO", {CURLOPT_FNMATCH_DATA, JANET_POINTER}}, // TODO
  {"resolve@TODO", {CURLOPT_RESOLVE, JANET_POINTER}}, // TODO
  {"close-socket-data@TODO", {CURLOPT_CLOSESOCKETDATA, JANET_POINTER}}, // TODO
  {"proxy-header@TODO", {CURLOPT_PROXYHEADER, JANET_POINTER}}, // TODO
  {"stream-depends@TODO", {CURLOPT_STREAM_DEPENDS, JANET_POINTER}}, // TODO
  {"stream-depends-e@TODO", {CURLOPT_STREAM_DEPENDS_E, JANET_POINTER}}, // TODO
  {"connect-to@TODO", {CURLOPT_CONNECT_TO, JANET_POINTER}}, // TODO
  {"mimepost@TODO", {CURLOPT_MIMEPOST, JANET_POINTER}}, // TODO
  {"resolver-start-data@TODO", {CURLOPT_RESOLVER_START_DATA, JANET_POINTER}}, // TODO
  {"curlu@TODO", {CURLOPT_CURLU, JANET_POINTER}}, // TODO
  {"trailer-data@TODO", {CURLOPT_TRAILERDATA, JANET_POINTER}}, // TODO
};

void opts_gen_dict(void) {
  int32_t size = sizeof(_key_opt_type_arr) / sizeof(_key_opt_type_arr[0]);  

  if (NULL == _opt_hash_map) {
    _opt_hash_map = kh_init(_curl_janet_hashmap_t);
  }  

  int absent;
  khint_t where;
  for (int32_t idx = 0; idx < size; idx++){    
    where = kh_put(_curl_janet_hashmap_t, _opt_hash_map, _key_opt_type_arr[idx].name, &absent);
    kh_value(_opt_hash_map, where) = &_key_opt_type_arr[idx].kjm;
  }
}

const _curl_janet_map_t* _opts_get(const char* key) {
  _curl_janet_map_t* val;
  khint_t where = kh_get(_curl_janet_hashmap_t, _opt_hash_map, key);
  val = kh_val(_opt_hash_map, where);
  return val;  
}

void opts_set(CURL* curl, Janet* key, Janet* val) {    
  const char* keyword = (const char*) janet_unwrap_keyword(*key);
  const _curl_janet_map_t* map = _opts_get(keyword);
  if (NULL == map) {
    janet_panic("invalid keyword");
  }
  
  int type = map->jtype;
  int opt = map->copt;
  
  switch(type) {
    case JANET_NUMBER:      
      curl_easy_setopt(curl, opt, janet_unwrap_integer(*val));
      break;
    case JANET_BOOLEAN:
      curl_easy_setopt(curl, opt, janet_unwrap_boolean(*val));
      break;
    case JANET_STRING:      
      curl_easy_setopt(curl, opt, janet_unwrap_string(*val));
      break;
    case JANET_FUNCTION:
      if(0 == strcmp(keyword, "write-function")) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)val);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, funcs_write);
      } else if(0 == strcmp(keyword, "header-function")) {
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)val);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, funcs_write);
      } else if(0 == strcmp(keyword, "read-function")) {
        curl_easy_setopt(curl, CURLOPT_READDATA, (void*)val);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, funcs_read);
      } else if(0 == strcmp(keyword, "progress-function")) {
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, (void*)val);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, funcs_progress);
      }      

      break;
    case JANET_POINTER:
      janet_panic("not implemented yet");
      // TODO:      
      break;
    default:
      janet_panic("value type is not supported");
      break;
  };
}