(import ../curl/build/curl :as curl)

(let [curl (curl/easy-init)]
  (curl/easy-setopt curl {
    :verbose false
    :header false
    :no-progress true
    :no-signal false
    :wildcard-match true
    # ======
    # ======
    :url "https://news.ycombinator.com"
    :timeout 15})

  (let [res (curl/easy-perform curl)]
    (if (not (false? res))
      (do
        (print (res :buffer))
        (print (res :size)))))
  (curl/easy-cleanup curl))