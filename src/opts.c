#include "curl.h"
#include "khash.h"

typedef int (*checker_t)(const int value); // TODO:
typedef struct _curl_janet_map _curl_janet_map_t;

struct _curl_janet_map {
  int copt;
  JanetType jtype;
  checker_t checker_t; // TODO:
};

KHASH_MAP_INIT_STR(_curl_janet_hashmap_t, _curl_janet_map_t*);
khash_t(_curl_janet_hashmap_t) *_opt_hash_map = NULL;

// ! => mask
static struct  {
  char name[128];
  _curl_janet_map_t kjm;  
} _key_opt_type_arr[] = {  
  {"url", {CURLOPT_URL, JANET_STRING, NULL}},
  {"proxy", {CURLOPT_PROXY, JANET_STRING, NULL}},
  {"user-password", {CURLOPT_USERPWD, JANET_STRING, NULL}},
  {"proxy-username-password", {CURLOPT_PROXYUSERPWD, JANET_STRING, NULL}},
  {"range", {CURLOPT_RANGE, JANET_STRING, NULL}},
  {"referer", {CURLOPT_REFERER, JANET_STRING, NULL}},
  {"ftp-port", {CURLOPT_FTPPORT, JANET_STRING, NULL}},
  {"user-agent", {CURLOPT_USERAGENT, JANET_STRING, NULL}},
  {"cookie", {CURLOPT_COOKIE, JANET_STRING, NULL}},
  {"ssl-cert", {CURLOPT_SSLCERT, JANET_STRING, NULL}},
  {"key-pass", {CURLOPT_KEYPASSWD, JANET_STRING, NULL}},
  {"cookie-file", {CURLOPT_COOKIEFILE, JANET_STRING, NULL}},
  {"custom-request", {CURLOPT_CUSTOMREQUEST, JANET_STRING, NULL}},
  {"interface", {CURLOPT_INTERFACE, JANET_STRING, NULL}},
  {"krb-level", {CURLOPT_KRBLEVEL, JANET_STRING, NULL}},
  {"ca-info", {CURLOPT_CAINFO, JANET_STRING, NULL}},
  {"random-file", {CURLOPT_RANDOM_FILE, JANET_STRING, NULL}},
  {"egd-socket", {CURLOPT_EGDSOCKET, JANET_STRING, NULL}},
  {"cookie-jar", {CURLOPT_COOKIEJAR, JANET_STRING, NULL}},
  {"ssl-cipher-list", {CURLOPT_SSL_CIPHER_LIST, JANET_STRING, NULL}},
  {"ssl-cert-type", {CURLOPT_SSLCERTTYPE, JANET_STRING, NULL}},
  {"ssl-key", {CURLOPT_SSLKEY, JANET_STRING, NULL}},
  {"ssl-key-type", {CURLOPT_SSLKEYTYPE, JANET_STRING, NULL}},
  {"ssl-engine", {CURLOPT_SSLENGINE, JANET_STRING, NULL}},
  {"ca-path", {CURLOPT_CAPATH, JANET_STRING, NULL}},
  {"accept-encoding", {CURLOPT_ACCEPT_ENCODING, JANET_STRING, NULL}},
  {"net-rc-file", {CURLOPT_NETRC_FILE, JANET_STRING, NULL}},
  {"ftp-account", {CURLOPT_FTP_ACCOUNT, JANET_STRING, NULL}},
  {"cookie-list", {CURLOPT_COOKIELIST, JANET_STRING, NULL}},
  {"ftp-alternative-to-user", {CURLOPT_FTP_ALTERNATIVE_TO_USER, JANET_STRING, NULL}},
  {"ssh-public-key-file", {CURLOPT_SSH_PUBLIC_KEYFILE, JANET_STRING, NULL}},
  {"ssh-private-key-file", {CURLOPT_SSH_PRIVATE_KEYFILE, JANET_STRING, NULL}},
  {"ssh-host-public-key-md5", {CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, JANET_STRING, NULL}},
  {"crl-file", {CURLOPT_CRLFILE, JANET_STRING, NULL}},
  {"issuer-cert", {CURLOPT_ISSUERCERT, JANET_STRING, NULL}},
  {"username", {CURLOPT_USERNAME, JANET_STRING, NULL}},
  {"password", {CURLOPT_PASSWORD, JANET_STRING, NULL}},
  {"proxy-username", {CURLOPT_PROXYUSERNAME, JANET_STRING, NULL}},
  {"proxy-password", {CURLOPT_PROXYPASSWORD, JANET_STRING, NULL}},
  {"no-proxy", {CURLOPT_NOPROXY, JANET_STRING, NULL}},
  {"socks5-gss-api-service", {CURLOPT_SOCKS5_GSSAPI_SERVICE, JANET_STRING, NULL}},
  {"ssh-known-hosts", {CURLOPT_SSH_KNOWNHOSTS, JANET_STRING, NULL}},
  {"mail-from", {CURLOPT_MAIL_FROM, JANET_STRING, NULL}},
  {"rtsp-session-id", {CURLOPT_RTSP_SESSION_ID, JANET_STRING, NULL}},
  {"rtsp-stream-uri", {CURLOPT_RTSP_STREAM_URI, JANET_STRING, NULL}},
  {"rtsp-transport", {CURLOPT_RTSP_TRANSPORT, JANET_STRING, NULL}},
  {"tls-auth-username", {CURLOPT_TLSAUTH_USERNAME, JANET_STRING, NULL}},
  {"tls-auth-password", {CURLOPT_TLSAUTH_PASSWORD, JANET_STRING, NULL}},
  {"tls-auth-type", {CURLOPT_TLSAUTH_TYPE, JANET_STRING, NULL}},
  {"dns-servers", {CURLOPT_DNS_SERVERS, JANET_STRING, NULL}},
  {"mail-auth", {CURLOPT_MAIL_AUTH, JANET_STRING, NULL}},
  {"xoauth2-bearer", {CURLOPT_XOAUTH2_BEARER, JANET_STRING, NULL}},
  {"dns-interface", {CURLOPT_DNS_INTERFACE, JANET_STRING, NULL}},
  {"dns-local-ip4", {CURLOPT_DNS_LOCAL_IP4, JANET_STRING, NULL}},
  {"dns-local-ip6", {CURLOPT_DNS_LOCAL_IP6, JANET_STRING, NULL}},
  {"login-options", {CURLOPT_LOGIN_OPTIONS, JANET_STRING, NULL}},
  {"pinned-public-key", {CURLOPT_PINNEDPUBLICKEY, JANET_STRING, NULL}},
  {"unix-socket-path", {CURLOPT_UNIX_SOCKET_PATH, JANET_STRING, NULL}},
  {"proxy-service-name", {CURLOPT_PROXY_SERVICE_NAME, JANET_STRING, NULL}},
  {"service-name", {CURLOPT_SERVICE_NAME, JANET_STRING, NULL}},
  {"default-protocol", {CURLOPT_DEFAULT_PROTOCOL, JANET_STRING, NULL}},
  {"proxy-ca-info", {CURLOPT_PROXY_CAINFO, JANET_STRING, NULL}},
  {"proxy-ca-path", {CURLOPT_PROXY_CAPATH, JANET_STRING, NULL}},
  {"proxy-tls-auth-username", {CURLOPT_PROXY_TLSAUTH_USERNAME, JANET_STRING, NULL}},
  {"proxy-tls-auth-password", {CURLOPT_PROXY_TLSAUTH_PASSWORD, JANET_STRING, NULL}},
  {"proxy-tls-auth-type", {CURLOPT_PROXY_TLSAUTH_TYPE, JANET_STRING, NULL}},
  {"proxy-ssl-cert", {CURLOPT_PROXY_SSLCERT, JANET_STRING, NULL}},
  {"proxy-ssl-cert-type", {CURLOPT_PROXY_SSLCERTTYPE, JANET_STRING, NULL}},
  {"proxy-ssl-key", {CURLOPT_PROXY_SSLKEY, JANET_STRING, NULL}},
  {"proxy-ssl-key-type", {CURLOPT_PROXY_SSLKEYTYPE, JANET_STRING, NULL}},
  {"proxy-key-password", {CURLOPT_PROXY_KEYPASSWD, JANET_STRING, NULL}},
  {"proxy-ssl-cipher-list", {CURLOPT_PROXY_SSL_CIPHER_LIST, JANET_STRING, NULL}},
  {"proxy-crl-file", {CURLOPT_PROXY_CRLFILE, JANET_STRING, NULL}},
  {"pre-proxy", {CURLOPT_PRE_PROXY, JANET_STRING, NULL}},
  {"proxy-pinned-public-key", {CURLOPT_PROXY_PINNEDPUBLICKEY, JANET_STRING, NULL}},
  {"abstract-unix-socket", {CURLOPT_ABSTRACT_UNIX_SOCKET, JANET_STRING, NULL}},
  {"request-target", {CURLOPT_REQUEST_TARGET, JANET_STRING, NULL}},
  {"tls13-ciphers", {CURLOPT_TLS13_CIPHERS, JANET_STRING, NULL}},
  {"proxy-tls13-ciphers", {CURLOPT_PROXY_TLS13_CIPHERS, JANET_STRING, NULL}},
  {"doh-url", {CURLOPT_DOH_URL, JANET_STRING, NULL}},

  {"verbose?", {CURLOPT_VERBOSE, JANET_BOOLEAN, NULL}},
  {"header?", {CURLOPT_HEADER, JANET_BOOLEAN, NULL}},
  {"no-progress?", {CURLOPT_NOPROGRESS, JANET_BOOLEAN, NULL}},
  {"no-body?", {CURLOPT_NOBODY, JANET_BOOLEAN, NULL}},
  {"fail-on-error?", {CURLOPT_FAILONERROR, JANET_BOOLEAN, NULL}},
  {"upload?", {CURLOPT_UPLOAD, JANET_BOOLEAN, NULL}},
  {"post?", {CURLOPT_POST, JANET_BOOLEAN, NULL}},
  {"dir-list-only?", {CURLOPT_DIRLISTONLY, JANET_BOOLEAN, NULL}},
  {"append?", {CURLOPT_APPEND, JANET_BOOLEAN, NULL}},
  {"follow-location?", {CURLOPT_FOLLOWLOCATION, JANET_BOOLEAN, NULL}},
  {"transfer-text?", {CURLOPT_TRANSFERTEXT, JANET_BOOLEAN, NULL}},
  {"put?", {CURLOPT_PUT, JANET_BOOLEAN, NULL}},
  {"auto-referer?", {CURLOPT_AUTOREFERER, JANET_BOOLEAN, NULL}},
  {"http-proxy-tunnel?", {CURLOPT_HTTPPROXYTUNNEL, JANET_BOOLEAN, NULL}},
  {"ssl-verify-peer?", {CURLOPT_SSL_VERIFYPEER, JANET_BOOLEAN, NULL}},
  {"file-time?", {CURLOPT_FILETIME, JANET_BOOLEAN, NULL}},
  {"fresh-connect?", {CURLOPT_FRESH_CONNECT, JANET_BOOLEAN, NULL}},
  {"forbid-reuse?", {CURLOPT_FORBID_REUSE, JANET_BOOLEAN, NULL}},
  {"http-get?", {CURLOPT_HTTPGET, JANET_BOOLEAN, NULL}},
  {"ftp-use-epsv?", {CURLOPT_FTP_USE_EPSV, JANET_BOOLEAN, NULL}},
  {"ssl-engine-default?", {CURLOPT_SSLENGINE_DEFAULT, JANET_BOOLEAN, NULL}},
  {"dns-use-global-cache?", {CURLOPT_DNS_USE_GLOBAL_CACHE, JANET_BOOLEAN, NULL}},
  {"cookie-session?", {CURLOPT_COOKIESESSION, JANET_BOOLEAN, NULL}},
  {"no-signal?", {CURLOPT_NOSIGNAL, JANET_BOOLEAN, NULL}},
  {"unrestricted-auth?", {CURLOPT_UNRESTRICTED_AUTH, JANET_BOOLEAN, NULL}},
  {"ftp-use-eprt?", {CURLOPT_FTP_USE_EPRT, JANET_BOOLEAN, NULL}},
  {"tcp-no-delay?", {CURLOPT_TCP_NODELAY, JANET_BOOLEAN, NULL}},
  {"ignore-content-length?", {CURLOPT_IGNORE_CONTENT_LENGTH, JANET_BOOLEAN, NULL}},
  {"ftp-skip-pasv-ip?", {CURLOPT_FTP_SKIP_PASV_IP, JANET_BOOLEAN, NULL}},
  {"connect-only?", {CURLOPT_CONNECT_ONLY, JANET_BOOLEAN, NULL}},
  {"ssl-sessionid-cache?", {CURLOPT_SSL_SESSIONID_CACHE, JANET_BOOLEAN, NULL}},
  {"http-transfer-decoding?", {CURLOPT_HTTP_TRANSFER_DECODING, JANET_BOOLEAN, NULL}},
  {"http-content-decoding?", {CURLOPT_HTTP_CONTENT_DECODING, JANET_BOOLEAN, NULL}},
  {"proxy-transfer-mode?", {CURLOPT_PROXY_TRANSFER_MODE, JANET_BOOLEAN, NULL}},
  {"cert-info?", {CURLOPT_CERTINFO, JANET_BOOLEAN, NULL}},
  {"socks5-gss-api-nec?", {CURLOPT_SOCKS5_GSSAPI_NEC, JANET_BOOLEAN, NULL}},
  {"ftp-use-pret?", {CURLOPT_FTP_USE_PRET, JANET_BOOLEAN, NULL}},
  {"wildcard-match?", {CURLOPT_WILDCARDMATCH, JANET_BOOLEAN, NULL}},
  {"transfer-encoding?", {CURLOPT_TRANSFER_ENCODING, JANET_BOOLEAN, NULL}},
  {"sasl-ir?", {CURLOPT_SASL_IR, JANET_BOOLEAN, NULL}},
  {"ssl-enable-npn?", {CURLOPT_SSL_ENABLE_NPN, JANET_BOOLEAN, NULL}},
  {"ssl-enable-alpn?", {CURLOPT_SSL_ENABLE_ALPN, JANET_BOOLEAN, NULL}},
  {"ssl-verify-status?", {CURLOPT_SSL_VERIFYSTATUS, JANET_BOOLEAN, NULL}},
  {"ssl-false-start?", {CURLOPT_SSL_FALSESTART, JANET_BOOLEAN, NULL}},
  {"path-as-is?", {CURLOPT_PATH_AS_IS, JANET_BOOLEAN, NULL}},
  {"pipe-wait?", {CURLOPT_PIPEWAIT, JANET_BOOLEAN, NULL}},
  {"tftp-no-options?", {CURLOPT_TFTP_NO_OPTIONS, JANET_BOOLEAN, NULL}},
  {"tcp-fastopen?", {CURLOPT_TCP_FASTOPEN, JANET_BOOLEAN, NULL}},
  {"keep-sending-on-error?", {CURLOPT_KEEP_SENDING_ON_ERROR, JANET_BOOLEAN, NULL}},
  {"proxy-ssl-verify-peer?", {CURLOPT_PROXY_SSL_VERIFYPEER, JANET_BOOLEAN, NULL}},
  {"suppress-connect-headers?", {CURLOPT_SUPPRESS_CONNECT_HEADERS, JANET_BOOLEAN, NULL}},
  {"ssh-compression?", {CURLOPT_SSH_COMPRESSION, JANET_BOOLEAN, NULL}},
  {"ha-proxy-protocol?", {CURLOPT_HAPROXYPROTOCOL, JANET_BOOLEAN, NULL}},
  {"dns-shuffle-addresses?", {CURLOPT_DNS_SHUFFLE_ADDRESSES, JANET_BOOLEAN, NULL}},
  {"disallow-username-in-url?", {CURLOPT_DISALLOW_USERNAME_IN_URL, JANET_BOOLEAN, NULL}},
  {"http09-allowed?", {CURLOPT_HTTP09_ALLOWED, JANET_BOOLEAN, NULL}},
  {"ssl-verify-host?", {CURLOPT_SSL_VERIFYHOST, JANET_NUMBER, NULL}},

  // ENUMS
  {"proxy-type", {CURLOPT_PROXYTYPE, JANET_NUMBER, NULL}},  
  {"ftp-create-missing-dirs", {CURLOPT_FTP_CREATE_MISSING_DIRS, JANET_NUMBER, NULL}},
  {"ip-resolve", {CURLOPT_IPRESOLVE, JANET_NUMBER, NULL}},
  {"use-ssl", {CURLOPT_USE_SSL, JANET_NUMBER, NULL}},
  {"ftp-ssl-auth", {CURLOPT_FTPSSLAUTH, JANET_NUMBER, NULL}},
  {"ftp-file-method", {CURLOPT_FTP_FILEMETHOD, JANET_NUMBER, NULL}},
  {"ftp-ssl-ccc", {CURLOPT_FTP_SSL_CCC, JANET_NUMBER, NULL}},
  {"rtsp-request", {CURLOPT_RTSP_REQUEST, JANET_NUMBER, NULL}},
  {"gss-api-delegation", {CURLOPT_GSSAPI_DELEGATION, JANET_NUMBER, NULL}},
  {"proxy-ssl-verify-host", {CURLOPT_PROXY_SSL_VERIFYHOST, JANET_NUMBER, NULL}},
  {"proxy-ssl-version", {CURLOPT_PROXY_SSLVERSION, JANET_NUMBER, NULL}},
  {"netrc", {CURLOPT_NETRC, JANET_NUMBER, NULL}},  

  // MASKS    
  {"ssh-auth-types", {CURLOPT_SSH_AUTH_TYPES, JANET_NUMBER, NULL}},
  {"post-redir", {CURLOPT_POSTREDIR, JANET_NUMBER, NULL}},
  {"protocols", {CURLOPT_PROTOCOLS, JANET_NUMBER, NULL}},
  {"redir-protocols", {CURLOPT_REDIR_PROTOCOLS, JANET_NUMBER, NULL}},
  {"ssl-options", {CURLOPT_SSL_OPTIONS, JANET_NUMBER, NULL}},
  {"header-opt", {CURLOPT_HEADEROPT, JANET_NUMBER, NULL}},
  {"proxy-ssl-options", {CURLOPT_PROXY_SSL_OPTIONS, JANET_NUMBER, NULL}},
  {"http-auth", {CURLOPT_HTTPAUTH, JANET_NUMBER, NULL}},
  {"proxy-auth", {CURLOPT_PROXYAUTH, JANET_NUMBER, NULL}},  
  {"socks5-auth", {CURLOPT_SOCKS5_AUTH, JANET_NUMBER, NULL}},

  {"port", {CURLOPT_PORT, JANET_NUMBER, NULL}},
  {"timeout", {CURLOPT_TIMEOUT, JANET_NUMBER, NULL}},
  {"infilesize", {CURLOPT_INFILESIZE, JANET_NUMBER, NULL}},
  {"low-speed-limit", {CURLOPT_LOW_SPEED_LIMIT, JANET_NUMBER, NULL}},
  {"low-speed-time", {CURLOPT_LOW_SPEED_TIME, JANET_NUMBER, NULL}},
  {"resume-from", {CURLOPT_RESUME_FROM, JANET_NUMBER, NULL}},
  {"crlf", {CURLOPT_CRLF, JANET_NUMBER, NULL}},
  {"ssl-version", {CURLOPT_SSLVERSION, JANET_NUMBER, NULL}},
  {"time-condition", {CURLOPT_TIMECONDITION, JANET_NUMBER, NULL}},
  {"time-value", {CURLOPT_TIMEVALUE, JANET_NUMBER, NULL}},
  {"proxy-port", {CURLOPT_PROXYPORT, JANET_NUMBER, NULL}},
  {"post-field-size", {CURLOPT_POSTFIELDSIZE, JANET_NUMBER, NULL}},
  {"max-redirs", {CURLOPT_MAXREDIRS, JANET_NUMBER, NULL}},
  {"max-connects", {CURLOPT_MAXCONNECTS, JANET_NUMBER, NULL}},
  {"obsolete72", {CURLOPT_OBSOLETE72, JANET_NUMBER, NULL}},
  {"connect-timeout", {CURLOPT_CONNECTTIMEOUT, JANET_NUMBER, NULL}},
  {"http-version", {CURLOPT_HTTP_VERSION, JANET_NUMBER, NULL}},
  {"dns-cache-timeout", {CURLOPT_DNS_CACHE_TIMEOUT, JANET_NUMBER, NULL}},
  {"buffer-size", {CURLOPT_BUFFERSIZE, JANET_NUMBER, NULL}},
  {"ftp-response-timeout", {CURLOPT_FTP_RESPONSE_TIMEOUT, JANET_NUMBER, NULL}},
  {"max-filesize", {CURLOPT_MAXFILESIZE, JANET_NUMBER, NULL}},
  {"local-port", {CURLOPT_LOCALPORT, JANET_NUMBER, NULL}},
  {"local-portrange", {CURLOPT_LOCALPORTRANGE, JANET_NUMBER, NULL}},
  {"timeout-ms", {CURLOPT_TIMEOUT_MS, JANET_NUMBER, NULL}},
  {"connect-timeout-ms", {CURLOPT_CONNECTTIMEOUT_MS, JANET_NUMBER, NULL}},
  {"new-file-perms", {CURLOPT_NEW_FILE_PERMS, JANET_NUMBER, NULL}},
  {"new-directory-perms", {CURLOPT_NEW_DIRECTORY_PERMS, JANET_NUMBER, NULL}},
  {"address-scope", {CURLOPT_ADDRESS_SCOPE, JANET_NUMBER, NULL}},
  {"tftp-blk-size", {CURLOPT_TFTP_BLKSIZE, JANET_NUMBER, NULL}},
  {"rtsp-client-cseq", {CURLOPT_RTSP_CLIENT_CSEQ, JANET_NUMBER, NULL}},
  {"rtsp-server-cseq", {CURLOPT_RTSP_SERVER_CSEQ, JANET_NUMBER, NULL}},
  {"accept-timeout-ms", {CURLOPT_ACCEPTTIMEOUT_MS, JANET_NUMBER, NULL}},
  {"tcp-keep-alive", {CURLOPT_TCP_KEEPALIVE, JANET_NUMBER, NULL}},
  {"tcp-keep-idle", {CURLOPT_TCP_KEEPIDLE, JANET_NUMBER, NULL}},
  {"tcp-keep-intvl", {CURLOPT_TCP_KEEPINTVL, JANET_NUMBER, NULL}},
  {"expect-100-timeout-ms", {CURLOPT_EXPECT_100_TIMEOUT_MS, JANET_NUMBER, NULL}},
  {"stream-weight", {CURLOPT_STREAM_WEIGHT, JANET_NUMBER, NULL}},
  {"happy-eyeballs-timeout-ms", {CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, JANET_NUMBER, NULL}},
  {"upload-buffer-size", {CURLOPT_UPLOAD_BUFFERSIZE, JANET_NUMBER, NULL}},
  {"upkeep-interval-ms", {CURLOPT_UPKEEP_INTERVAL_MS, JANET_NUMBER, NULL}},

  {"in-file-size-large", {CURLOPT_INFILESIZE_LARGE, JANET_NUMBER, NULL}},
  {"resume-from-large", {CURLOPT_RESUME_FROM_LARGE, JANET_NUMBER, NULL}},
  {"max-file-size-large", {CURLOPT_MAXFILESIZE_LARGE, JANET_NUMBER, NULL}},
  {"post-field-size-large", {CURLOPT_POSTFIELDSIZE_LARGE, JANET_NUMBER, NULL}},
  {"max-send-speed-large", {CURLOPT_MAX_SEND_SPEED_LARGE, JANET_NUMBER, NULL}},
  {"max-recv-speed-large", {CURLOPT_MAX_RECV_SPEED_LARGE, JANET_NUMBER, NULL}},
  {"time-value-large", {CURLOPT_TIMEVALUE_LARGE, JANET_NUMBER, NULL}},

  {"write-function", {CURLOPT_WRITEFUNCTION, JANET_CFUNCTION, NULL}},
  {"read-function", {CURLOPT_READFUNCTION, JANET_CFUNCTION, NULL}},
  {"progress-function", {CURLOPT_PROGRESSFUNCTION, JANET_CFUNCTION, NULL}},
  {"header-function", {CURLOPT_HEADERFUNCTION, JANET_CFUNCTION, NULL}},
  {"debug-function", {CURLOPT_DEBUGFUNCTION, JANET_CFUNCTION, NULL}},
  {"ssl-ctx-function", {CURLOPT_SSL_CTX_FUNCTION, JANET_CFUNCTION, NULL}},
  {"ioctl-function", {CURLOPT_IOCTLFUNCTION, JANET_CFUNCTION, NULL}},
  {"conv-from-network-function", {CURLOPT_CONV_FROM_NETWORK_FUNCTION, JANET_CFUNCTION, NULL}},
  {"conv-to-network-function", {CURLOPT_CONV_TO_NETWORK_FUNCTION, JANET_CFUNCTION, NULL}},
  {"conv-from-utf8-function", {CURLOPT_CONV_FROM_UTF8_FUNCTION, JANET_CFUNCTION, NULL}},
  {"sockopt-function", {CURLOPT_SOCKOPTFUNCTION, JANET_CFUNCTION, NULL}},
  {"open-socket-function", {CURLOPT_OPENSOCKETFUNCTION, JANET_CFUNCTION, NULL}},
  {"seek-function", {CURLOPT_SEEKFUNCTION, JANET_CFUNCTION, NULL}},
  {"ssh-key-function", {CURLOPT_SSH_KEYFUNCTION, JANET_CFUNCTION, NULL}},
  {"interleave-function", {CURLOPT_INTERLEAVEFUNCTION, JANET_CFUNCTION, NULL}},
  {"chunk-bgn-function", {CURLOPT_CHUNK_BGN_FUNCTION, JANET_CFUNCTION, NULL}},
  {"chunk-end-function", {CURLOPT_CHUNK_END_FUNCTION, JANET_CFUNCTION, NULL}},
  {"fnmatch-function", {CURLOPT_FNMATCH_FUNCTION, JANET_CFUNCTION, NULL}},
  {"close-socket-function", {CURLOPT_CLOSESOCKETFUNCTION, JANET_CFUNCTION, NULL}},
  {"xfer-info-function", {CURLOPT_XFERINFOFUNCTION, JANET_CFUNCTION, NULL}},
  {"resolver-start-function", {CURLOPT_RESOLVER_START_FUNCTION, JANET_CFUNCTION, NULL}},
  {"trailer-function", {CURLOPT_TRAILERFUNCTION, JANET_CFUNCTION, NULL}},

  {"write-data", {CURLOPT_WRITEDATA, JANET_POINTER, NULL}},
  {"read-data", {CURLOPT_READDATA, JANET_POINTER, NULL}},
  {"error-buffer", {CURLOPT_ERRORBUFFER, JANET_POINTER, NULL}},
  {"post-fields", {CURLOPT_POSTFIELDS, JANET_POINTER, NULL}},
  {"http-header", {CURLOPT_HTTPHEADER, JANET_POINTER, NULL}},
  {"http-post", {CURLOPT_HTTPPOST, JANET_POINTER, NULL}},
  {"quote", {CURLOPT_QUOTE, JANET_POINTER, NULL}},
  {"header-data", {CURLOPT_HEADERDATA, JANET_POINTER, NULL}},
  {"stderr", {CURLOPT_STDERR, JANET_POINTER, NULL}},
  {"post-quote", {CURLOPT_POSTQUOTE, JANET_POINTER, NULL}},
  {"obsolete40", {CURLOPT_OBSOLETE40, JANET_POINTER, NULL}},
  {"progress-data", {CURLOPT_PROGRESSDATA, JANET_POINTER, NULL}},
  {"telnet-options", {CURLOPT_TELNETOPTIONS, JANET_POINTER, NULL}},
  {"prequote", {CURLOPT_PREQUOTE, JANET_POINTER, NULL}},
  {"debug-data", {CURLOPT_DEBUGDATA, JANET_POINTER, NULL}},
  {"share", {CURLOPT_SHARE, JANET_POINTER, NULL}},
  {"private", {CURLOPT_PRIVATE, JANET_POINTER, NULL}},
  {"http200-aliases", {CURLOPT_HTTP200ALIASES, JANET_POINTER, NULL}},
  {"ssl-ctx-data", {CURLOPT_SSL_CTX_DATA, JANET_POINTER, NULL}},
  {"ioctl-data", {CURLOPT_IOCTLDATA, JANET_POINTER, NULL}},
  {"sockopt-data", {CURLOPT_SOCKOPTDATA, JANET_POINTER, NULL}},
  {"open-socke-tdata", {CURLOPT_OPENSOCKETDATA, JANET_POINTER, NULL}},
  {"copy-post-fields", {CURLOPT_COPYPOSTFIELDS, JANET_POINTER, NULL}},
  {"seek-data", {CURLOPT_SEEKDATA, JANET_POINTER, NULL}},
  {"ssh-key-data", {CURLOPT_SSH_KEYDATA, JANET_POINTER, NULL}},
  {"mail-rcpt", {CURLOPT_MAIL_RCPT, JANET_POINTER, NULL}},
  {"interleave-data", {CURLOPT_INTERLEAVEDATA, JANET_POINTER, NULL}},
  {"chunk-data", {CURLOPT_CHUNK_DATA, JANET_POINTER, NULL}},
  {"fnmatch-data", {CURLOPT_FNMATCH_DATA, JANET_POINTER, NULL}},
  {"resolve", {CURLOPT_RESOLVE, JANET_POINTER, NULL}},
  {"close-socket-data", {CURLOPT_CLOSESOCKETDATA, JANET_POINTER, NULL}},
  {"proxy-header", {CURLOPT_PROXYHEADER, JANET_POINTER, NULL}},
  {"stream-depends", {CURLOPT_STREAM_DEPENDS, JANET_POINTER, NULL}},
  {"stream-depends-e", {CURLOPT_STREAM_DEPENDS_E, JANET_POINTER, NULL}},
  {"connect-to", {CURLOPT_CONNECT_TO, JANET_POINTER, NULL}},
  {"mimepost", {CURLOPT_MIMEPOST, JANET_POINTER, NULL}},
  {"resolver-start-data", {CURLOPT_RESOLVER_START_DATA, JANET_POINTER, NULL}},
  {"curlu", {CURLOPT_CURLU, JANET_POINTER, NULL}},
  {"trailer-data", {CURLOPT_TRAILERDATA, JANET_POINTER, NULL}},
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
    case JANET_CFUNCTION:
      janet_panic("not implemented yet");
      // TODO:      
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