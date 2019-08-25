(import ../build/curl :as curl)

(defn download
  "Download a file with curl easy"
  [url to]
  (def c (curl/easy/init))
  (with [file (file/open to :wb)]
    (var run-yet false)
    (defn progress [a b c d]
      "Progress function while downloading"
      (if (zero? b) (break))
      (def out-of-50 
        (let [x (math/floor (/ (* b 50) a))]
          (if (= x x) x 0)))
      (def rest (- 50 out-of-50))
      (if run-yet
        (:write stdout "\e[F\e[F"))
      (set run-yet true)
      (print "Downloading \e[36m" url "\n\e[34m["
             (string/repeat "." out-of-50) (string/repeat " " rest)  "]\e[0m"))
    (:setopt c
             :url url
             :write-function (fn [buf] (file/write file buf))
             :no-progress? false
             :progress-function progress)
    (:perform c)))

(download 
  "https://upload.wikimedia.org/wikipedia/commons/7/79/Big_Buck_Bunny_small.ogv"  
  "test/DOWNLOAD")
