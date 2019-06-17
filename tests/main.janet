(import ../src/build/curl :as curl)

(def file (file/open "DOWNLOAD" :wb+))

(def c (curl/easy/init))
(:setopt c
  :url "http://localhost:8000/file_example_OGG_1920_13_3mg.ogg"
  # :url "https://upload.wikimedia.org/wikipedia/commons/7/79/Big_Buck_Bunny_small.ogv"  
  :write-function (fn [buf] (file/write file buf))
  :header-function (fn [buf] (print buf))
  # :verbose? false
  # :read-function (fn [len] (print "do i need to read " len " bytes?") "holly shit")
  :no-progress? false
  :progress-function (fn [a b c d] (print "progress => [%" (math/floor (/ (* b 100) a)) "]"))
  )
(:perform c)
(:flush file)
(:close file)
