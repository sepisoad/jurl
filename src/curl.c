#include "curl.h"

JANET_MODULE_ENTRY(JanetTable *env) {
  opts_gen_dict();

  // GENERAL
  janet_def(env, "version", janet_cstringv(LIBCURL_VERSION),"underlying libcurl value string");

  // .:ENUMS:.
  // CURLOPT_PROXYTYPE
  janet_def(env, "proxy-type/http", janet_wrap_integer(CURLPROXY_HTTP), "http proxy type");
  janet_def(env, "proxy-type/https", janet_wrap_integer(CURLPROXY_HTTPS), "https proxy type");
  janet_def(env, "proxy-type/http-1.0", janet_wrap_integer(CURLPROXY_HTTP_1_0), "http 1.0 proxy type");
  janet_def(env, "proxy-type/socks4", janet_wrap_integer(CURLPROXY_SOCKS4), "socks4 proxy type");
  janet_def(env, "proxy-type/socks4a", janet_wrap_integer(CURLPROXY_SOCKS4A), "socks4a proxy type");
  janet_def(env, "proxy-type/socks5", janet_wrap_integer(CURLPROXY_SOCKS5), "socks5 proxy type");
  janet_def(env, "proxy-type/socks5-hostname", janet_wrap_integer(CURLPROXY_SOCKS5_HOSTNAME), "socks5 resolver proxy type");

  // CURLOPT_FTP_CREATE_MISSING_DIRS  
  janet_def(env, "ftp-create-missing-dir/no", janet_wrap_integer(CURLFTP_CREATE_DIR_NONE), "do not create missing dirs");
  janet_def(env, "ftp-create-missing-dir/yes", janet_wrap_integer(CURLFTP_CREATE_DIR), "create missing dirs");
  janet_def(env, "ftp-create-missing-dir/retry", janet_wrap_integer(CURLFTP_CREATE_DIR_RETRY), "retry create missing dirs");  

  // CURLOPT_IPRESOLVE  
  janet_def(env, "ip-resolve/whatever", janet_wrap_integer(CURL_IPRESOLVE_WHATEVER), "whatever");
  janet_def(env, "ip-resolve/v4", janet_wrap_integer(CURL_IPRESOLVE_V4), "v4");
  janet_def(env, "ip-resolve/v6", janet_wrap_integer(CURL_IPRESOLVE_V6), "v6");

  // CURLOPT_USE_SSL  
  janet_def(env, "use-ssl/none", janet_wrap_integer(CURLUSESSL_NONE), "do not attempt to use ssl");
  janet_def(env, "use-ssl/try", janet_wrap_integer(CURLUSESSL_TRY), "try using ssl, proceed anyway otherwise");
  janet_def(env, "use-ssl/control", janet_wrap_integer(CURLUSESSL_CONTROL), "ssl for the control connection or fail");
  janet_def(env, "use-ssl/all", janet_wrap_integer(CURLUSESSL_ALL), "ssl for all communication or fail");

  // CURLOPT_FTPSSLAUTH  
  janet_def(env, "ftp-auth/default", janet_wrap_integer(CURLFTPAUTH_DEFAULT), "let libcurl decide");
  janet_def(env, "ftp-auth/ssl", janet_wrap_integer(CURLFTPAUTH_SSL), "use auth ssl");
  janet_def(env, "ftp-auth/tls", janet_wrap_integer(CURLFTPAUTH_TLS), "use auth tls");

  // CURLOPT_FTP_FILEMETHOD
  janet_def(env, "ftp-method/default", janet_wrap_integer(CURLFTPMETHOD_DEFAULT), "let libcurl pick");
  janet_def(env, "ftp-method/multicwd", janet_wrap_integer(CURLFTPMETHOD_MULTICWD), "single cwd operation for each path part");
  janet_def(env, "ftp-method/nocwd", janet_wrap_integer(CURLFTPMETHOD_NOCWD), "no cwd at all");
  janet_def(env, "ftp-method/singlecwd", janet_wrap_integer(CURLFTPMETHOD_SINGLECWD), "one cwd to full dir, then work on file");

  // CURLOPT_FTP_SSL_CCC
  janet_def(env, "ftp-ssl-ccc/none", janet_wrap_integer(CURLFTPSSL_CCC_NONE), "do not send ccc");
  janet_def(env, "ftp-ssl-ccc/passive", janet_wrap_integer(CURLFTPSSL_CCC_PASSIVE), "Let the server initiate the shutdown");
  janet_def(env, "ftp-ssl-ccc/active", janet_wrap_integer(CURLFTPSSL_CCC_ACTIVE), "Initiate the shutdown");

  // CURLOPT_RTSP_REQUEST
  janet_def(env, "rtsp-req/none", janet_wrap_integer(CURL_RTSPREQ_NONE), "//TODO: no description");
  janet_def(env, "rtsp-req/options", janet_wrap_integer(CURL_RTSPREQ_OPTIONS), "//TODO: no description");
  janet_def(env, "rtsp-req/describe", janet_wrap_integer(CURL_RTSPREQ_DESCRIBE), "//TODO: no description");
  janet_def(env, "rtsp-req/announce", janet_wrap_integer(CURL_RTSPREQ_ANNOUNCE), "//TODO: no description");
  janet_def(env, "rtsp-req/setup", janet_wrap_integer(CURL_RTSPREQ_SETUP), "//TODO: no description");
  janet_def(env, "rtsp-req/play", janet_wrap_integer(CURL_RTSPREQ_PLAY), "//TODO: no description");
  janet_def(env, "rtsp-req/pause", janet_wrap_integer(CURL_RTSPREQ_PAUSE), "//TODO: no description");
  janet_def(env, "rtsp-req/teardown", janet_wrap_integer(CURL_RTSPREQ_TEARDOWN), "//TODO: no description");
  janet_def(env, "rtsp-req/get-parameter", janet_wrap_integer(CURL_RTSPREQ_GET_PARAMETER), "//TODO: no description");
  janet_def(env, "rtsp-req/set-parameter", janet_wrap_integer(CURL_RTSPREQ_SET_PARAMETER), "//TODO: no description");
  janet_def(env, "rtsp-req/record", janet_wrap_integer(CURL_RTSPREQ_RECORD), "//TODO: no description");
  janet_def(env, "rtsp-req/receive", janet_wrap_integer(CURL_RTSPREQ_RECEIVE), "//TODO: no description");

  // CURLOPT_GSSAPI_DELEGATION
  janet_def(env, "gss-api-delegation/none", janet_wrap_integer(CURLGSSAPI_DELEGATION_NONE), "no delegation (default)");
  janet_def(env, "gss-api-delegation/policy-flag", janet_wrap_integer(CURLGSSAPI_DELEGATION_POLICY_FLAG), "if permitted by policy");
  janet_def(env, "gss-api-delegation/flag", janet_wrap_integer(CURLGSSAPI_DELEGATION_FLAG), "delegate always");

  // CURLOPT_PROXY_SSL_VERIFYHOST
  janet_def(env, "proxy-ssl-verifyhost/no", janet_wrap_integer(0), "do not verify");  
  janet_def(env, "proxy-ssl-verifyhost/yes", janet_wrap_integer(2), "do not verify");

  // CURLOPT_PROXY_SSLVERSION
  janet_def(env, "ssl-version/default", janet_wrap_integer(CURL_SSLVERSION_DEFAULT), "default");
  janet_def(env, "ssl-version/tls-1", janet_wrap_integer(CURL_SSLVERSION_TLSv1), "tls 1");
  janet_def(env, "ssl-version/ssl-2", janet_wrap_integer(CURL_SSLVERSION_SSLv2), "ssl 2");
  janet_def(env, "ssl-version/ssl-3", janet_wrap_integer(CURL_SSLVERSION_SSLv3), "ssl 3");
  janet_def(env, "ssl-version/tls-1.0", janet_wrap_integer(CURL_SSLVERSION_TLSv1_0), "tls 1.0");
  janet_def(env, "ssl-version/tls-1.1", janet_wrap_integer(CURL_SSLVERSION_TLSv1_1), "tls 1.1");
  janet_def(env, "ssl-version/tls-1.2", janet_wrap_integer(CURL_SSLVERSION_TLSv1_2), "tls 1.2");
  janet_def(env, "ssl-version/tls-1.3", janet_wrap_integer(CURL_SSLVERSION_TLSv1_3), "tls 1.3");

  // CURLOPT_NETRC  
  janet_def(env, "netrc/ignored", janet_wrap_integer(CURL_NETRC_IGNORED), "ignored");
  janet_def(env, "netrc/optional", janet_wrap_integer(CURL_NETRC_OPTIONAL), "optional");
  janet_def(env, "netrc/required", janet_wrap_integer(CURL_NETRC_REQUIRED), "required");

  // .:MASK:.
  // CURLOPT_SSH_AUTH_TYPES  
  janet_def(env, "ssh-auth/any", janet_wrap_integer(CURLSSH_AUTH_ANY), "any (bitmask)");
  janet_def(env, "ssh-auth/none", janet_wrap_integer(CURLSSH_AUTH_NONE), "none (bitmask)");
  janet_def(env, "ssh-auth/public-key", janet_wrap_integer(CURLSSH_AUTH_PUBLICKEY), "public key (bitmask)");
  janet_def(env, "ssh-auth/password", janet_wrap_integer(CURLSSH_AUTH_PASSWORD), "password (bitmask)");
  janet_def(env, "ssh-auth/host", janet_wrap_integer(CURLSSH_AUTH_HOST), "host (bitmask)");
  janet_def(env, "ssh-auth/keyboard", janet_wrap_integer(CURLSSH_AUTH_KEYBOARD), "keyboard (bitmask)");
  janet_def(env, "ssh-auth/agent", janet_wrap_integer(CURLSSH_AUTH_AGENT), "agent (bitmask)");
  janet_def(env, "ssh-auth/gss-api", janet_wrap_integer(CURLSSH_AUTH_GSSAPI), "gss api (bitmask)");
  janet_def(env, "ssh-auth/default", janet_wrap_integer(CURLSSH_AUTH_DEFAULT), "default (bitmask)");

  // CURLOPT_POSTREDIR
  janet_def(env, "redir/get-all", janet_wrap_integer(CURL_REDIR_GET_ALL), "get all (bitmask)");
  janet_def(env, "redir/post-301", janet_wrap_integer(CURL_REDIR_POST_301), "post 301 (bitmask)");
  janet_def(env, "redir/post-302", janet_wrap_integer(CURL_REDIR_POST_302), "post 302 (bitmask)");
  janet_def(env, "redir/post-303", janet_wrap_integer(CURL_REDIR_POST_303), "post 303 (bitmask)");
  janet_def(env, "redir/post-all", janet_wrap_integer(CURL_REDIR_POST_ALL), "post all (bitmask)");
  
  // CURLOPT_PROTOCOLS
  janet_def(env, "proto/http", janet_wrap_integer(CURLPROTO_HTTP), "http (bitmask)");
  janet_def(env, "proto/https", janet_wrap_integer(CURLPROTO_HTTPS), "https (bitmask)");
  janet_def(env, "proto/ftp", janet_wrap_integer(CURLPROTO_FTP), "ftp (bitmask)");
  janet_def(env, "proto/ftps", janet_wrap_integer(CURLPROTO_FTPS), "ftps (bitmask)");
  janet_def(env, "proto/scp", janet_wrap_integer(CURLPROTO_SCP), "scp (bitmask)");
  janet_def(env, "proto/sftp", janet_wrap_integer(CURLPROTO_SFTP), "sftp (bitmask)");
  janet_def(env, "proto/telnet", janet_wrap_integer(CURLPROTO_TELNET), "telnet (bitmask)");
  janet_def(env, "proto/ldap", janet_wrap_integer(CURLPROTO_LDAP), "ldap (bitmask)");
  janet_def(env, "proto/ldaps", janet_wrap_integer(CURLPROTO_LDAPS), "ldaps (bitmask)");
  janet_def(env, "proto/dict", janet_wrap_integer(CURLPROTO_DICT), "dict (bitmask)");
  janet_def(env, "proto/file", janet_wrap_integer(CURLPROTO_FILE), "file (bitmask)");
  janet_def(env, "proto/tftp", janet_wrap_integer(CURLPROTO_TFTP), "tftp (bitmask)");
  janet_def(env, "proto/imap", janet_wrap_integer(CURLPROTO_IMAP), "imap (bitmask)");
  janet_def(env, "proto/imaps", janet_wrap_integer(CURLPROTO_IMAPS), "imaps (bitmask)");
  janet_def(env, "proto/pop3", janet_wrap_integer(CURLPROTO_POP3), "pop3 (bitmask)");
  janet_def(env, "proto/pop3s", janet_wrap_integer(CURLPROTO_POP3S), "pop3s (bitmask)");
  janet_def(env, "proto/smtp", janet_wrap_integer(CURLPROTO_SMTP), "smtp (bitmask)");
  janet_def(env, "proto/smtps", janet_wrap_integer(CURLPROTO_SMTPS), "smtps (bitmask)");
  janet_def(env, "proto/rtsp", janet_wrap_integer(CURLPROTO_RTSP), "rtsp (bitmask)");
  janet_def(env, "proto/rtmp", janet_wrap_integer(CURLPROTO_RTMP), "rtmp (bitmask)");
  janet_def(env, "proto/rtmpt", janet_wrap_integer(CURLPROTO_RTMPT), "rtmpt (bitmask)");
  janet_def(env, "proto/rtmpe", janet_wrap_integer(CURLPROTO_RTMPE), "rtmpe (bitmask)");
  janet_def(env, "proto/rtmpte", janet_wrap_integer(CURLPROTO_RTMPTE), "rtmpte (bitmask)");
  janet_def(env, "proto/rtmps", janet_wrap_integer(CURLPROTO_RTMPS), "rtmps (bitmask)");
  janet_def(env, "proto/rtmpts", janet_wrap_integer(CURLPROTO_RTMPTS), "rtmpts (bitmask)");
  janet_def(env, "proto/gopher", janet_wrap_integer(CURLPROTO_GOPHER), "gopher (bitmask)");
  janet_def(env, "proto/smb", janet_wrap_integer(CURLPROTO_SMB), "smb (bitmask)");
  janet_def(env, "proto/smbs", janet_wrap_integer(CURLPROTO_SMBS), "smbs (bitmask)");
  janet_def(env, "proto/all", janet_wrap_integer(CURLPROTO_ALL), "all (bitmask)");

  // CURLOPT_REDIR_PROTOCOLS
  janet_def(env, "re-dir-proto/http", janet_wrap_integer(CURLPROTO_HTTP), "http (bitmask)");
  janet_def(env, "re-dir-proto/https", janet_wrap_integer(CURLPROTO_HTTPS), "https (bitmask)");
  janet_def(env, "re-dir-proto/ftp", janet_wrap_integer(CURLPROTO_FTP), "ftp (bitmask)");
  janet_def(env, "re-dir-proto/ftps", janet_wrap_integer(CURLPROTO_FTPS), "ftps (bitmask)");
  janet_def(env, "re-dir-proto/scp", janet_wrap_integer(CURLPROTO_SCP), "scp (bitmask)");
  janet_def(env, "re-dir-proto/sftp", janet_wrap_integer(CURLPROTO_SFTP), "sftp (bitmask)");
  janet_def(env, "re-dir-proto/telnet", janet_wrap_integer(CURLPROTO_TELNET), "telnet (bitmask)");
  janet_def(env, "re-dir-proto/ldap", janet_wrap_integer(CURLPROTO_LDAP), "ldap (bitmask)");
  janet_def(env, "re-dir-proto/ldaps", janet_wrap_integer(CURLPROTO_LDAPS), "ldaps (bitmask)");
  janet_def(env, "re-dir-proto/dict", janet_wrap_integer(CURLPROTO_DICT), "dict (bitmask)");
  janet_def(env, "re-dir-proto/file", janet_wrap_integer(CURLPROTO_FILE), "file (bitmask)");
  janet_def(env, "re-dir-proto/tftp", janet_wrap_integer(CURLPROTO_TFTP), "tftp (bitmask)");
  janet_def(env, "re-dir-proto/imap", janet_wrap_integer(CURLPROTO_IMAP), "imap (bitmask)");
  janet_def(env, "re-dir-proto/imaps", janet_wrap_integer(CURLPROTO_IMAPS), "imaps (bitmask)");
  janet_def(env, "re-dir-proto/pop3", janet_wrap_integer(CURLPROTO_POP3), "pop3 (bitmask)");
  janet_def(env, "re-dir-proto/pop3s", janet_wrap_integer(CURLPROTO_POP3S), "pop3s (bitmask)");
  janet_def(env, "re-dir-proto/smtp", janet_wrap_integer(CURLPROTO_SMTP), "smtp (bitmask)");
  janet_def(env, "re-dir-proto/smtps", janet_wrap_integer(CURLPROTO_SMTPS), "smtps (bitmask)");
  janet_def(env, "re-dir-proto/rtsp", janet_wrap_integer(CURLPROTO_RTSP), "rtsp (bitmask)");
  janet_def(env, "re-dir-proto/rtmp", janet_wrap_integer(CURLPROTO_RTMP), "rtmp (bitmask)");
  janet_def(env, "re-dir-proto/rtmpt", janet_wrap_integer(CURLPROTO_RTMPT), "rtmpt (bitmask)");
  janet_def(env, "re-dir-proto/rtmpe", janet_wrap_integer(CURLPROTO_RTMPE), "rtmpe (bitmask)");
  janet_def(env, "re-dir-proto/rtmpte", janet_wrap_integer(CURLPROTO_RTMPTE), "rtmpte (bitmask)");
  janet_def(env, "re-dir-proto/rtmps", janet_wrap_integer(CURLPROTO_RTMPS), "rtmps (bitmask)");
  janet_def(env, "re-dir-proto/rtmpts", janet_wrap_integer(CURLPROTO_RTMPTS), "rtmpts (bitmask)");
  janet_def(env, "re-dir-proto/gopher", janet_wrap_integer(CURLPROTO_GOPHER), "gopher (bitmask)");
  janet_def(env, "re-dir-proto/smb", janet_wrap_integer(CURLPROTO_SMB), "smb (bitmask)");
  janet_def(env, "re-dir-proto/smbs", janet_wrap_integer(CURLPROTO_SMBS), "smbs (bitmask)");
  janet_def(env, "re-dir-proto/all", janet_wrap_integer(CURLPROTO_ALL), "all (bitmask)");

  // CURLOPT_SSL_OPTIONS  
  janet_def(env, "ssl-opt/allow", janet_wrap_integer(CURLSSLOPT_ALLOW_BEAST), "allow beast (bitmask)");
  janet_def(env, "ssl-opt/no", janet_wrap_integer(CURLSSLOPT_NO_REVOKE), "no revoke (bitmask)");

  // CURLOPT_PROXY_SSL_OPTIONS
  janet_def(env, "proxy-ssl-opt/allow", janet_wrap_integer(CURLSSLOPT_ALLOW_BEAST), "allow beast (bitmask)");
  janet_def(env, "proxy-ssl-opt/no", janet_wrap_integer(CURLSSLOPT_NO_REVOKE), "no revoke (bitmask)");
  
  // CURLOPT_HEADEROPT    
  janet_def(env, "header-opt/unified", janet_wrap_integer(CURLHEADER_UNIFIED), "unified (bitmask)");
  janet_def(env, "header-opt/separate", janet_wrap_integer(CURLHEADER_SEPARATE), "separate (bitmask)");

  // CURLOPT_HTTPAUTH  
  janet_def(env, "http-auth/none", janet_wrap_integer(CURLAUTH_NONE), "no authentication");
  janet_def(env, "http-auth/basic", janet_wrap_integer(CURLAUTH_BASIC), "basic authentication type");
  janet_def(env, "http-auth/digest", janet_wrap_integer(CURLAUTH_DIGEST), "digest authentication type");
  janet_def(env, "http-auth/digest-ie", janet_wrap_integer(CURLAUTH_DIGEST_IE), "digest authentication type with IE flavor");  
  janet_def(env, "http-auth/negotiate", janet_wrap_integer(CURLAUTH_NEGOTIATE), "negotiate authentication type");
  janet_def(env, "http-auth/gss-api", janet_wrap_integer(CURLAUTH_GSSAPI), "gss api authentication type");
  janet_def(env, "http-auth/ntlm", janet_wrap_integer(CURLAUTH_NTLM), "ntlm authentication type");
  janet_def(env, "http-auth/ntlm-wb", janet_wrap_integer(CURLAUTH_NTLM_WB), "ntlm wb authentication type");
  janet_def(env, "http-auth/bearer", janet_wrap_integer(CURLAUTH_BEARER), "bearer authentication type");
  janet_def(env, "http-auth/only", janet_wrap_integer(CURLAUTH_ONLY), "//TODO: how to handle this?");
  janet_def(env, "http-auth/any", janet_wrap_integer(CURLAUTH_ANY), "any authentication type");
  janet_def(env, "http-auth/any-safe", janet_wrap_integer(CURLAUTH_ANYSAFE), "any safe authentication type");

  // CURLOPT_PROXYAUTH  
  janet_def(env, "proxy/none", janet_wrap_integer(CURLAUTH_NONE), "none");
  janet_def(env, "proxy/basic", janet_wrap_integer(CURLAUTH_BASIC), "basic");
  janet_def(env, "proxy/digest", janet_wrap_integer(CURLAUTH_DIGEST), "digest");
  janet_def(env, "proxy/negotiate", janet_wrap_integer(CURLAUTH_NEGOTIATE), "negotiate");
  janet_def(env, "proxy/gssapi", janet_wrap_integer(CURLAUTH_GSSAPI), "gssapi");
  janet_def(env, "proxy/ntlm", janet_wrap_integer(CURLAUTH_NTLM), "ntlm");
  janet_def(env, "proxy/digest-ie", janet_wrap_integer(CURLAUTH_DIGEST_IE), "digest-ie");
  janet_def(env, "proxy/ntlm-wb", janet_wrap_integer(CURLAUTH_NTLM_WB), "ntlm-wb");
  janet_def(env, "proxy/bearer", janet_wrap_integer(CURLAUTH_BEARER), "bearer");
  janet_def(env, "proxy/only", janet_wrap_integer(CURLAUTH_ONLY), "only");
  janet_def(env, "proxy/any", janet_wrap_integer(CURLAUTH_ANY), "any");
  janet_def(env, "proxy/anysafe", janet_wrap_integer(CURLAUTH_ANYSAFE), "anysafe");

  // CURLOPT_SOCKS5_AUTH
  janet_def(env, "sock5-auth/none", janet_wrap_integer(CURLAUTH_NONE), "no authentication (bitmask)");
  janet_def(env, "sock5-auth/basic", janet_wrap_integer(CURLAUTH_BASIC), "basic authentication type (bitmask)");
  janet_def(env, "sock5-auth/digest", janet_wrap_integer(CURLAUTH_DIGEST), "digest authentication type (bitmask)");
  janet_def(env, "sock5-auth/digest-ie", janet_wrap_integer(CURLAUTH_DIGEST_IE), "digest authentication type with IE flavor (bitmask)");  
  janet_def(env, "sock5-auth/negotiate", janet_wrap_integer(CURLAUTH_NEGOTIATE), "negotiate authentication type (bitmask)");
  janet_def(env, "sock5-auth/gss-api", janet_wrap_integer(CURLAUTH_GSSAPI), "gss api authentication type (bitmask)");
  janet_def(env, "sock5-auth/ntlm", janet_wrap_integer(CURLAUTH_NTLM), "ntlm authentication type (bitmask)");
  janet_def(env, "sock5-auth/ntlm-wb", janet_wrap_integer(CURLAUTH_NTLM_WB), "ntlm wb authentication type (bitmask)");
  janet_def(env, "sock5-auth/bearer", janet_wrap_integer(CURLAUTH_BEARER), "bearer authentication type (bitmask)");
  janet_def(env, "sock5-auth/only", janet_wrap_integer(CURLAUTH_ONLY), "//TODO: how to handle this? (bitmask)");
  janet_def(env, "sock5-auth/any", janet_wrap_integer(CURLAUTH_ANY), "any authentication type (bitmask)");
  janet_def(env, "sock5-auth/any-safe", janet_wrap_integer(CURLAUTH_ANYSAFE), "any safe authentication type (bitmask)");

  // janet_def(env, "###", janet_wrap_integer($$$), "@@@");

  // curl easy submodule
  submod_easy(env);
}