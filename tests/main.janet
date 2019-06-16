(import ../src/build/curl :as curl)

(print curl/version)
(let [curl (curl/easy/init)]
  # (curl/easy/setopt curl 
  #   :url "https://janet-lang.org"
  #   :verbose? true
  #   :timeout 10)  
  # (curl/easy/perform curl)
  (print curl/proxy-type/socks4)
  (print curl/proto/smtp)
  (print curl/proto/ftp)
  (print curl/proto/http)
  (curl/easy/cleanup curl))