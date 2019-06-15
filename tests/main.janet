(import ../src/build/curl :as curl)

(print curl/version)
(let [curl (curl/easy/init)]
  (print (curl/easy/escape curl "https://janet-lang.org"))
  (print (curl/easy/unescape curl
    (curl/easy/escape curl "https://janet-lang.org")))
  (print (curl/easy/strerror 10))
  (curl/easy/setopt curl 
    :url "ssssss" 
    :timeout 4554
    :can true)
  (curl/easy/cleanup curl))