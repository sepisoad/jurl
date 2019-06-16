#include "curl.h"
#include "khash.h"

typedef struct _curl_janet_map _curl_janet_map_t;

struct _curl_janet_map {
  int copt;
  JanetType jtype;
};

KHASH_MAP_INIT_STR(_curl_janet_hashmap_t, _curl_janet_map_t*);
khash_t(_curl_janet_hashmap_t) *_opt_hash_map = NULL;

// $ => enum; 
// ? => bool; 
// ! => mask
static struct  {
  char name[128];
  _curl_janet_map_t kjm;  
} _key_opt_type_arr[] = {  
  {"url", {CURLOPT_URL, JANET_STRING}},
  {"proxy", {CURLOPT_PROXY, JANET_STRING}},
  {"userpwd", {CURLOPT_USERPWD, JANET_STRING}},
  {"proxyuserpwd", {CURLOPT_PROXYUSERPWD, JANET_STRING}},
  {"range", {CURLOPT_RANGE, JANET_STRING}},
  {"referer", {CURLOPT_REFERER, JANET_STRING}},
  {"ftpport", {CURLOPT_FTPPORT, JANET_STRING}},
  {"useragent", {CURLOPT_USERAGENT, JANET_STRING}},
  {"cookie", {CURLOPT_COOKIE, JANET_STRING}},
  {"sslcert", {CURLOPT_SSLCERT, JANET_STRING}},
  {"keypasswd", {CURLOPT_KEYPASSWD, JANET_STRING}},
  {"cookiefile", {CURLOPT_COOKIEFILE, JANET_STRING}},
  {"customrequest", {CURLOPT_CUSTOMREQUEST, JANET_STRING}},
  {"interface", {CURLOPT_INTERFACE, JANET_STRING}},
  {"krblevel", {CURLOPT_KRBLEVEL, JANET_STRING}},
  {"cainfo", {CURLOPT_CAINFO, JANET_STRING}},
  {"random-file", {CURLOPT_RANDOM_FILE, JANET_STRING}},
  {"egdsocket", {CURLOPT_EGDSOCKET, JANET_STRING}},
  {"cookiejar", {CURLOPT_COOKIEJAR, JANET_STRING}},
  {"ssl-cipher-list", {CURLOPT_SSL_CIPHER_LIST, JANET_STRING}},
  {"sslcerttype", {CURLOPT_SSLCERTTYPE, JANET_STRING}},
  {"sslkey", {CURLOPT_SSLKEY, JANET_STRING}},
  {"sslkeytype", {CURLOPT_SSLKEYTYPE, JANET_STRING}},
  {"sslengine", {CURLOPT_SSLENGINE, JANET_STRING}},
  {"capath", {CURLOPT_CAPATH, JANET_STRING}},
  {"accept-encoding", {CURLOPT_ACCEPT_ENCODING, JANET_STRING}},
  {"netrc-file", {CURLOPT_NETRC_FILE, JANET_STRING}},
  {"ftp-account", {CURLOPT_FTP_ACCOUNT, JANET_STRING}},
  {"cookielist", {CURLOPT_COOKIELIST, JANET_STRING}},
  {"ftp-alternative-to-user", {CURLOPT_FTP_ALTERNATIVE_TO_USER, JANET_STRING}},
  {"ssh-public-keyfile", {CURLOPT_SSH_PUBLIC_KEYFILE, JANET_STRING}},
  {"ssh-private-keyfile", {CURLOPT_SSH_PRIVATE_KEYFILE, JANET_STRING}},
  {"ssh-host-public-key-md5", {CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, JANET_STRING}},
  {"crlfile", {CURLOPT_CRLFILE, JANET_STRING}},
  {"issuercert", {CURLOPT_ISSUERCERT, JANET_STRING}},
  {"username", {CURLOPT_USERNAME, JANET_STRING}},
  {"password", {CURLOPT_PASSWORD, JANET_STRING}},
  {"proxyusername", {CURLOPT_PROXYUSERNAME, JANET_STRING}},
  {"proxypassword", {CURLOPT_PROXYPASSWORD, JANET_STRING}},
  {"noproxy", {CURLOPT_NOPROXY, JANET_STRING}},
  {"socks5-gssapi-service", {CURLOPT_SOCKS5_GSSAPI_SERVICE, JANET_STRING}},
  {"ssh-knownhosts", {CURLOPT_SSH_KNOWNHOSTS, JANET_STRING}},
  {"mail-from", {CURLOPT_MAIL_FROM, JANET_STRING}},
  {"rtsp-session-id", {CURLOPT_RTSP_SESSION_ID, JANET_STRING}},
  {"rtsp-stream-uri", {CURLOPT_RTSP_STREAM_URI, JANET_STRING}},
  {"rtsp-transport", {CURLOPT_RTSP_TRANSPORT, JANET_STRING}},
  {"tlsauth-username", {CURLOPT_TLSAUTH_USERNAME, JANET_STRING}},
  {"tlsauth-password", {CURLOPT_TLSAUTH_PASSWORD, JANET_STRING}},
  {"tlsauth-type", {CURLOPT_TLSAUTH_TYPE, JANET_STRING}},
  {"dns-servers", {CURLOPT_DNS_SERVERS, JANET_STRING}},
  {"mail-auth", {CURLOPT_MAIL_AUTH, JANET_STRING}},
  {"xoauth2-bearer", {CURLOPT_XOAUTH2_BEARER, JANET_STRING}},
  {"dns-interface", {CURLOPT_DNS_INTERFACE, JANET_STRING}},
  {"dns-local-ip4", {CURLOPT_DNS_LOCAL_IP4, JANET_STRING}},
  {"dns-local-ip6", {CURLOPT_DNS_LOCAL_IP6, JANET_STRING}},
  {"login-options", {CURLOPT_LOGIN_OPTIONS, JANET_STRING}},
  {"pinnedpublickey", {CURLOPT_PINNEDPUBLICKEY, JANET_STRING}},
  {"unix-socket-path", {CURLOPT_UNIX_SOCKET_PATH, JANET_STRING}},
  {"proxy-service-name", {CURLOPT_PROXY_SERVICE_NAME, JANET_STRING}},
  {"service-name", {CURLOPT_SERVICE_NAME, JANET_STRING}},
  {"default-protocol", {CURLOPT_DEFAULT_PROTOCOL, JANET_STRING}},
  {"proxy-cainfo", {CURLOPT_PROXY_CAINFO, JANET_STRING}},
  {"proxy-capath", {CURLOPT_PROXY_CAPATH, JANET_STRING}},
  {"proxy-tlsauth-username", {CURLOPT_PROXY_TLSAUTH_USERNAME, JANET_STRING}},
  {"proxy-tlsauth-password", {CURLOPT_PROXY_TLSAUTH_PASSWORD, JANET_STRING}},
  {"proxy-tlsauth-type", {CURLOPT_PROXY_TLSAUTH_TYPE, JANET_STRING}},
  {"proxy-sslcert", {CURLOPT_PROXY_SSLCERT, JANET_STRING}},
  {"proxy-sslcerttype", {CURLOPT_PROXY_SSLCERTTYPE, JANET_STRING}},
  {"proxy-sslkey", {CURLOPT_PROXY_SSLKEY, JANET_STRING}},
  {"proxy-sslkeytype", {CURLOPT_PROXY_SSLKEYTYPE, JANET_STRING}},
  {"proxy-keypasswd", {CURLOPT_PROXY_KEYPASSWD, JANET_STRING}},
  {"proxy-ssl-cipher-list", {CURLOPT_PROXY_SSL_CIPHER_LIST, JANET_STRING}},
  {"proxy-crlfile", {CURLOPT_PROXY_CRLFILE, JANET_STRING}},
  {"pre-proxy", {CURLOPT_PRE_PROXY, JANET_STRING}},
  {"proxy-pinnedpublickey", {CURLOPT_PROXY_PINNEDPUBLICKEY, JANET_STRING}},
  {"abstract-unix-socket", {CURLOPT_ABSTRACT_UNIX_SOCKET, JANET_STRING}},
  {"request-target", {CURLOPT_REQUEST_TARGET, JANET_STRING}},
  {"tls13-ciphers", {CURLOPT_TLS13_CIPHERS, JANET_STRING}},
  {"proxy-tls13-ciphers", {CURLOPT_PROXY_TLS13_CIPHERS, JANET_STRING}},
  {"doh-url", {CURLOPT_DOH_URL, JANET_STRING}},

  {"verbose?", {CURLOPT_VERBOSE, JANET_BOOLEAN}},
  {"header?", {CURLOPT_HEADER, JANET_BOOLEAN}},
  {"noprogress?", {CURLOPT_NOPROGRESS, JANET_BOOLEAN}},
  {"nobody?", {CURLOPT_NOBODY, JANET_BOOLEAN}},
  {"failonerror?", {CURLOPT_FAILONERROR, JANET_BOOLEAN}},
  {"upload?", {CURLOPT_UPLOAD, JANET_BOOLEAN}},
  {"post?", {CURLOPT_POST, JANET_BOOLEAN}},
  {"dirlistonly?", {CURLOPT_DIRLISTONLY, JANET_BOOLEAN}},
  {"append?", {CURLOPT_APPEND, JANET_BOOLEAN}},
  {"followlocation?", {CURLOPT_FOLLOWLOCATION, JANET_BOOLEAN}},
  {"transfertext?", {CURLOPT_TRANSFERTEXT, JANET_BOOLEAN}},
  {"put?", {CURLOPT_PUT, JANET_BOOLEAN}},
  {"autoreferer?", {CURLOPT_AUTOREFERER, JANET_BOOLEAN}},
  {"httpproxytunnel?", {CURLOPT_HTTPPROXYTUNNEL, JANET_BOOLEAN}},
  {"ssl-verifypeer?", {CURLOPT_SSL_VERIFYPEER, JANET_BOOLEAN}},
  {"filetime?", {CURLOPT_FILETIME, JANET_BOOLEAN}},
  {"fresh-connect?", {CURLOPT_FRESH_CONNECT, JANET_BOOLEAN}},
  {"forbid-reuse?", {CURLOPT_FORBID_REUSE, JANET_BOOLEAN}},
  {"httpget?", {CURLOPT_HTTPGET, JANET_BOOLEAN}},
  {"ftp-use-epsv?", {CURLOPT_FTP_USE_EPSV, JANET_BOOLEAN}},
  {"sslengine-default?", {CURLOPT_SSLENGINE_DEFAULT, JANET_BOOLEAN}},
  {"dns-use-global-cache?", {CURLOPT_DNS_USE_GLOBAL_CACHE, JANET_BOOLEAN}},
  {"cookiesession?", {CURLOPT_COOKIESESSION, JANET_BOOLEAN}},
  {"nosignal?", {CURLOPT_NOSIGNAL, JANET_BOOLEAN}},
  {"unrestricted-auth?", {CURLOPT_UNRESTRICTED_AUTH, JANET_BOOLEAN}},
  {"ftp-use-eprt?", {CURLOPT_FTP_USE_EPRT, JANET_BOOLEAN}},
  {"tcp-nodelay?", {CURLOPT_TCP_NODELAY, JANET_BOOLEAN}},
  {"ignore-content-length?", {CURLOPT_IGNORE_CONTENT_LENGTH, JANET_BOOLEAN}},
  {"ftp-skip-pasv-ip?", {CURLOPT_FTP_SKIP_PASV_IP, JANET_BOOLEAN}},
  {"connect-only?", {CURLOPT_CONNECT_ONLY, JANET_BOOLEAN}},
  {"ssl-sessionid-cache?", {CURLOPT_SSL_SESSIONID_CACHE, JANET_BOOLEAN}},
  {"http-transfer-decoding?", {CURLOPT_HTTP_TRANSFER_DECODING, JANET_BOOLEAN}},
  {"http-content-decoding?", {CURLOPT_HTTP_CONTENT_DECODING, JANET_BOOLEAN}},
  {"proxy-transfer-mode?", {CURLOPT_PROXY_TRANSFER_MODE, JANET_BOOLEAN}},
  {"certinfo?", {CURLOPT_CERTINFO, JANET_BOOLEAN}},
  {"socks5-gssapi-nec?", {CURLOPT_SOCKS5_GSSAPI_NEC, JANET_BOOLEAN}},
  {"ftp-use-pret?", {CURLOPT_FTP_USE_PRET, JANET_BOOLEAN}},
  {"wildcardmatch?", {CURLOPT_WILDCARDMATCH, JANET_BOOLEAN}},
  {"transfer-encoding?", {CURLOPT_TRANSFER_ENCODING, JANET_BOOLEAN}},
  {"sasl-ir?", {CURLOPT_SASL_IR, JANET_BOOLEAN}},
  {"ssl-enable-npn?", {CURLOPT_SSL_ENABLE_NPN, JANET_BOOLEAN}},
  {"ssl-enable-alpn?", {CURLOPT_SSL_ENABLE_ALPN, JANET_BOOLEAN}},
  {"ssl-verifystatus?", {CURLOPT_SSL_VERIFYSTATUS, JANET_BOOLEAN}},
  {"ssl-falsestart?", {CURLOPT_SSL_FALSESTART, JANET_BOOLEAN}},
  {"path-as-is?", {CURLOPT_PATH_AS_IS, JANET_BOOLEAN}},
  {"pipewait?", {CURLOPT_PIPEWAIT, JANET_BOOLEAN}},
  {"tftp-no-options?", {CURLOPT_TFTP_NO_OPTIONS, JANET_BOOLEAN}},
  {"tcp-fastopen?", {CURLOPT_TCP_FASTOPEN, JANET_BOOLEAN}},
  {"keep-sending-on-error?", {CURLOPT_KEEP_SENDING_ON_ERROR, JANET_BOOLEAN}},
  {"proxy-ssl-verifypeer?", {CURLOPT_PROXY_SSL_VERIFYPEER, JANET_BOOLEAN}},
  {"suppress-connect-headers?", {CURLOPT_SUPPRESS_CONNECT_HEADERS, JANET_BOOLEAN}},
  {"ssh-compression?", {CURLOPT_SSH_COMPRESSION, JANET_BOOLEAN}},
  {"haproxyprotocol?", {CURLOPT_HAPROXYPROTOCOL, JANET_BOOLEAN}},
  {"dns-shuffle-addresses?", {CURLOPT_DNS_SHUFFLE_ADDRESSES, JANET_BOOLEAN}},
  {"disallow-username-in-url?", {CURLOPT_DISALLOW_USERNAME_IN_URL, JANET_BOOLEAN}},
  {"http09-allowed?", {CURLOPT_HTTP09_ALLOWED, JANET_BOOLEAN}},

  {"netrc!", {CURLOPT_NETRC, JANET_NUMBER}},
  {"proxyauth!", {CURLOPT_PROXYAUTH, JANET_NUMBER}},
  {"ssh-auth-types!", {CURLOPT_SSH_AUTH_TYPES, JANET_NUMBER}},
  {"postredir!", {CURLOPT_POSTREDIR, JANET_NUMBER}},
  {"protocols!", {CURLOPT_PROTOCOLS, JANET_NUMBER}},
  {"redir-protocols!", {CURLOPT_REDIR_PROTOCOLS, JANET_NUMBER}},
  {"ssl-options!", {CURLOPT_SSL_OPTIONS, JANET_NUMBER}},
  {"headeropt!", {CURLOPT_HEADEROPT, JANET_NUMBER}},
  {"proxy-ssl-options!", {CURLOPT_PROXY_SSL_OPTIONS, JANET_NUMBER}},
  {"socks5-auth!", {CURLOPT_SOCKS5_AUTH, JANET_NUMBER}},

  {"ssl-verifyhost$", {CURLOPT_SSL_VERIFYHOST, JANET_NUMBER}},
  {"proxytype$", {CURLOPT_PROXYTYPE, JANET_NUMBER}},
  {"httpauth$", {CURLOPT_HTTPAUTH, JANET_NUMBER}},
  {"ftp-create-missing-dirs$", {CURLOPT_FTP_CREATE_MISSING_DIRS, JANET_NUMBER}},
  {"ipresolve$", {CURLOPT_IPRESOLVE, JANET_NUMBER}},
  {"use-ssl$", {CURLOPT_USE_SSL, JANET_NUMBER}},
  {"ftpsslauth$", {CURLOPT_FTPSSLAUTH, JANET_NUMBER}},
  {"ftp-filemethod$", {CURLOPT_FTP_FILEMETHOD, JANET_NUMBER}},
  {"ftp-ssl-ccc$", {CURLOPT_FTP_SSL_CCC, JANET_NUMBER}},
  {"rtsp-request$", {CURLOPT_RTSP_REQUEST, JANET_NUMBER}},
  {"gssapi-delegation$", {CURLOPT_GSSAPI_DELEGATION, JANET_NUMBER}},
  {"proxy-ssl-verifyhost$", {CURLOPT_PROXY_SSL_VERIFYHOST, JANET_NUMBER}},
  {"proxy-sslversion$", {CURLOPT_PROXY_SSLVERSION, JANET_NUMBER}},

  {"port", {CURLOPT_PORT, JANET_NUMBER}},
  {"timeout", {CURLOPT_TIMEOUT, JANET_NUMBER}},
  {"infilesize", {CURLOPT_INFILESIZE, JANET_NUMBER}},
  {"low-speed-limit", {CURLOPT_LOW_SPEED_LIMIT, JANET_NUMBER}},
  {"low-speed-time", {CURLOPT_LOW_SPEED_TIME, JANET_NUMBER}},
  {"resume-from", {CURLOPT_RESUME_FROM, JANET_NUMBER}},
  {"crlf", {CURLOPT_CRLF, JANET_NUMBER}},
  {"sslversion", {CURLOPT_SSLVERSION, JANET_NUMBER}},
  {"timecondition", {CURLOPT_TIMECONDITION, JANET_NUMBER}},
  {"timevalue", {CURLOPT_TIMEVALUE, JANET_NUMBER}},
  {"proxyport", {CURLOPT_PROXYPORT, JANET_NUMBER}},
  {"postfieldsize", {CURLOPT_POSTFIELDSIZE, JANET_NUMBER}},
  {"maxredirs", {CURLOPT_MAXREDIRS, JANET_NUMBER}},
  {"maxconnects", {CURLOPT_MAXCONNECTS, JANET_NUMBER}},
  {"obsolete72", {CURLOPT_OBSOLETE72, JANET_NUMBER}},
  {"connecttimeout", {CURLOPT_CONNECTTIMEOUT, JANET_NUMBER}},
  {"http-version", {CURLOPT_HTTP_VERSION, JANET_NUMBER}},
  {"dns-cache-timeout", {CURLOPT_DNS_CACHE_TIMEOUT, JANET_NUMBER}},
  {"buffersize", {CURLOPT_BUFFERSIZE, JANET_NUMBER}},
  {"ftp-response-timeout", {CURLOPT_FTP_RESPONSE_TIMEOUT, JANET_NUMBER}},
  {"maxfilesize", {CURLOPT_MAXFILESIZE, JANET_NUMBER}},
  {"localport", {CURLOPT_LOCALPORT, JANET_NUMBER}},
  {"localportrange", {CURLOPT_LOCALPORTRANGE, JANET_NUMBER}},
  {"timeout-ms", {CURLOPT_TIMEOUT_MS, JANET_NUMBER}},
  {"connecttimeout-ms", {CURLOPT_CONNECTTIMEOUT_MS, JANET_NUMBER}},
  {"new-file-perms", {CURLOPT_NEW_FILE_PERMS, JANET_NUMBER}},
  {"new-directory-perms", {CURLOPT_NEW_DIRECTORY_PERMS, JANET_NUMBER}},
  {"address-scope", {CURLOPT_ADDRESS_SCOPE, JANET_NUMBER}},
  {"tftp-blksize", {CURLOPT_TFTP_BLKSIZE, JANET_NUMBER}},
  {"rtsp-client-cseq", {CURLOPT_RTSP_CLIENT_CSEQ, JANET_NUMBER}},
  {"rtsp-server-cseq", {CURLOPT_RTSP_SERVER_CSEQ, JANET_NUMBER}},
  {"accepttimeout-ms", {CURLOPT_ACCEPTTIMEOUT_MS, JANET_NUMBER}},
  {"tcp-keepalive", {CURLOPT_TCP_KEEPALIVE, JANET_NUMBER}},
  {"tcp-keepidle", {CURLOPT_TCP_KEEPIDLE, JANET_NUMBER}},
  {"tcp-keepintvl", {CURLOPT_TCP_KEEPINTVL, JANET_NUMBER}},
  {"expect-100-timeout-ms", {CURLOPT_EXPECT_100_TIMEOUT_MS, JANET_NUMBER}},
  {"stream-weight", {CURLOPT_STREAM_WEIGHT, JANET_NUMBER}},
  {"happy-eyeballs-timeout-ms", {CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, JANET_NUMBER}},
  {"upload-buffersize", {CURLOPT_UPLOAD_BUFFERSIZE, JANET_NUMBER}},
  {"upkeep-interval-ms", {CURLOPT_UPKEEP_INTERVAL_MS, JANET_NUMBER}},

  {"infilesize-large", {CURLOPT_INFILESIZE_LARGE, JANET_NUMBER}},
  {"resume-from-large", {CURLOPT_RESUME_FROM_LARGE, JANET_NUMBER}},
  {"maxfilesize-large", {CURLOPT_MAXFILESIZE_LARGE, JANET_NUMBER}},
  {"postfieldsize-large", {CURLOPT_POSTFIELDSIZE_LARGE, JANET_NUMBER}},
  {"max-send-speed-large", {CURLOPT_MAX_SEND_SPEED_LARGE, JANET_NUMBER}},
  {"max-recv-speed-large", {CURLOPT_MAX_RECV_SPEED_LARGE, JANET_NUMBER}},
  {"timevalue-large", {CURLOPT_TIMEVALUE_LARGE, JANET_NUMBER}},

  {"writefunction", {CURLOPT_WRITEFUNCTION, JANET_CFUNCTION}},
  {"readfunction", {CURLOPT_READFUNCTION, JANET_CFUNCTION}},
  {"progressfunction", {CURLOPT_PROGRESSFUNCTION, JANET_CFUNCTION}},
  {"headerfunction", {CURLOPT_HEADERFUNCTION, JANET_CFUNCTION}},
  {"debugfunction", {CURLOPT_DEBUGFUNCTION, JANET_CFUNCTION}},
  {"ssl-ctx-function", {CURLOPT_SSL_CTX_FUNCTION, JANET_CFUNCTION}},
  {"ioctlfunction", {CURLOPT_IOCTLFUNCTION, JANET_CFUNCTION}},
  {"conv-from-network-function", {CURLOPT_CONV_FROM_NETWORK_FUNCTION, JANET_CFUNCTION}},
  {"conv-to-network-function", {CURLOPT_CONV_TO_NETWORK_FUNCTION, JANET_CFUNCTION}},
  {"conv-from-utf8-function", {CURLOPT_CONV_FROM_UTF8_FUNCTION, JANET_CFUNCTION}},
  {"sockoptfunction", {CURLOPT_SOCKOPTFUNCTION, JANET_CFUNCTION}},
  {"opensocketfunction", {CURLOPT_OPENSOCKETFUNCTION, JANET_CFUNCTION}},
  {"seekfunction", {CURLOPT_SEEKFUNCTION, JANET_CFUNCTION}},
  {"ssh-keyfunction", {CURLOPT_SSH_KEYFUNCTION, JANET_CFUNCTION}},
  {"interleavefunction", {CURLOPT_INTERLEAVEFUNCTION, JANET_CFUNCTION}},
  {"chunk-bgn-function", {CURLOPT_CHUNK_BGN_FUNCTION, JANET_CFUNCTION}},
  {"chunk-end-function", {CURLOPT_CHUNK_END_FUNCTION, JANET_CFUNCTION}},
  {"fnmatch-function", {CURLOPT_FNMATCH_FUNCTION, JANET_CFUNCTION}},
  {"closesocketfunction", {CURLOPT_CLOSESOCKETFUNCTION, JANET_CFUNCTION}},
  {"xferinfofunction", {CURLOPT_XFERINFOFUNCTION, JANET_CFUNCTION}},
  {"resolver-start-function", {CURLOPT_RESOLVER_START_FUNCTION, JANET_CFUNCTION}},
  {"trailerfunction", {CURLOPT_TRAILERFUNCTION, JANET_CFUNCTION}},

  {"writedata", {CURLOPT_WRITEDATA, JANET_POINTER}},
  {"readdata", {CURLOPT_READDATA, JANET_POINTER}},
  {"errorbuffer", {CURLOPT_ERRORBUFFER, JANET_POINTER}},
  {"postfields", {CURLOPT_POSTFIELDS, JANET_POINTER}},
  {"httpheader", {CURLOPT_HTTPHEADER, JANET_POINTER}},
  {"httppost", {CURLOPT_HTTPPOST, JANET_POINTER}},
  {"quote", {CURLOPT_QUOTE, JANET_POINTER}},
  {"headerdata", {CURLOPT_HEADERDATA, JANET_POINTER}},
  {"stderr", {CURLOPT_STDERR, JANET_POINTER}},
  {"postquote", {CURLOPT_POSTQUOTE, JANET_POINTER}},
  {"obsolete40", {CURLOPT_OBSOLETE40, JANET_POINTER}},
  {"progressdata", {CURLOPT_PROGRESSDATA, JANET_POINTER}},
  {"telnetoptions", {CURLOPT_TELNETOPTIONS, JANET_POINTER}},
  {"prequote", {CURLOPT_PREQUOTE, JANET_POINTER}},
  {"debugdata", {CURLOPT_DEBUGDATA, JANET_POINTER}},
  {"share", {CURLOPT_SHARE, JANET_POINTER}},
  {"private", {CURLOPT_PRIVATE, JANET_POINTER}},
  {"http200-aliases", {CURLOPT_HTTP200ALIASES, JANET_POINTER}},
  {"ssl-ctx-data", {CURLOPT_SSL_CTX_DATA, JANET_POINTER}},
  {"ioctldata", {CURLOPT_IOCTLDATA, JANET_POINTER}},
  {"sockoptdata", {CURLOPT_SOCKOPTDATA, JANET_POINTER}},
  {"opensocketdata", {CURLOPT_OPENSOCKETDATA, JANET_POINTER}},
  {"copypostfields", {CURLOPT_COPYPOSTFIELDS, JANET_POINTER}},
  {"seekdata", {CURLOPT_SEEKDATA, JANET_POINTER}},
  {"ssh-keydata", {CURLOPT_SSH_KEYDATA, JANET_POINTER}},
  {"mail-rcpt", {CURLOPT_MAIL_RCPT, JANET_POINTER}},
  {"interleavedata", {CURLOPT_INTERLEAVEDATA, JANET_POINTER}},
  {"chunk-data", {CURLOPT_CHUNK_DATA, JANET_POINTER}},
  {"fnmatch-data", {CURLOPT_FNMATCH_DATA, JANET_POINTER}},
  {"resolve", {CURLOPT_RESOLVE, JANET_POINTER}},
  {"closesocketdata", {CURLOPT_CLOSESOCKETDATA, JANET_POINTER}},
  {"proxyheader", {CURLOPT_PROXYHEADER, JANET_POINTER}},
  {"stream-depends", {CURLOPT_STREAM_DEPENDS, JANET_POINTER}},
  {"stream-depends-e", {CURLOPT_STREAM_DEPENDS_E, JANET_POINTER}},
  {"connect-to", {CURLOPT_CONNECT_TO, JANET_POINTER}},
  {"mimepost", {CURLOPT_MIMEPOST, JANET_POINTER}},
  {"resolver-start-data", {CURLOPT_RESOLVER_START_DATA, JANET_POINTER}},
  {"curlu", {CURLOPT_CURLU, JANET_POINTER}},
  {"trailerdata", {CURLOPT_TRAILERDATA, JANET_POINTER}},
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