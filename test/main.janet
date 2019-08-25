(import ../build/curl :as curl)

(with [file (file/open "test/DOWNLOAD" :wb+)]

  (def c (curl/easy/init))

  (defn progress [a b c d]
    "Progress function while downloading"
    (def percent 
      (let [x (math/floor (/ (* b 100) a))]
        (if (= x x) x 0)))
    (def rest (- 100 percent))
    (print "\e[Fprogress => [%" (string/repeat "." percent) (string/repeat " " rest)  "]"))

  (:setopt c
           :url "https://upload.wikimedia.org/wikipedia/commons/7/79/Big_Buck_Bunny_small.ogv"  
           :write-function (fn [buf] (file/write file buf))
           :header-function (fn [buf] (:write stdout buf) (:flush stdout))
           :no-progress? false
           :progress-function progress)
  (:perform c))
