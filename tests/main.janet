(import ../src/build/curl :as curl)

(print curl/version)
(let [curl (curl/easy/init)]
  (curl/easy/setopt curl 
    :url "https://janet-lang.org"
    :verbose? true
    :timeout 10)  
  (curl/easy/perform curl)
  (curl/easy/cleanup curl))