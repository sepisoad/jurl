(import ../curl/build/curl :as curl)

(let [curl (curl/easy-init)]
  (curl/easy-setopt curl {
    :verbose true
    :header false
    :no-progress true
    :no-signal false
    :wildcard-match true
    # ======
    :port 443
    #:proxy "socks5://127.0.0.1:1987"
    :userpwd "user:pass"
    :url "https://sepisoad.com"
    # ======
    :timeout 15})

  (let [res (curl/easy-perform curl)]
    (if (not (false? res))
      (do
        (print (res :buffer))
        (print (res :size)))))
  (curl/easy-cleanup curl))