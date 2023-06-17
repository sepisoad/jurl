#include <janet.h>
#include <curl/curl.h>
#include "khash.h"

//==============================================================================
//  ██████╗ ██████╗ ███╗   ██╗███████╗████████╗ █████╗ ███╗   ██╗████████╗███████╗
// ██╔════╝██╔═══██╗████╗  ██║██╔════╝╚══██╔══╝██╔══██╗████╗  ██║╚══██╔══╝██╔════╝
// ██║     ██║   ██║██╔██╗ ██║███████╗   ██║   ███████║██╔██╗ ██║   ██║   ███████╗
// ██║     ██║   ██║██║╚██╗██║╚════██║   ██║   ██╔══██║██║╚██╗██║   ██║   ╚════██║
// ╚██████╗╚██████╔╝██║ ╚████║███████║   ██║   ██║  ██║██║ ╚████║   ██║   ███████║
//  ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝
//==============================================================================

#define DEFAULT_EASY_RECV_BUF_LEN 2014

//==============================================================================
// ████████╗██╗   ██╗██████╗ ███████╗███████╗
// ╚══██╔══╝╚██╗ ██╔╝██╔══██╗██╔════╝██╔════╝
//    ██║    ╚████╔╝ ██████╔╝█████╗  ███████╗
//    ██║     ╚██╔╝  ██╔═══╝ ██╔══╝  ╚════██║
//    ██║      ██║   ██║     ███████╗███████║
//    ╚═╝      ╚═╝   ╚═╝     ╚══════╝╚══════╝
//==============================================================================

typedef struct MapCurlOptionToJanetType MapCurlOptionToJanetType;
typedef struct MapCurlInfoToJanetType MapCurlInfoToJanetType;
typedef struct Curl Curl;
typedef struct Curlsh Curlsh;
typedef struct Url Url;
typedef struct Mime Mime;
typedef struct MimePart MimePart; // does not need GC, it gets freed when Mime parent object GC happens

struct MapCurlOptionToJanetType {
    int option;
    JanetType type;
};

struct MapCurlInfoToJanetType {
    int info;
    JanetType type;
};

struct Curl {
    CURL *curl;
    JanetFunction *write_function;
    JanetFunction *header_function;
    JanetFunction *read_function;
    JanetFunction *progress_function;
};

struct Curlsh {
    CURLSH *curlsh;
};

struct Url {
    CURLU *url;
};

struct Mime {
    curl_mime *mime;
};

struct MimePart {
    curl_mimepart *mimepart;
};

//==============================================================================
// ██████╗ ██████╗  ██████╗ ████████╗ ██████╗ ████████╗██╗   ██╗██████╗ ███████╗███████╗
// ██╔══██╗██╔══██╗██╔═══██╗╚══██╔══╝██╔═══██╗╚══██╔══╝╚██╗ ██╔╝██╔══██╗██╔════╝██╔════╝
// ██████╔╝██████╔╝██║   ██║   ██║   ██║   ██║   ██║    ╚████╔╝ ██████╔╝█████╗  ███████╗
// ██╔═══╝ ██╔══██╗██║   ██║   ██║   ██║   ██║   ██║     ╚██╔╝  ██╔═══╝ ██╔══╝  ╚════██║
// ██║     ██║  ██║╚██████╔╝   ██║   ╚██████╔╝   ██║      ██║   ██║     ███████╗███████║
// ╚═╝     ╚═╝  ╚═╝ ╚═════╝    ╚═╝    ╚═════╝    ╚═╝      ╚═╝   ╚═╝     ╚══════╝╚══════╝
//==============================================================================

static int curl_gc_fn(void *data, size_t len);
static int curl_get_fn(void *data, Janet key, Janet *out);
static Janet easy_init(int32_t argc, Janet *argv);
static Janet easy_clone(int32_t argc, Janet *argv);
static Janet easy_escape(int32_t argc, Janet *argv);
static Janet easy_unescape(int32_t argc, Janet *argv);
static Janet easy_strerror(int32_t argc, Janet *argv);
static Janet easy_setopt(int32_t argc, Janet *argv);
static Janet easy_pause(int32_t argc, Janet *argv);
static Janet easy_reset(int32_t argc, Janet *argv);
static Janet easy_upkeep(int32_t argc, Janet *argv);
static Janet easy_recv(int32_t argc, Janet *argv);
static Janet easy_send(int32_t argc, Janet *argv);
static Janet easy_perform(int32_t argc, Janet *argv);
static Janet easy_query(int32_t argc, Janet *argv);

static int curlsh_gc_fn(void *data, size_t len);
static int curl_mark_fn(void *data, size_t len);
static int curlsh_get_fn(void *data, Janet key, Janet *out);
static Janet share_init(int32_t argc, Janet *argv);
static Janet share_setopt(int32_t argc, Janet *argv);
static Janet share_strerror(int32_t argc, Janet *argv);

static int url_gc_fn(void *data, size_t len);
static int url_get_fn(void *data, Janet key, Janet *out);
static Janet url_init(int32_t argc, Janet *argv);
static Janet url_clone(int32_t argc, Janet *argv);
static Janet url_get(int32_t argc, Janet *argv);
static Janet url_set(int32_t argc, Janet *argv);

static int mime_gc_fn(void *data, size_t len);
static int mime_get_fn(void *data, Janet key, Janet *out);
static Janet mime_init(int32_t argc, Janet *argv);

//==============================================================================
//  ██████╗ ██╗      ██████╗ ██████╗  █████╗ ██╗     ███████╗
// ██╔════╝ ██║     ██╔═══██╗██╔══██╗██╔══██╗██║     ██╔════╝
// ██║  ███╗██║     ██║   ██║██████╔╝███████║██║     ███████╗
// ██║   ██║██║     ██║   ██║██╔══██╗██╔══██║██║     ╚════██║
// ╚██████╔╝███████╗╚██████╔╝██████╔╝██║  ██║███████╗███████║
//  ╚═════╝ ╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝
//==============================================================================

KHASH_MAP_INIT_STR(HashMapCurlOptionToJanetType, MapCurlOptionToJanetType *);
khash_t(HashMapCurlOptionToJanetType) *hashmap_opt_to_type = NULL;

KHASH_MAP_INIT_STR(HashMapCurlInfoToJanetType, MapCurlInfoToJanetType *);
khash_t(HashMapCurlInfoToJanetType) *hashmap_info_to_type = NULL;

JanetAbstractType curl_obj = {"curl", curl_gc_fn, curl_mark_fn, curl_get_fn, JANET_ATEND_GET};
JanetAbstractType curlsh_obj = {"curlsh", curlsh_gc_fn, NULL, curlsh_get_fn, JANET_ATEND_GET};
JanetAbstractType url_obj = {"url", url_gc_fn, NULL, url_get_fn, JANET_ATEND_GET};
JanetAbstractType mime_obj = {"mime", mime_gc_fn, NULL, mime_get_fn, JANET_ATEND_GET};
JanetAbstractType mimepart_obj = {"mimepart", JANET_ATEND_NAME};

static JanetMethod curl_methods[] = {
    {"clone", easy_clone},
    {"escape", easy_escape},
    {"unescape", easy_unescape},
    {"setopt", easy_setopt},
    {"pause", easy_pause},
    {"reset", easy_reset},
    {"upkeep", easy_upkeep},
    {"recv", easy_recv},
    {"send", easy_send},
    {"perform", easy_perform},
    {"query", easy_query},
    {NULL, NULL}
};

static JanetMethod curlsh_methods[] = {
    {"setopt", easy_setopt},
    {NULL, NULL}
};

static JanetMethod url_methods[] = {
    {"clone", url_clone},
    {"get", url_get},
    {"set", url_set},
    {NULL, NULL}
};

static JanetMethod mime_methods[] = { // FIXME:
    {NULL, NULL}
};

//==============================================================================
// ██████╗  █████╗ ███████╗███████╗
// ██╔══██╗██╔══██╗██╔════╝██╔════╝
// ██████╔╝███████║███████╗█████╗
// ██╔══██╗██╔══██║╚════██║██╔══╝
// ██████╔╝██║  ██║███████║███████╗
// ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝
//==============================================================================

static Janet curl_make(CURL *curl) {
    Curl *c = (Curl *) janet_abstract(&curl_obj, sizeof(Curl));
    memset(c, 0, sizeof(Curl));
    c->curl = curl;
    return janet_wrap_abstract(c);
}

static int curl_gc_fn(void *data, size_t len) {
    (void) len;

    Curl *c = (Curl *)data;
    curl_easy_cleanup(c->curl);

    return 0;
}

static int curl_mark_fn(void *data, size_t len) {
    (void) len;

    Curl *c = (Curl *)data;
    if (c->read_function) janet_mark(janet_wrap_function(c->read_function));
    if (c->write_function) janet_mark(janet_wrap_function(c->write_function));
    if (c->progress_function) janet_mark(janet_wrap_function(c->progress_function));
    if (c->header_function) janet_mark(janet_wrap_function(c->header_function));
    return 0;
}

static int curl_get_fn(void *data, Janet key, Janet *out) {
    (void) data;

    if (!janet_checktype(key, JANET_KEYWORD))
        janet_panicf("expected keyword, got %v", key);
    return janet_getmethod(janet_unwrap_keyword(key), curl_methods, out);
}

static Janet curlsh_make(CURLSH *curlsh) {
    Curlsh *c = (Curlsh *) janet_abstract(&curl_obj, sizeof(Curlsh));
    c->curlsh = curlsh;
    return janet_wrap_abstract(c);
}

static int curlsh_gc_fn(void *data, size_t len) {
    (void) len;

    Curlsh *c = (Curlsh *)data;
    curl_share_cleanup(c->curlsh);

    return 0;
}

static int curlsh_get_fn(void *data, Janet key, Janet *out) {
    (void) data;

    if (!janet_checktype(key, JANET_KEYWORD))
        janet_panicf("expected keyword, got %v", key);
    return janet_getmethod(janet_unwrap_keyword(key), curlsh_methods, out);
}

static Janet url_make(CURLU *url) {
    Url *u = (Url *) janet_abstract(&url_obj, sizeof(Url));
    u->url = url;
    return janet_wrap_abstract(u);
}

static int url_gc_fn(void *data, size_t len) {
    (void) len;

    Url *u = (Url *)data;
    curl_url_cleanup(u->url);

    return 0;
}

static int url_get_fn(void *data, Janet key, Janet *out) {
    (void) data;

    if (!janet_checktype(key, JANET_KEYWORD))
        janet_panicf("expected keyword, got %v", key);
    return janet_getmethod(janet_unwrap_keyword(key), url_methods, out);
}

static Janet mime_make(curl_mime *mime) {
    Mime *m = (Mime *) janet_abstract(&mime_obj, sizeof(Mime));
    m->mime = mime;
    return janet_wrap_abstract(m);
}

static int mime_gc_fn(void *data, size_t len) {
    (void) len;

    Mime *m = (Mime *)data;
    curl_mime_free(m->mime);

    return 0;
}

static int mime_get_fn(void *data, Janet key, Janet *out) {
    (void) data;

    if (!janet_checktype(key, JANET_KEYWORD))
        janet_panicf("expected keyword, got %v", key);
    return janet_getmethod(janet_unwrap_keyword(key), mime_methods, out);
}

static Janet mimepart_make(curl_mimepart *mimepart) {
    MimePart *mp = (MimePart *) janet_abstract(&mimepart_obj, sizeof(MimePart));
    mp->mimepart = mimepart;
    return janet_wrap_abstract(mp);
}

//==============================================================================
//  ██████╗ █████╗ ██╗     ██╗     ██████╗  █████╗  ██████╗██╗  ██╗███████╗
// ██╔════╝██╔══██╗██║     ██║     ██╔══██╗██╔══██╗██╔════╝██║ ██╔╝██╔════╝
// ██║     ███████║██║     ██║     ██████╔╝███████║██║     █████╔╝ ███████╗
// ██║     ██╔══██║██║     ██║     ██╔══██╗██╔══██║██║     ██╔═██╗ ╚════██║
// ╚██████╗██║  ██║███████╗███████╗██████╔╝██║  ██║╚██████╗██║  ██╗███████║
//  ╚═════╝╚═╝  ╚═╝╚══════╝╚══════╝╚═════╝ ╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝
//==============================================================================

static size_t funcs_write(void *buff, size_t size, size_t count, void *udata) {
    const size_t len = size * count;

    if (len > 0 && buff != NULL) {
        JanetBuffer *jbuff = janet_buffer(len);
        janet_buffer_push_bytes(jbuff, buff, len);
        Janet arg = janet_wrap_buffer(jbuff);

        JanetFunction *jfunc = (JanetFunction *)udata;
        janet_call(jfunc, 1, &arg);
    }

    return len;
}

static size_t funcs_read(char *buff, size_t size, size_t count, void *udata) {
    Janet len = janet_wrap_number(size * count);
    JanetFunction *jfunc = (JanetFunction *)udata;
    Janet jbuff = janet_call(jfunc, 1, &len);
    JanetByteView bytes = janet_getbytes(&jbuff, 0);
    if ((size_t) bytes.len > size) bytes.len = (int32_t) size;
    memcpy(buff, bytes.bytes, bytes.len);
    return (size_t) bytes.len;
}

static int funcs_progress(void *udata, double dltotal, double dlnow, double ultotal, double ulnow) {
    JanetFunction *jfunc = (JanetFunction *)udata;
    Janet args[4];
    args[0] = janet_wrap_number(dltotal);
    args[1] = janet_wrap_number(dlnow);
    args[2] = janet_wrap_number(ultotal);
    args[3] = janet_wrap_number(ulnow);

    Janet res = janet_call(jfunc, 4, args);

    return janet_truthy(res);
}

// static size_t funcs_mime_mem_read(char *buffer, size_t size, size_t nitems, void *instream)
// {
//   curl_mimepart *part = (curl_mimepart *) instream;
//   size_t sz = (size_t) part->datasize - part->state.offset;
//   (void) size;   /* Always 1.*/

//   if(sz > nitems)
//     sz = nitems;

//   if(sz)
//     memcpy(buffer, (char *) &part->data[part->state.offset], sz);

//   part->state.offset += sz;
//   return sz;
// }

// static int funcs_mime_mem_seek(void *instream, curl_off_t offset, int whence)
// {
//   curl_mimepart *part = (curl_mimepart *) instream;

//   switch(whence) {
//   case SEEK_CUR:
//     offset += part->state.offset;
//     break;
//   case SEEK_END:
//     offset += part->datasize;
//     break;
//   }

//   if(offset < 0 || offset > part->datasize)
//     return CURL_SEEKFUNC_FAIL;

//   part->state.offset = (size_t) offset;
//   return CURL_SEEKFUNC_OK;
// }

// static void funcs_mime_mem_free(void *ptr)
// {
//   Curl_safefree(((curl_mimepart *) ptr)->data);
// }


//==============================================================================
//  ██████╗ ██████╗ ████████╗██╗ ██████╗ ███╗   ██╗███████╗
// ██╔═══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
// ██║   ██║██████╔╝   ██║   ██║██║   ██║██╔██╗ ██║███████╗
// ██║   ██║██╔═══╝    ██║   ██║██║   ██║██║╚██╗██║╚════██║
// ╚██████╔╝██║        ██║   ██║╚██████╔╝██║ ╚████║███████║
//  ╚═════╝ ╚═╝        ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
//==============================================================================

static struct  {
    char name[64];
    MapCurlOptionToJanetType kjm;
} _key_option_type_arr[] = {
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
    {"aws-sigv4", {CURLOPT_AWS_SIGV4, JANET_STRING}},
    {"copy-post-fields", {CURLOPT_COPYPOSTFIELDS, JANET_STRING}},

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
    {"post-fields", {CURLOPT_POSTFIELDS, JANET_POINTER}},
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
    {"http-header", {CURLOPT_HTTPHEADER, JANET_POINTER}},
    {"error-buffer@TODO", {CURLOPT_ERRORBUFFER, JANET_POINTER}}, // TODO
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
    {"open-socket-data@TODO", {CURLOPT_OPENSOCKETDATA, JANET_POINTER}}, // TODO
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

static void options_gen_dict(void) {
    int32_t size = sizeof(_key_option_type_arr) / sizeof(_key_option_type_arr[0]);

    if (NULL == hashmap_opt_to_type) {
        hashmap_opt_to_type = kh_init(HashMapCurlOptionToJanetType);
    }

    int absent;
    khint_t where;
    for (int32_t idx = 0; idx < size; idx++) {
        where = kh_put(HashMapCurlOptionToJanetType, hashmap_opt_to_type, _key_option_type_arr[idx].name, &absent);
        kh_value(hashmap_opt_to_type, where) = &_key_option_type_arr[idx].kjm;
    }
}

static const MapCurlOptionToJanetType *options_get(const char *key) {
    MapCurlOptionToJanetType *val;
    khint_t where = kh_get(HashMapCurlOptionToJanetType, hashmap_opt_to_type, key);
    val = kh_val(hashmap_opt_to_type, where);
    return val;
}

static struct curl_slist *fill_list_from_tuple(Janet tuple) {
    const JanetTupleHead *t = janet_tuple_head(janet_unwrap_tuple(tuple));
    if (t->length <= 0) {
        janet_panic("input list cannot be empty");
    }

    const char *str1 = (const char *) janet_unwrap_string(t->data[0]);
    struct curl_slist *list = curl_slist_append(NULL, str1);

    for (int32_t idx = 1; idx < t->length; idx++) {
        const char *str = (const char *) janet_unwrap_string(t->data[idx]);
        list = curl_slist_append(list, str);
    }

    return list;
}

static struct curl_slist *fill_list_from_array(Janet array) {
    const JanetArray *a = janet_unwrap_array(array);
    if (a->count <= 0) {
        janet_panic("input list cannot be empty");
    }

    const char *str1 = (const char *) janet_unwrap_string(a->data[0]);
    struct curl_slist *list = curl_slist_append(NULL, str1);

    for (int32_t idx = 1; idx < a->count; idx++) {
        const char *str = (const char *) janet_unwrap_string(a->data[idx]);
        list = curl_slist_append(list, str);
    }

    return list;
}

static void options_set(Curl *c, Janet *key, Janet *val) {
    const char *keyword = (const char *) janet_unwrap_keyword(*key);
    const MapCurlOptionToJanetType *map = options_get(keyword);
    CURL *curl = c->curl;
    if (NULL == map) {
        janet_panic("invalid keyword");
    }

    int type = map->type;
    int opt = map->option;
    JanetFunction *fn;

    switch (type) {
        case JANET_NUMBER:
            if (!janet_checkint(*val)) janet_panicf("expected integer, got %v", *val);
            curl_easy_setopt(curl, opt, janet_unwrap_integer(*val));
            break;
        case JANET_BOOLEAN:
            curl_easy_setopt(curl, opt, janet_unwrap_boolean(*val));
            break;
        case JANET_STRING:
            curl_easy_setopt(curl, opt, janet_unwrap_string(*val));
            break;
        case JANET_FUNCTION:
            fn = janet_unwrap_function(*val);
            if (0 == strcmp(keyword, "write-function")) {
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fn);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, funcs_write);
                c->write_function = fn;
            } else if (0 == strcmp(keyword, "header-function")) {
                curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)fn);
                curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, funcs_write);
                c->header_function = fn;
            } else if (0 == strcmp(keyword, "read-function")) {
                curl_easy_setopt(curl, CURLOPT_READDATA, (void *)fn);
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, funcs_read);
                c->read_function = fn;
            } else if (0 == strcmp(keyword, "progress-function")) {
                curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, (void *)fn);
                curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, funcs_progress);
                c->progress_function = fn;
            }

            break;
        case JANET_POINTER:
            if (0 == strcmp(keyword, "http-header")) {
                struct curl_slist *list;
                if (janet_checktype(*val, JANET_TUPLE) != 0) {
                    list = fill_list_from_tuple(*val);
                } else if (janet_checktype(*val, JANET_ARRAY) != 0) {
                    list = fill_list_from_array(*val);
                } else {
                    janet_panic("header must be an array or a tuple");
                }

                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            } else {
                janet_panic("not implemented yet");
            }
            // TODO:
            break;
        default:
            janet_panic("value type is not supported");
            break;
    };
}

//==============================================================================
// ██╗███╗   ██╗███████╗ ██████╗
// ██║████╗  ██║██╔════╝██╔═══██╗
// ██║██╔██╗ ██║█████╗  ██║   ██║
// ██║██║╚██╗██║██╔══╝  ██║   ██║
// ██║██║ ╚████║██║     ╚██████╔╝
// ╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝
//==============================================================================

static struct  {
    char name[64];
    MapCurlInfoToJanetType ijm;
} _key_info_type_arr[] = {
    {"query-effective-url", {CURLINFO_EFFECTIVE_URL, JANET_STRING}},
    {"query-content-type", {CURLINFO_CONTENT_TYPE, JANET_STRING}},
    {"query-private", {CURLINFO_PRIVATE, JANET_STRING}},
    {"query-ftp-entry-path", {CURLINFO_FTP_ENTRY_PATH, JANET_STRING}},
    {"query-redirect-url", {CURLINFO_REDIRECT_URL, JANET_STRING}},
    {"query-primary-ip", {CURLINFO_PRIMARY_IP, JANET_STRING}},
    {"query-rtsp-session-id", {CURLINFO_RTSP_SESSION_ID, JANET_STRING}},
    {"query-local-ip", {CURLINFO_LOCAL_IP, JANET_STRING}},
    {"query-scheme", {CURLINFO_SCHEME, JANET_STRING}},

    {"query-response-code", {CURLINFO_RESPONSE_CODE, JANET_NUMBER}},
    {"query-total-time", {CURLINFO_TOTAL_TIME, JANET_NUMBER}},
    {"query-name-lookup-time", {CURLINFO_NAMELOOKUP_TIME, JANET_NUMBER}},
    {"query-connect-time", {CURLINFO_CONNECT_TIME, JANET_NUMBER}},
    {"query-pre-transfer-time", {CURLINFO_PRETRANSFER_TIME, JANET_NUMBER}},
    {"query-size-upload", {CURLINFO_SIZE_UPLOAD, JANET_NUMBER}},
    {"query-size-upload-t", {CURLINFO_SIZE_UPLOAD_T, JANET_NUMBER}},
    {"query-size-download", {CURLINFO_SIZE_DOWNLOAD, JANET_NUMBER}},
    {"query-size-download-t", {CURLINFO_SIZE_DOWNLOAD_T, JANET_NUMBER}},
    {"query-speed-download", {CURLINFO_SPEED_DOWNLOAD, JANET_NUMBER}},
    {"query-speed-download-t", {CURLINFO_SPEED_DOWNLOAD_T, JANET_NUMBER}},
    {"query-speed-upload", {CURLINFO_SPEED_UPLOAD, JANET_NUMBER}},
    {"query-speed-upload-t", {CURLINFO_SPEED_UPLOAD_T, JANET_NUMBER}},
    {"query-header-size", {CURLINFO_HEADER_SIZE, JANET_NUMBER}},
    {"query-request-size", {CURLINFO_REQUEST_SIZE, JANET_NUMBER}},
    {"query-ssl-verify-result", {CURLINFO_SSL_VERIFYRESULT, JANET_NUMBER}},
    {"query-file-time", {CURLINFO_FILETIME, JANET_NUMBER}},
    {"query-file-time-t", {CURLINFO_FILETIME_T, JANET_NUMBER}},
    {"query-content-length-download", {CURLINFO_CONTENT_LENGTH_DOWNLOAD, JANET_NUMBER}},
    {"query-content-length-download-t", {CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, JANET_NUMBER}},
    {"query-content-length-upload", {CURLINFO_CONTENT_LENGTH_UPLOAD, JANET_NUMBER}},
    {"query-content-length-upload-t", {CURLINFO_CONTENT_LENGTH_UPLOAD_T, JANET_NUMBER}},
    {"query-start-transfer-time", {CURLINFO_STARTTRANSFER_TIME, JANET_NUMBER}},
    {"query-redirect-time", {CURLINFO_REDIRECT_TIME, JANET_NUMBER}},
    {"query-redirect-count", {CURLINFO_REDIRECT_COUNT, JANET_NUMBER}},
    {"query-http-connect-code", {CURLINFO_HTTP_CONNECTCODE, JANET_NUMBER}},
    {"query-http-auth-avail", {CURLINFO_HTTPAUTH_AVAIL, JANET_NUMBER}},
    {"query-proxy-auth-avail", {CURLINFO_PROXYAUTH_AVAIL, JANET_NUMBER}},
    {"query-os-errno", {CURLINFO_OS_ERRNO, JANET_NUMBER}},
    {"query-num-connects", {CURLINFO_NUM_CONNECTS, JANET_NUMBER}},
    {"query-last-socket", {CURLINFO_LASTSOCKET, JANET_NUMBER}},
    {"query-app-connect-time", {CURLINFO_APPCONNECT_TIME, JANET_NUMBER}},
    {"query-condition-unmet", {CURLINFO_CONDITION_UNMET, JANET_NUMBER}},
    {"query-rtsp-client-cseq", {CURLINFO_RTSP_CLIENT_CSEQ, JANET_NUMBER}},
    {"query-rtsp-server-cseq", {CURLINFO_RTSP_SERVER_CSEQ, JANET_NUMBER}},
    {"query-rtsp-cseq-recv", {CURLINFO_RTSP_CSEQ_RECV, JANET_NUMBER}},
    {"query-primary-port", {CURLINFO_PRIMARY_PORT, JANET_NUMBER}},
    {"query-local-port", {CURLINFO_LOCAL_PORT, JANET_NUMBER}},
    {"query-http-version", {CURLINFO_HTTP_VERSION, JANET_NUMBER}},
    {"query-proxy-ssl-verify-result", {CURLINFO_PROXY_SSL_VERIFYRESULT, JANET_NUMBER}},
    {"query-protocol", {CURLINFO_PROTOCOL, JANET_NUMBER}},
    {"query-total-time-t", {CURLINFO_TOTAL_TIME_T, JANET_NUMBER}},
    {"query-name-lookup-time-t", {CURLINFO_NAMELOOKUP_TIME_T, JANET_NUMBER}},
    {"query-connect-time-t", {CURLINFO_CONNECT_TIME_T, JANET_NUMBER}},
    {"query-pre-transfer-time-t", {CURLINFO_PRETRANSFER_TIME_T, JANET_NUMBER}},
    {"query-start-transfer-time-t", {CURLINFO_STARTTRANSFER_TIME_T, JANET_NUMBER}},
    {"query-redirect-time-t", {CURLINFO_REDIRECT_TIME_T, JANET_NUMBER}},
    {"query-app-connect-time-t", {CURLINFO_APPCONNECT_TIME_T, JANET_NUMBER}},

    {"query-ssl-engines", {CURLINFO_SSL_ENGINES, JANET_POINTER}}, // TODO:
    {"query-cookie-list", {CURLINFO_COOKIELIST, JANET_POINTER}}, // TODO:
    {"query-cert-info", {CURLINFO_CERTINFO, JANET_POINTER}}, // TODO:
    {"query-tls-session", {CURLINFO_TLS_SESSION, JANET_POINTER}}, // TODO:
    {"query-active-socket", {CURLINFO_ACTIVESOCKET, JANET_POINTER}}, // TODOL
    {"query-tls-ssl-ptr", {CURLINFO_TLS_SSL_PTR, JANET_POINTER}}, // TODO:
};

static void info_gen_dict(void) {
    int32_t size = sizeof(_key_info_type_arr) / sizeof(_key_info_type_arr[0]);

    if (NULL == hashmap_info_to_type) {
        hashmap_info_to_type = kh_init(HashMapCurlInfoToJanetType);
    }

    int absent;
    khint_t where;
    for (int32_t idx = 0; idx < size; idx++) {
        where = kh_put(HashMapCurlInfoToJanetType, hashmap_info_to_type, _key_info_type_arr[idx].name, &absent);
        kh_value(hashmap_info_to_type, where) = &_key_info_type_arr[idx].ijm;
    }
}

static const MapCurlInfoToJanetType *info_get(const char *key) {
    MapCurlInfoToJanetType *val;
    khint_t where = kh_get(HashMapCurlInfoToJanetType, hashmap_info_to_type, key);
    val = kh_val(hashmap_info_to_type, where);
    return val;
}

static Janet info_query(CURL *curl, Janet *key) {
    const char *keyword = (const char *) janet_unwrap_keyword(*key);
    const MapCurlInfoToJanetType *map = info_get(keyword);
    if (NULL == map) {
        janet_panic("invalid keyword");
    }

    int type = map->type;
    int info = map->info;

    int32_t res_number;
    const char *res_string;

    switch (type) {
        case JANET_NUMBER:
            if (CURLE_OK == curl_easy_getinfo(curl, info, &res_number)) {
                return janet_wrap_integer(res_number);
            }
            break;
        case JANET_STRING:
            if (CURLE_OK == curl_easy_getinfo(curl, info, &res_string)) {
                return janet_wrap_string((const char *)res_string);
            }
            break;
        default:
            janet_panic("value type is not supported");
            break;
    };

    return janet_wrap_false();
}

//==============================================================================
// ███████╗ █████╗ ███████╗██╗   ██╗
// ██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝
// █████╗  ███████║███████╗ ╚████╔╝
// ██╔══╝  ██╔══██║╚════██║  ╚██╔╝
// ███████╗██║  ██║███████║   ██║
// ╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝
//==============================================================================

static Janet easy_init(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 0);
    (void)argv;

    CURL *curl = curl_easy_init();
    return curl ? curl_make(curl) : janet_wrap_nil();
}

static Janet easy_clone(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);
    CURL *clone = curl_easy_duphandle(curl->curl);

    return clone ? curl_make(clone) : janet_wrap_nil();
}

static Janet easy_escape(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);
    const char *iurl = janet_getcstring(argv, 1);
    if (NULL == iurl) {
        janet_panic("input url string is invalid or null");
    }

    int len = strlen(iurl);
    if (0 >= len) {
        janet_panic("input url cannot be empty");
    }

    char *ourl = curl_easy_escape(curl->curl, iurl, len);
    if (NULL == ourl) {
        curl_free(ourl);
        return janet_wrap_false();
    }

    Janet res = janet_cstringv(ourl);
    curl_free(ourl);

    return res;
}

static Janet easy_unescape(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);
    const char *iurl = janet_getcstring(argv, 1);
    if (NULL == iurl) {
        janet_panic("input url string is invalid or null");
    }

    int ilen = strlen(iurl);
    if (0 >= ilen) {
        janet_panic("input url cannot be empty");
    }

    int olen = 0;

    char *ourl = curl_easy_unescape(curl->curl, iurl, ilen, &olen);
    if (NULL == ourl || 0 >= olen) {
        return janet_wrap_false();
    }

    Janet res = janet_cstringv(ourl);
    curl_free(ourl);

    return res;
}

static Janet easy_strerror(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    int code = janet_getinteger(argv, 0);

    const char *errstr = curl_easy_strerror(code);
    Janet res = janet_cstringv(errstr);

    return res;
}

static Janet easy_setopt(int32_t argc, Janet *argv) {
    janet_arity(argc, 2, -1);

    if ((argc - 1) % 2 != 0) {
        janet_panic("options count must be even, options are regarded as tuple");
    }

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    for (int32_t idx = 1; idx < argc; idx += 2) {
        options_set(curl, argv + idx, argv + idx + 1);
    }

    return janet_wrap_true();
}

static Janet easy_pause(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);
    const char *modestr = (const char *) janet_getkeyword(argv, 1);

    int mode = -1;
    if (0 == strcmp(modestr, "pause-mode-recv")) {
        mode = CURLPAUSE_RECV;
    } else if (0 == strcmp(modestr, "pause-mode-send")) {
        mode = CURLPAUSE_SEND;
    } else if (0 == strcmp(modestr, "paus-modee-all")) {
        mode = CURLPAUSE_ALL;
    } else if (0 == strcmp(modestr, "pause-mode-cont")) {
        mode = CURLPAUSE_CONT;
    } else {
        janet_panic("invalid pause mode");
    }

    curl_easy_pause(curl->curl, mode);
    return janet_wrap_true();
}

static Janet easy_reset(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    curl_easy_reset(curl->curl);
    return janet_wrap_nil();
}

static Janet easy_upkeep(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    curl_easy_upkeep(curl->curl);
    return janet_wrap_nil();
}

static Janet easy_recv(int32_t argc, Janet *argv) {
    janet_arity(argc, 1, 2);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    size_t buflen = DEFAULT_EASY_RECV_BUF_LEN;
    size_t recvlen = -1;
    if (argc == 2) {
        buflen = janet_getinteger(argv, 1);
    }

    char *buffer = (char *) calloc(buflen, sizeof(char));
    int res = curl_easy_recv(curl->curl, buffer, buflen, &recvlen);
    JanetBuffer *jbuff = janet_buffer(buflen);
    janet_buffer_push_bytes(jbuff, (const uint8_t *)buffer, buflen);
    free(buffer);

    // TODO: i am not satisfied with this function

    if (0 == recvlen) {
        return janet_ckeywordv("closed");
    }

    switch (res) {
        case CURLE_OK:
            return janet_wrap_buffer(jbuff);
            break;
        case CURLE_AGAIN:
            return janet_ckeywordv("more");
            break;
        case CURLE_UNSUPPORTED_PROTOCOL:
            return janet_ckeywordv("unsopported");
            break;
        default:
            return janet_wrap_false();
    }
}

static Janet easy_send(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    size_t sendlen = -1;
    JanetByteView buffer = janet_getbytes(argv, 1);

    int res = curl_easy_send(curl->curl, buffer.bytes, buffer.len, &sendlen);

    switch (res) {
        case CURLE_OK:
            return janet_wrap_true();
            break;
        case CURLE_AGAIN:
            return janet_ckeywordv("more");
            break;
        case CURLE_UNSUPPORTED_PROTOCOL:
            return janet_ckeywordv("unsopported");
            break;
        default:
            return janet_wrap_false();
    }
}

static Janet easy_perform(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    CURLcode res;
    res = curl_easy_perform(curl->curl);
    return janet_wrap_integer(res);
}

static Janet easy_query(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);

    return info_query(curl->curl, argv + 1);
}

static const JanetReg curl_easy_cfuns[] = {
    {
        "easy/init", easy_init,
        "(def curl (easy/init)) \n\n"
        "create a new curl handler"
    },
    {
        "easy/clone", easy_clone,
        "(easy/clone curl) \n\n"
        "clones a libcurl session handle"
    },
    {
        "easy/escape", easy_escape,
        "(easy/escape curl \"https://janet-lang.org\") \n\n"
        "encode the input string as an url escaped string"
    },
    {
        "easy/unescape", easy_unescape,
        "(easy/unescape curl \"https\%3A\%2F\%2Fjanet-lang.org\") \n\n"
        "decode the input url string as an unescape string"
    },
    {
        "easy/strerror", easy_strerror,
        "(easy/strerror code) \n\n"
        "return error string of an error code"
    },
    {
        "easy/setopt", easy_setopt,
        "(easy/setopt code :url \"https://janet-lang.org\") \n\n"
        "set options for a curl easy handle"
    },
    {
        "easy/pause", easy_pause,
        "(easy/pause curl :pause_mode_all) \n\n"
        "pause and unpause a connection"
    },
    {
        "easy/reset", easy_reset,
        "(easy/reset curl) \n\n"
        "reset all options of a libcurl session handle"
    },
    {
        "easy/upkeep", easy_upkeep,
        "(easy/upkeep curl) \n\n"
        "perform any connection upkeep checks"
    },
    {
        "easy/recv", easy_recv,
        "(easy/recv curl) \n\n"
        "receives raw data on an \"easy\" connection"
    },
    {
        "easy/send", easy_send,
        "(easy/send curl) \n\n"
        "sends raw data over an \"easy\" connection"
    },
    {
        "easy/perform", easy_perform,
        "(easy/perform curl) \n\n"
        "perform a blocking file transfer"
    },
    {
        "easy/query", easy_query,
        "(easy/query curl :query-effective-url) \n\n"
        "query information from curl easy handler"
    },
    {NULL, NULL, NULL}
};

static void submod_easy(JanetTable *env) {
    janet_cfuns(env, "curl", curl_easy_cfuns);
}

//==============================================================================
// ███████╗██╗  ██╗ █████╗ ██████╗ ███████╗
// ██╔════╝██║  ██║██╔══██╗██╔══██╗██╔════╝
// ███████╗███████║███████║██████╔╝█████╗
// ╚════██║██╔══██║██╔══██║██╔══██╗██╔══╝
// ███████║██║  ██║██║  ██║██║  ██║███████╗
// ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝
//==============================================================================

static Janet share_init(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 0);
    (void)argv;

    CURLSH *curlsh = curl_share_init();
    return curlsh ? curlsh_make(curlsh) : janet_wrap_nil();
}

static Janet share_setopt(int32_t argc, Janet *argv) {
    janet_arity(argc, 2, -1);

    if ((argc - 1) % 2 != 0) {
        janet_panic("options count must be even, options are regarded as tuple");
    }

    janet_panic("this function is implemented yet");

    (void) argv;
    // Curl* curl = janet_getabstract(argv, 0, &curl_obj);

    // for (int32_t idx = 1; idx < argc; idx+=2){
    //   options_set(curl, argv + idx, argv + idx + 1);
    // }

    return janet_wrap_true();
}

static Janet share_strerror(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    int code = janet_getinteger(argv, 0);

    const char *errstr = curl_share_strerror(code);
    Janet res = janet_cstringv(errstr);

    return res;
}

static const JanetReg curl_share_cfuns[] = {
    {
        "share/init", share_init,
        "(def curl (share/init)) \n\n"
        "create a new curl share handler"
    },
    {
        "share/strerror", share_strerror,
        "(share/strerror code) \n\n"
        "return error string of an error code"
    },
    {
        "share/setopt", share_setopt,
        "(share/setopt code :url \"https://janet-lang.org\") \n\n"
        "set options for a curl share handle"
    },
    {NULL, NULL, NULL}
};

static void submod_share(JanetTable *env) {
    janet_cfuns(env, "curl", curl_share_cfuns);
}

//==============================================================================
// ██╗   ██╗██████╗ ██╗
// ██║   ██║██╔══██╗██║
// ██║   ██║██████╔╝██║
// ██║   ██║██╔══██╗██║
// ╚██████╔╝██║  ██║███████╗
//  ╚═════╝ ╚═╝  ╚═╝╚══════╝
//==============================================================================

static Janet url_init(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 0);
    (void)argv;

    CURLU *url = curl_url();
    return url ? url_make(url) : janet_wrap_nil();
}

static Janet url_clone(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Url *url = janet_getabstract(argv, 0, &url_obj);
    CURLU *clone = curl_url_dup(url->url);

    return clone ? url_make(clone) : janet_wrap_nil();
}

static Janet url_get(int32_t argc, Janet *argv) {
    janet_arity(argc, 2, 3);

    Url *url = janet_getabstract(argv, 0, &url_obj);
    const uint32_t part = janet_getinteger(argv, 1);
    char *extracted;
    uint32_t flag = 0;

    if (3 == argc) {
        flag = (uint32_t) janet_getinteger(argv, 2);
    }

    if (CURLUE_OK == curl_url_get(url->url, part, &extracted, flag)) {
        Janet res = janet_cstringv(extracted);
        curl_free(extracted);
        return res;
    }

    return janet_wrap_nil();
}

static Janet url_set(int32_t argc, Janet *argv) {
    janet_arity(argc, 3, 4);

    Url *url = janet_getabstract(argv, 0, &url_obj);
    const uint32_t part = janet_getinteger(argv, 1);
    const char *extracted = janet_getcstring(argv, 2);
    uint32_t flag = 0;

    if (4 == argc) {
        flag = (uint32_t) janet_getinteger(argv, 3);
    }

    if (CURLUE_OK == curl_url_set(url->url, part, extracted, flag)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static const JanetReg curl_url_cfuns[] = {
    {
        "url/init", url_init,
        "(def url (url/init)) \n\n"
        "create a new url object"
    },
    {
        "url/clone", url_clone,
        "(url/clone url) \n\n"
        "clones a url object"
    },
    {
        "url/get", url_get,
        "(url/get url :url-part-host) \n\n"
        "extract a part from a url object"
    },
    {
        "url/set", url_set,
        "(url/set url :url-part-port \"2019\") \n\n"
        "extract a part from a url object"
    },
    {NULL, NULL, NULL}
};

static void submod_url(JanetTable *env) {
    janet_cfuns(env, "curl", curl_url_cfuns);
}

//==============================================================================
// ███╗   ███╗██╗███╗   ███╗███████╗
// ████╗ ████║██║████╗ ████║██╔════╝
// ██╔████╔██║██║██╔████╔██║█████╗
// ██║╚██╔╝██║██║██║╚██╔╝██║██╔══╝
// ██║ ╚═╝ ██║██║██║ ╚═╝ ██║███████╗
// ╚═╝     ╚═╝╚═╝╚═╝     ╚═╝╚══════╝
//==============================================================================

static Janet mime_init(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Curl *curl = janet_getabstract(argv, 0, &curl_obj);
    curl_mime *mime = curl_mime_init(curl->curl);

    return mime ? mime_make(mime) : janet_wrap_nil();
}

static Janet mime_part(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    Mime *mime = janet_getabstract(argv, 0, &curl_obj);
    curl_mimepart *mimepart = curl_mime_addpart(mime->mime);

    return mimepart ? mimepart_make(mimepart) : janet_wrap_nil();
}

static Janet mime_subpart(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    Mime *mime = janet_getabstract(argv, 0, &curl_obj);
    MimePart *mimepart = janet_getabstract(argv, 1, &curl_obj);

    if (CURLE_OK == curl_mime_subparts(mimepart->mimepart, mime->mime)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_name(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    const char *name = janet_getcstring(argv, 1);

    if (CURLE_OK == curl_mime_name(mimepart->mimepart, name)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_type(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    const char *type = janet_getcstring(argv, 1);

    if (CURLE_OK == curl_mime_type(mimepart->mimepart, type)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_data(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    const JanetBuffer *buffer = janet_getbuffer(argv, 1);

    if (CURLE_OK == curl_mime_data(mimepart->mimepart, (const char *)buffer->data, buffer->count)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_encoder(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    const char *encoding = janet_getcstring(argv, 1);

    if (CURLE_OK == curl_mime_encoder(mimepart->mimepart, encoding)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_headers(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    JanetView array = janet_getindexed(argv, 1);

    if (array.len <= 0) {
        janet_panic("input list cannot be empty");
    }

    const char *str1 = (const char *) janet_unwrap_string(array.items[0]);
    struct curl_slist *list = curl_slist_append(NULL, str1);

    for (int32_t idx = 1; idx < array.len; idx++) {
        const char *str = (const char *) janet_unwrap_string(array.items[idx]);
        list = curl_slist_append(list, str);
    }

    const int auto_free = 1;
    if (CURLE_OK == curl_mime_headers(mimepart->mimepart, list, auto_free)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_filename(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    const char *filename = janet_getcstring(argv, 1);

    if (CURLE_OK == curl_mime_filename(mimepart->mimepart, filename)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static Janet mime_filedata(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    MimePart *mimepart = janet_getabstract(argv, 0, &curl_obj);
    const char *filename = janet_getcstring(argv, 1);

    if (CURLE_OK == curl_mime_filedata(mimepart->mimepart, filename)) {
        return janet_wrap_true();
    }

    return janet_wrap_false();
}

static const JanetReg curl_mime_cfuns[] = {
    {
        "mime/init", mime_init,
        "(mime/init curl) \n\n"
        "create a mime handle"
    },
    {
        "mime/part", mime_part,
        "(mime/part mime) \n\n"
        "append a new empty part to a mime object"
    },
    {
        "mime/subpart", mime_subpart,
        "(mime/subpart mimepart mime) \n\n"
        "set subparts of a multipart mime part"
    },
    {
        "mime/name", mime_name,
        "(mime/name mimepart \"wtf\") \n\n"
        "set a mime part's name"
    },
    {
        "mime/type", mime_type,
        "(mime/type mimepart \"image\\png\") \n\n"
        "set a mime part's content type"
    },
    {
        "mime/data", mime_data,
        "(mime/data mimepart buffer) \n\n"
        "set a mime part's content"
    },
    {
        "mime/encoder", mime_encoder,
        "(mime/encoder mimepart buffer) \n\n"
        "set mime data transfer encoder"
    },
    {
        "mime/headers", mime_headers,
        "(mime/headers mimepart [\"aaa\" \"bbb\"]) \n\n"
        "set a mime part's custom headers"
    },
    {
        "mime/filename", mime_filename,
        "(mime/filename mimepart \"myfile.png\") \n\n"
        "set a mime part's remote file name"
    },
    {
        "mime/filedata", mime_filedata,
        "(mime/filedata mimepart \"/home/lol/myfile.png\") \n\n"
        "set a mime part's body data from a file contents"
    },
    {NULL, NULL, NULL}
};

static void submod_mime(JanetTable *env) {
    janet_cfuns(env, "curl", curl_mime_cfuns);
}

//==============================================================================
// ███╗   ███╗ ██████╗ ██████╗ ██╗   ██╗██╗     ███████╗
// ████╗ ████║██╔═══██╗██╔══██╗██║   ██║██║     ██╔════╝
// ██╔████╔██║██║   ██║██║  ██║██║   ██║██║     █████╗
// ██║╚██╔╝██║██║   ██║██║  ██║██║   ██║██║     ██╔══╝
// ██║ ╚═╝ ██║╚██████╔╝██████╔╝╚██████╔╝███████╗███████╗
// ╚═╝     ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚══════╝╚══════╝
//==============================================================================

JANET_MODULE_ENTRY(JanetTable *env) {
    options_gen_dict();
    info_gen_dict();

    // GENERAL
    janet_def(env, "version", janet_cstringv(LIBCURL_VERSION), "underlying libcurl value string");

    // .:ENUMS:.
    // CURLOPT_PROXYTYPE
    janet_def(env, "proxy-type-http", janet_wrap_integer(CURLPROXY_HTTP), "http proxy type");
    janet_def(env, "proxy-type-https", janet_wrap_integer(CURLPROXY_HTTPS), "https proxy type");
    janet_def(env, "proxy-type-http-1.0", janet_wrap_integer(CURLPROXY_HTTP_1_0), "http 1.0 proxy type");
    janet_def(env, "proxy-type-socks4", janet_wrap_integer(CURLPROXY_SOCKS4), "socks4 proxy type");
    janet_def(env, "proxy-type-socks4a", janet_wrap_integer(CURLPROXY_SOCKS4A), "socks4a proxy type");
    janet_def(env, "proxy-type-socks5", janet_wrap_integer(CURLPROXY_SOCKS5), "socks5 proxy type");
    janet_def(env, "proxy-type-socks5-hostname", janet_wrap_integer(CURLPROXY_SOCKS5_HOSTNAME), "socks5 resolver proxy type");

    // CURLOPT_FTP_CREATE_MISSING_DIRS
    janet_def(env, "ftp-create-missing-dir-no", janet_wrap_integer(CURLFTP_CREATE_DIR_NONE), "do not create missing dirs");
    janet_def(env, "ftp-create-missing-dir-yes", janet_wrap_integer(CURLFTP_CREATE_DIR), "create missing dirs");
    janet_def(env, "ftp-create-missing-dir-retry", janet_wrap_integer(CURLFTP_CREATE_DIR_RETRY), "retry create missing dirs");

    // CURLOPT_IPRESOLVE
    janet_def(env, "ip-resolve-whatever", janet_wrap_integer(CURL_IPRESOLVE_WHATEVER), "whatever");
    janet_def(env, "ip-resolve-v4", janet_wrap_integer(CURL_IPRESOLVE_V4), "v4");
    janet_def(env, "ip-resolve-v6", janet_wrap_integer(CURL_IPRESOLVE_V6), "v6");

    // CURLOPT_USE_SSL
    janet_def(env, "use-ssl-none", janet_wrap_integer(CURLUSESSL_NONE), "do not attempt to use ssl");
    janet_def(env, "use-ssl-try", janet_wrap_integer(CURLUSESSL_TRY), "try using ssl, proceed anyway otherwise");
    janet_def(env, "use-ssl-control", janet_wrap_integer(CURLUSESSL_CONTROL), "ssl for the control connection or fail");
    janet_def(env, "use-ssl-all", janet_wrap_integer(CURLUSESSL_ALL), "ssl for all communication or fail");

    // CURLOPT_FTPSSLAUTH
    janet_def(env, "ftp-auth-default", janet_wrap_integer(CURLFTPAUTH_DEFAULT), "let libcurl decide");
    janet_def(env, "ftp-auth-ssl", janet_wrap_integer(CURLFTPAUTH_SSL), "use auth ssl");
    janet_def(env, "ftp-auth-tls", janet_wrap_integer(CURLFTPAUTH_TLS), "use auth tls");

    // CURLOPT_FTP_FILEMETHOD
    janet_def(env, "ftp-method-default", janet_wrap_integer(CURLFTPMETHOD_DEFAULT), "let libcurl pick");
    janet_def(env, "ftp-method-multicwd", janet_wrap_integer(CURLFTPMETHOD_MULTICWD), "single cwd operation for each path part");
    janet_def(env, "ftp-method-nocwd", janet_wrap_integer(CURLFTPMETHOD_NOCWD), "no cwd at all");
    janet_def(env, "ftp-method-singlecwd", janet_wrap_integer(CURLFTPMETHOD_SINGLECWD), "one cwd to full dir, then work on file");

    // CURLOPT_FTP_SSL_CCC
    janet_def(env, "ftp-ssl-ccc-none", janet_wrap_integer(CURLFTPSSL_CCC_NONE), "do not send ccc");
    janet_def(env, "ftp-ssl-ccc-passive", janet_wrap_integer(CURLFTPSSL_CCC_PASSIVE), "Let the server initiate the shutdown");
    janet_def(env, "ftp-ssl-ccc-active", janet_wrap_integer(CURLFTPSSL_CCC_ACTIVE), "Initiate the shutdown");

    // CURLOPT_RTSP_REQUEST
    janet_def(env, "rtsp-req-none", janet_wrap_integer(CURL_RTSPREQ_NONE), "//TODO: no description");
    janet_def(env, "rtsp-req-options", janet_wrap_integer(CURL_RTSPREQ_OPTIONS), "//TODO: no description");
    janet_def(env, "rtsp-req-describe", janet_wrap_integer(CURL_RTSPREQ_DESCRIBE), "//TODO: no description");
    janet_def(env, "rtsp-req-announce", janet_wrap_integer(CURL_RTSPREQ_ANNOUNCE), "//TODO: no description");
    janet_def(env, "rtsp-req-setup", janet_wrap_integer(CURL_RTSPREQ_SETUP), "//TODO: no description");
    janet_def(env, "rtsp-req-play", janet_wrap_integer(CURL_RTSPREQ_PLAY), "//TODO: no description");
    janet_def(env, "rtsp-req-pause", janet_wrap_integer(CURL_RTSPREQ_PAUSE), "//TODO: no description");
    janet_def(env, "rtsp-req-teardown", janet_wrap_integer(CURL_RTSPREQ_TEARDOWN), "//TODO: no description");
    janet_def(env, "rtsp-req-get-parameter", janet_wrap_integer(CURL_RTSPREQ_GET_PARAMETER), "//TODO: no description");
    janet_def(env, "rtsp-req-set-parameter", janet_wrap_integer(CURL_RTSPREQ_SET_PARAMETER), "//TODO: no description");
    janet_def(env, "rtsp-req-record", janet_wrap_integer(CURL_RTSPREQ_RECORD), "//TODO: no description");
    janet_def(env, "rtsp-req-receive", janet_wrap_integer(CURL_RTSPREQ_RECEIVE), "//TODO: no description");

    // CURLOPT_GSSAPI_DELEGATION
    janet_def(env, "gss-api-delegation-none", janet_wrap_integer(CURLGSSAPI_DELEGATION_NONE), "no delegation (default)");
    janet_def(env, "gss-api-delegation-policy-flag", janet_wrap_integer(CURLGSSAPI_DELEGATION_POLICY_FLAG), "if permitted by policy");
    janet_def(env, "gss-api-delegation-flag", janet_wrap_integer(CURLGSSAPI_DELEGATION_FLAG), "delegate always");

    // CURLOPT_PROXY_SSL_VERIFYHOST
    janet_def(env, "proxy-ssl-verifyhost-no", janet_wrap_integer(0), "do not verify");
    janet_def(env, "proxy-ssl-verifyhost-yes", janet_wrap_integer(2), "do not verify");

    // CURLOPT_PROXY_SSLVERSION
    janet_def(env, "ssl-version-default", janet_wrap_integer(CURL_SSLVERSION_DEFAULT), "default");
    janet_def(env, "ssl-version-tls-1", janet_wrap_integer(CURL_SSLVERSION_TLSv1), "tls 1");
    janet_def(env, "ssl-version-ssl-2", janet_wrap_integer(CURL_SSLVERSION_SSLv2), "ssl 2");
    janet_def(env, "ssl-version-ssl-3", janet_wrap_integer(CURL_SSLVERSION_SSLv3), "ssl 3");
    janet_def(env, "ssl-version-tls-1.0", janet_wrap_integer(CURL_SSLVERSION_TLSv1_0), "tls 1.0");
    janet_def(env, "ssl-version-tls-1.1", janet_wrap_integer(CURL_SSLVERSION_TLSv1_1), "tls 1.1");
    janet_def(env, "ssl-version-tls-1.2", janet_wrap_integer(CURL_SSLVERSION_TLSv1_2), "tls 1.2");
    janet_def(env, "ssl-version-tls-1.3", janet_wrap_integer(CURL_SSLVERSION_TLSv1_3), "tls 1.3");

    // CURLOPT_NETRC
    janet_def(env, "netrc-ignored", janet_wrap_integer(CURL_NETRC_IGNORED), "ignored");
    janet_def(env, "netrc-optional", janet_wrap_integer(CURL_NETRC_OPTIONAL), "optional");
    janet_def(env, "netrc-required", janet_wrap_integer(CURL_NETRC_REQUIRED), "required");


    janet_def(env, "url-part-url", janet_wrap_integer(CURLUPART_URL), "url part of url");
    janet_def(env, "url-part-scheme", janet_wrap_integer(CURLUPART_SCHEME), "scheme part of url");
    janet_def(env, "url-part-user", janet_wrap_integer(CURLUPART_USER), "user part of url");
    janet_def(env, "url-part-password", janet_wrap_integer(CURLUPART_PASSWORD), "password part of url");
    janet_def(env, "url-part-options", janet_wrap_integer(CURLUPART_OPTIONS), "options part of url");
    janet_def(env, "url-part-host", janet_wrap_integer(CURLUPART_HOST), "host part of url");
    janet_def(env, "url-part-port", janet_wrap_integer(CURLUPART_PORT), "port part of url");
    janet_def(env, "url-part-path", janet_wrap_integer(CURLUPART_PATH), "path part of url");
    janet_def(env, "url-part-query", janet_wrap_integer(CURLUPART_QUERY), "query part of url");
    janet_def(env, "url-part-fragment", janet_wrap_integer(CURLUPART_FRAGMENT), "fragment part of url");

    // QUERY
    janet_def(env, "query-none", janet_wrap_integer(CURLINFO_NONE), "query none");
    janet_def(env, "query-effective-url", janet_wrap_integer(CURLINFO_EFFECTIVE_URL), "query effective-url");
    janet_def(env, "query-response-code", janet_wrap_integer(CURLINFO_RESPONSE_CODE), "query response-code");
    janet_def(env, "query-total-time", janet_wrap_integer(CURLINFO_TOTAL_TIME), "query total-time");
    janet_def(env, "query-name-lookup-time", janet_wrap_integer(CURLINFO_NAMELOOKUP_TIME), "query namelookup-time");
    janet_def(env, "query-connect-time", janet_wrap_integer(CURLINFO_CONNECT_TIME), "query connect-time");
    janet_def(env, "query-pre-transfer-time", janet_wrap_integer(CURLINFO_PRETRANSFER_TIME), "query pretransfer-time");
    janet_def(env, "query-size-upload", janet_wrap_integer(CURLINFO_SIZE_UPLOAD), "query size-upload");
    janet_def(env, "query-size-upload-t", janet_wrap_integer(CURLINFO_SIZE_UPLOAD_T), "query size-upload-t");
    janet_def(env, "query-size-download", janet_wrap_integer(CURLINFO_SIZE_DOWNLOAD), "query size-download");
    janet_def(env, "query-size-download-t", janet_wrap_integer(CURLINFO_SIZE_DOWNLOAD_T), "query size-download-t");
    janet_def(env, "query-speed-download", janet_wrap_integer(CURLINFO_SPEED_DOWNLOAD), "query speed-download");
    janet_def(env, "query-speed-download-t", janet_wrap_integer(CURLINFO_SPEED_DOWNLOAD_T), "query speed-download-t");
    janet_def(env, "query-speed-upload", janet_wrap_integer(CURLINFO_SPEED_UPLOAD), "query speed-upload");
    janet_def(env, "query-speed-upload-t", janet_wrap_integer(CURLINFO_SPEED_UPLOAD_T), "query speed-upload-t");
    janet_def(env, "query-header-size", janet_wrap_integer(CURLINFO_HEADER_SIZE), "query header-size");
    janet_def(env, "query-request-size", janet_wrap_integer(CURLINFO_REQUEST_SIZE), "query request-size");
    janet_def(env, "query-ssl-verify-result", janet_wrap_integer(CURLINFO_SSL_VERIFYRESULT), "query ssl-verifyresult");
    janet_def(env, "query-file-time", janet_wrap_integer(CURLINFO_FILETIME), "query filetime");
    janet_def(env, "query-file-time-t", janet_wrap_integer(CURLINFO_FILETIME_T), "query filetime-t");
    janet_def(env, "query-content-length-download", janet_wrap_integer(CURLINFO_CONTENT_LENGTH_DOWNLOAD), "query content-length-download");
    janet_def(env, "query-content-length-download-t", janet_wrap_integer(CURLINFO_CONTENT_LENGTH_DOWNLOAD_T), "query content-length-download-t");
    janet_def(env, "query-content-length-upload", janet_wrap_integer(CURLINFO_CONTENT_LENGTH_UPLOAD), "query content-length-upload");
    janet_def(env, "query-content-length-upload-t", janet_wrap_integer(CURLINFO_CONTENT_LENGTH_UPLOAD_T), "query content-length-upload-t");
    janet_def(env, "query-start-transfer-time", janet_wrap_integer(CURLINFO_STARTTRANSFER_TIME), "query starttransfer-time");
    janet_def(env, "query-content-type", janet_wrap_integer(CURLINFO_CONTENT_TYPE), "query content-type");
    janet_def(env, "query-redirect-time", janet_wrap_integer(CURLINFO_REDIRECT_TIME), "query redirect-time");
    janet_def(env, "query-redirect-count", janet_wrap_integer(CURLINFO_REDIRECT_COUNT), "query redirect-count");
    janet_def(env, "query-private", janet_wrap_integer(CURLINFO_PRIVATE), "query private");
    janet_def(env, "query-http-connect-code", janet_wrap_integer(CURLINFO_HTTP_CONNECTCODE), "query http-connectcode");
    janet_def(env, "query-http-auth-avail", janet_wrap_integer(CURLINFO_HTTPAUTH_AVAIL), "query httpauth-avail");
    janet_def(env, "query-proxy-auth-avail", janet_wrap_integer(CURLINFO_PROXYAUTH_AVAIL), "query proxyauth-avail");
    janet_def(env, "query-os-errno", janet_wrap_integer(CURLINFO_OS_ERRNO), "query os-errno");
    janet_def(env, "query-num-connects", janet_wrap_integer(CURLINFO_NUM_CONNECTS), "query num-connects");
    janet_def(env, "query-ssl-engines", janet_wrap_integer(CURLINFO_SSL_ENGINES), "query ssl-engines");
    janet_def(env, "query-cookie-list", janet_wrap_integer(CURLINFO_COOKIELIST), "query cookielist");
    janet_def(env, "query-last-socket", janet_wrap_integer(CURLINFO_LASTSOCKET), "query lastsocket");
    janet_def(env, "query-ftp-entry-path", janet_wrap_integer(CURLINFO_FTP_ENTRY_PATH), "query ftp-entry-path");
    janet_def(env, "query-redirect-url", janet_wrap_integer(CURLINFO_REDIRECT_URL), "query redirect-url");
    janet_def(env, "query-primary-ip", janet_wrap_integer(CURLINFO_PRIMARY_IP), "query primary-ip");
    janet_def(env, "query-app-connect-time", janet_wrap_integer(CURLINFO_APPCONNECT_TIME), "query appconnect-time");
    janet_def(env, "query-cert-info", janet_wrap_integer(CURLINFO_CERTINFO), "query certinfo");
    janet_def(env, "query-condition-unmet", janet_wrap_integer(CURLINFO_CONDITION_UNMET), "query condition-unmet");
    janet_def(env, "query-rtsp-session-id", janet_wrap_integer(CURLINFO_RTSP_SESSION_ID), "query rtsp-session-id");
    janet_def(env, "query-rtsp-client-cseq", janet_wrap_integer(CURLINFO_RTSP_CLIENT_CSEQ), "query rtsp-client-cseq");
    janet_def(env, "query-rtsp-server-cseq", janet_wrap_integer(CURLINFO_RTSP_SERVER_CSEQ), "query rtsp-server-cseq");
    janet_def(env, "query-rtsp-cseq-recv", janet_wrap_integer(CURLINFO_RTSP_CSEQ_RECV), "query rtsp-cseq-recv");
    janet_def(env, "query-primary-port", janet_wrap_integer(CURLINFO_PRIMARY_PORT), "query primary-port");
    janet_def(env, "query-local-ip", janet_wrap_integer(CURLINFO_LOCAL_IP), "query local-ip");
    janet_def(env, "query-local-port", janet_wrap_integer(CURLINFO_LOCAL_PORT), "query local-port");
    janet_def(env, "query-tls-session", janet_wrap_integer(CURLINFO_TLS_SESSION), "query tls-session");
    janet_def(env, "query-active-socket", janet_wrap_integer(CURLINFO_ACTIVESOCKET), "query activesocket");
    janet_def(env, "query-tls-ssl-ptr", janet_wrap_integer(CURLINFO_TLS_SSL_PTR), "query tls-ssl-ptr");
    janet_def(env, "query-http-version", janet_wrap_integer(CURLINFO_HTTP_VERSION), "query http-version");
    janet_def(env, "query-proxy-ssl-verify-result", janet_wrap_integer(CURLINFO_PROXY_SSL_VERIFYRESULT), "query proxy-ssl-verifyresult");
    janet_def(env, "query-protocol", janet_wrap_integer(CURLINFO_PROTOCOL), "query protocol");
    janet_def(env, "query-scheme", janet_wrap_integer(CURLINFO_SCHEME), "query scheme");
    janet_def(env, "query-total-time-t", janet_wrap_integer(CURLINFO_TOTAL_TIME_T), "query total-time-t");
    janet_def(env, "query-name-lookup-time-t", janet_wrap_integer(CURLINFO_NAMELOOKUP_TIME_T), "query namelookup-time-t");
    janet_def(env, "query-connect-time-t", janet_wrap_integer(CURLINFO_CONNECT_TIME_T), "query connect-time-t");
    janet_def(env, "query-pre-transfer-time-t", janet_wrap_integer(CURLINFO_PRETRANSFER_TIME_T), "query pretransfer-time-t");
    janet_def(env, "query-start-transfer-time-t", janet_wrap_integer(CURLINFO_STARTTRANSFER_TIME_T), "query starttransfer-time-t");
    janet_def(env, "query-redirect-time-t", janet_wrap_integer(CURLINFO_REDIRECT_TIME_T), "query redirect-time-t");
    janet_def(env, "query-app-connect-time-t", janet_wrap_integer(CURLINFO_APPCONNECT_TIME_T), "query appconnect-time-t");

    // .:MASK:.
    // CURLOPT_SSH_AUTH_TYPES
    janet_def(env, "ssh-auth-any", janet_wrap_integer(CURLSSH_AUTH_ANY), "any (bitmask)");
    janet_def(env, "ssh-auth-none", janet_wrap_integer(CURLSSH_AUTH_NONE), "none (bitmask)");
    janet_def(env, "ssh-auth-public-key", janet_wrap_integer(CURLSSH_AUTH_PUBLICKEY), "public key (bitmask)");
    janet_def(env, "ssh-auth-password", janet_wrap_integer(CURLSSH_AUTH_PASSWORD), "password (bitmask)");
    janet_def(env, "ssh-auth-host", janet_wrap_integer(CURLSSH_AUTH_HOST), "host (bitmask)");
    janet_def(env, "ssh-auth-keyboard", janet_wrap_integer(CURLSSH_AUTH_KEYBOARD), "keyboard (bitmask)");
    janet_def(env, "ssh-auth-agent", janet_wrap_integer(CURLSSH_AUTH_AGENT), "agent (bitmask)");
    janet_def(env, "ssh-auth-gss-api", janet_wrap_integer(CURLSSH_AUTH_GSSAPI), "gss api (bitmask)");
    janet_def(env, "ssh-auth-default", janet_wrap_integer(CURLSSH_AUTH_DEFAULT), "default (bitmask)");

    // CURLOPT_POSTREDIR
    janet_def(env, "redir-get-all", janet_wrap_integer(CURL_REDIR_GET_ALL), "get all (bitmask)");
    janet_def(env, "redir-post-301", janet_wrap_integer(CURL_REDIR_POST_301), "post 301 (bitmask)");
    janet_def(env, "redir-post-302", janet_wrap_integer(CURL_REDIR_POST_302), "post 302 (bitmask)");
    janet_def(env, "redir-post-303", janet_wrap_integer(CURL_REDIR_POST_303), "post 303 (bitmask)");
    janet_def(env, "redir-post-all", janet_wrap_integer(CURL_REDIR_POST_ALL), "post all (bitmask)");

    // CURLOPT_PROTOCOLS
    janet_def(env, "proto-http", janet_wrap_integer(CURLPROTO_HTTP), "http (bitmask)");
    janet_def(env, "proto-https", janet_wrap_integer(CURLPROTO_HTTPS), "https (bitmask)");
    janet_def(env, "proto-ftp", janet_wrap_integer(CURLPROTO_FTP), "ftp (bitmask)");
    janet_def(env, "proto-ftps", janet_wrap_integer(CURLPROTO_FTPS), "ftps (bitmask)");
    janet_def(env, "proto-scp", janet_wrap_integer(CURLPROTO_SCP), "scp (bitmask)");
    janet_def(env, "proto-sftp", janet_wrap_integer(CURLPROTO_SFTP), "sftp (bitmask)");
    janet_def(env, "proto-telnet", janet_wrap_integer(CURLPROTO_TELNET), "telnet (bitmask)");
    janet_def(env, "proto-ldap", janet_wrap_integer(CURLPROTO_LDAP), "ldap (bitmask)");
    janet_def(env, "proto-ldaps", janet_wrap_integer(CURLPROTO_LDAPS), "ldaps (bitmask)");
    janet_def(env, "proto-dict", janet_wrap_integer(CURLPROTO_DICT), "dict (bitmask)");
    janet_def(env, "proto-file", janet_wrap_integer(CURLPROTO_FILE), "file (bitmask)");
    janet_def(env, "proto-tftp", janet_wrap_integer(CURLPROTO_TFTP), "tftp (bitmask)");
    janet_def(env, "proto-imap", janet_wrap_integer(CURLPROTO_IMAP), "imap (bitmask)");
    janet_def(env, "proto-imaps", janet_wrap_integer(CURLPROTO_IMAPS), "imaps (bitmask)");
    janet_def(env, "proto-pop3", janet_wrap_integer(CURLPROTO_POP3), "pop3 (bitmask)");
    janet_def(env, "proto-pop3s", janet_wrap_integer(CURLPROTO_POP3S), "pop3s (bitmask)");
    janet_def(env, "proto-smtp", janet_wrap_integer(CURLPROTO_SMTP), "smtp (bitmask)");
    janet_def(env, "proto-smtps", janet_wrap_integer(CURLPROTO_SMTPS), "smtps (bitmask)");
    janet_def(env, "proto-rtsp", janet_wrap_integer(CURLPROTO_RTSP), "rtsp (bitmask)");
    janet_def(env, "proto-rtmp", janet_wrap_integer(CURLPROTO_RTMP), "rtmp (bitmask)");
    janet_def(env, "proto-rtmpt", janet_wrap_integer(CURLPROTO_RTMPT), "rtmpt (bitmask)");
    janet_def(env, "proto-rtmpe", janet_wrap_integer(CURLPROTO_RTMPE), "rtmpe (bitmask)");
    janet_def(env, "proto-rtmpte", janet_wrap_integer(CURLPROTO_RTMPTE), "rtmpte (bitmask)");
    janet_def(env, "proto-rtmps", janet_wrap_integer(CURLPROTO_RTMPS), "rtmps (bitmask)");
    janet_def(env, "proto-rtmpts", janet_wrap_integer(CURLPROTO_RTMPTS), "rtmpts (bitmask)");
    janet_def(env, "proto-gopher", janet_wrap_integer(CURLPROTO_GOPHER), "gopher (bitmask)");
    janet_def(env, "proto-smb", janet_wrap_integer(CURLPROTO_SMB), "smb (bitmask)");
    janet_def(env, "proto-smbs", janet_wrap_integer(CURLPROTO_SMBS), "smbs (bitmask)");
    janet_def(env, "proto-all", janet_wrap_integer(CURLPROTO_ALL), "all (bitmask)");

    // CURLOPT_REDIR_PROTOCOLS
    janet_def(env, "re-dir-proto-http", janet_wrap_integer(CURLPROTO_HTTP), "http (bitmask)");
    janet_def(env, "re-dir-proto-https", janet_wrap_integer(CURLPROTO_HTTPS), "https (bitmask)");
    janet_def(env, "re-dir-proto-ftp", janet_wrap_integer(CURLPROTO_FTP), "ftp (bitmask)");
    janet_def(env, "re-dir-proto-ftps", janet_wrap_integer(CURLPROTO_FTPS), "ftps (bitmask)");
    janet_def(env, "re-dir-proto-scp", janet_wrap_integer(CURLPROTO_SCP), "scp (bitmask)");
    janet_def(env, "re-dir-proto-sftp", janet_wrap_integer(CURLPROTO_SFTP), "sftp (bitmask)");
    janet_def(env, "re-dir-proto-telnet", janet_wrap_integer(CURLPROTO_TELNET), "telnet (bitmask)");
    janet_def(env, "re-dir-proto-ldap", janet_wrap_integer(CURLPROTO_LDAP), "ldap (bitmask)");
    janet_def(env, "re-dir-proto-ldaps", janet_wrap_integer(CURLPROTO_LDAPS), "ldaps (bitmask)");
    janet_def(env, "re-dir-proto-dict", janet_wrap_integer(CURLPROTO_DICT), "dict (bitmask)");
    janet_def(env, "re-dir-proto-file", janet_wrap_integer(CURLPROTO_FILE), "file (bitmask)");
    janet_def(env, "re-dir-proto-tftp", janet_wrap_integer(CURLPROTO_TFTP), "tftp (bitmask)");
    janet_def(env, "re-dir-proto-imap", janet_wrap_integer(CURLPROTO_IMAP), "imap (bitmask)");
    janet_def(env, "re-dir-proto-imaps", janet_wrap_integer(CURLPROTO_IMAPS), "imaps (bitmask)");
    janet_def(env, "re-dir-proto-pop3", janet_wrap_integer(CURLPROTO_POP3), "pop3 (bitmask)");
    janet_def(env, "re-dir-proto-pop3s", janet_wrap_integer(CURLPROTO_POP3S), "pop3s (bitmask)");
    janet_def(env, "re-dir-proto-smtp", janet_wrap_integer(CURLPROTO_SMTP), "smtp (bitmask)");
    janet_def(env, "re-dir-proto-smtps", janet_wrap_integer(CURLPROTO_SMTPS), "smtps (bitmask)");
    janet_def(env, "re-dir-proto-rtsp", janet_wrap_integer(CURLPROTO_RTSP), "rtsp (bitmask)");
    janet_def(env, "re-dir-proto-rtmp", janet_wrap_integer(CURLPROTO_RTMP), "rtmp (bitmask)");
    janet_def(env, "re-dir-proto-rtmpt", janet_wrap_integer(CURLPROTO_RTMPT), "rtmpt (bitmask)");
    janet_def(env, "re-dir-proto-rtmpe", janet_wrap_integer(CURLPROTO_RTMPE), "rtmpe (bitmask)");
    janet_def(env, "re-dir-proto-rtmpte", janet_wrap_integer(CURLPROTO_RTMPTE), "rtmpte (bitmask)");
    janet_def(env, "re-dir-proto-rtmps", janet_wrap_integer(CURLPROTO_RTMPS), "rtmps (bitmask)");
    janet_def(env, "re-dir-proto-rtmpts", janet_wrap_integer(CURLPROTO_RTMPTS), "rtmpts (bitmask)");
    janet_def(env, "re-dir-proto-gopher", janet_wrap_integer(CURLPROTO_GOPHER), "gopher (bitmask)");
    janet_def(env, "re-dir-proto-smb", janet_wrap_integer(CURLPROTO_SMB), "smb (bitmask)");
    janet_def(env, "re-dir-proto-smbs", janet_wrap_integer(CURLPROTO_SMBS), "smbs (bitmask)");
    janet_def(env, "re-dir-proto-all", janet_wrap_integer(CURLPROTO_ALL), "all (bitmask)");

    // CURLOPT_SSL_OPTIONS
    janet_def(env, "ssl-opt-allow", janet_wrap_integer(CURLSSLOPT_ALLOW_BEAST), "allow beast (bitmask)");
    janet_def(env, "ssl-opt-no", janet_wrap_integer(CURLSSLOPT_NO_REVOKE), "no revoke (bitmask)");

    // CURLOPT_PROXY_SSL_OPTIONS
    janet_def(env, "proxy-ssl-opt-allow", janet_wrap_integer(CURLSSLOPT_ALLOW_BEAST), "allow beast (bitmask)");
    janet_def(env, "proxy-ssl-opt-no", janet_wrap_integer(CURLSSLOPT_NO_REVOKE), "no revoke (bitmask)");

    // CURLOPT_HEADEROPT
    janet_def(env, "header-opt-unified", janet_wrap_integer(CURLHEADER_UNIFIED), "unified (bitmask)");
    janet_def(env, "header-opt-separate", janet_wrap_integer(CURLHEADER_SEPARATE), "separate (bitmask)");

    // CURLOPT_HTTPAUTH
    janet_def(env, "http-auth-none", janet_wrap_integer(CURLAUTH_NONE), "no authentication");
    janet_def(env, "http-auth-basic", janet_wrap_integer(CURLAUTH_BASIC), "basic authentication type");
    janet_def(env, "http-auth-digest", janet_wrap_integer(CURLAUTH_DIGEST), "digest authentication type");
    janet_def(env, "http-auth-digest-ie", janet_wrap_integer(CURLAUTH_DIGEST_IE), "digest authentication type with IE flavor");
    janet_def(env, "http-auth-negotiate", janet_wrap_integer(CURLAUTH_NEGOTIATE), "negotiate authentication type");
    janet_def(env, "http-auth-gss-api", janet_wrap_integer(CURLAUTH_GSSAPI), "gss api authentication type");
    janet_def(env, "http-auth-ntlm", janet_wrap_integer(CURLAUTH_NTLM), "ntlm authentication type");
    janet_def(env, "http-auth-ntlm-wb", janet_wrap_integer(CURLAUTH_NTLM_WB), "ntlm wb authentication type");
    janet_def(env, "http-auth-bearer", janet_wrap_integer(CURLAUTH_BEARER), "bearer authentication type");
    janet_def(env, "http-auth-only", janet_wrap_integer(CURLAUTH_ONLY), "//TODO: how to handle this?");
    janet_def(env, "http-auth-any", janet_wrap_integer(CURLAUTH_ANY), "any authentication type");
    janet_def(env, "http-auth-any-safe", janet_wrap_integer(CURLAUTH_ANYSAFE), "any safe authentication type");
    janet_def(env, "http-auth-aws-sigv4", janet_wrap_integer(CURLAUTH_AWS_SIGV4), "aws sigv4 authentication type");

    // CURLOPT_PROXYAUTH
    janet_def(env, "proxy-none", janet_wrap_integer(CURLAUTH_NONE), "none");
    janet_def(env, "proxy-basic", janet_wrap_integer(CURLAUTH_BASIC), "basic");
    janet_def(env, "proxy-digest", janet_wrap_integer(CURLAUTH_DIGEST), "digest");
    janet_def(env, "proxy-negotiate", janet_wrap_integer(CURLAUTH_NEGOTIATE), "negotiate");
    janet_def(env, "proxy-gssapi", janet_wrap_integer(CURLAUTH_GSSAPI), "gssapi");
    janet_def(env, "proxy-ntlm", janet_wrap_integer(CURLAUTH_NTLM), "ntlm");
    janet_def(env, "proxy-digest-ie", janet_wrap_integer(CURLAUTH_DIGEST_IE), "digest-ie");
    janet_def(env, "proxy-ntlm-wb", janet_wrap_integer(CURLAUTH_NTLM_WB), "ntlm-wb");
    janet_def(env, "proxy-bearer", janet_wrap_integer(CURLAUTH_BEARER), "bearer");
    janet_def(env, "proxy-only", janet_wrap_integer(CURLAUTH_ONLY), "only");
    janet_def(env, "proxy-any", janet_wrap_integer(CURLAUTH_ANY), "any");
    janet_def(env, "proxy-anysafe", janet_wrap_integer(CURLAUTH_ANYSAFE), "anysafe");

    // CURLOPT_SOCKS5_AUTH
    janet_def(env, "sock5-auth-none", janet_wrap_integer(CURLAUTH_NONE), "no authentication (bitmask)");
    janet_def(env, "sock5-auth-basic", janet_wrap_integer(CURLAUTH_BASIC), "basic authentication type (bitmask)");
    janet_def(env, "sock5-auth-digest", janet_wrap_integer(CURLAUTH_DIGEST), "digest authentication type (bitmask)");
    janet_def(env, "sock5-auth-digest-ie", janet_wrap_integer(CURLAUTH_DIGEST_IE), "digest authentication type with IE flavor (bitmask)");
    janet_def(env, "sock5-auth-negotiate", janet_wrap_integer(CURLAUTH_NEGOTIATE), "negotiate authentication type (bitmask)");
    janet_def(env, "sock5-auth-gss-api", janet_wrap_integer(CURLAUTH_GSSAPI), "gss api authentication type (bitmask)");
    janet_def(env, "sock5-auth-ntlm", janet_wrap_integer(CURLAUTH_NTLM), "ntlm authentication type (bitmask)");
    janet_def(env, "sock5-auth-ntlm-wb", janet_wrap_integer(CURLAUTH_NTLM_WB), "ntlm wb authentication type (bitmask)");
    janet_def(env, "sock5-auth-bearer", janet_wrap_integer(CURLAUTH_BEARER), "bearer authentication type (bitmask)");
    janet_def(env, "sock5-auth-only", janet_wrap_integer(CURLAUTH_ONLY), "//TODO: how to handle this? (bitmask)");
    janet_def(env, "sock5-auth-any", janet_wrap_integer(CURLAUTH_ANY), "any authentication type (bitmask)");
    janet_def(env, "sock5-auth-any-safe", janet_wrap_integer(CURLAUTH_ANYSAFE), "any safe authentication type (bitmask)");

    // CURLU_EXTRACT_FLAGS
    janet_def(env, "url-extract-flag-default-port", janet_wrap_integer(CURLU_DEFAULT_PORT), "default port (bitmask)");
    janet_def(env, "url-extract-flag-no-default-port", janet_wrap_integer(CURLU_NO_DEFAULT_PORT), "no default port (bitmask)");
    janet_def(env, "url-extract-flag-default-scheme", janet_wrap_integer(CURLU_DEFAULT_SCHEME), "default scheme (bitmask)");
    janet_def(env, "url-extract-flag-non-support-scheme", janet_wrap_integer(CURLU_NON_SUPPORT_SCHEME), "non support scheme (bitmask)");
    janet_def(env, "url-extract-flag-path-as-is", janet_wrap_integer(CURLU_PATH_AS_IS), "path as is (bitmask)");
    janet_def(env, "url-extract-flag-disallow-user", janet_wrap_integer(CURLU_DISALLOW_USER), "disallow user (bitmask)");
    janet_def(env, "url-extract-flag-url-decode", janet_wrap_integer(CURLU_URLDECODE), "url decode (bitmask)");
    janet_def(env, "url-extract-flag-url-encode", janet_wrap_integer(CURLU_URLENCODE), "url encode (bitmask)");
    janet_def(env, "url-extract-flag-append-query", janet_wrap_integer(CURLU_APPENDQUERY), "append query (bitmask)");
    janet_def(env, "url-extract-flag-guess-scheme", janet_wrap_integer(CURLU_GUESS_SCHEME), "guess scheme (bitmask)");

    // submodules
    submod_easy(env);
    submod_share(env);
    submod_url(env);
    submod_mime(env);
}
