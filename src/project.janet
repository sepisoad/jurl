(declare-project
  :name "jurl"
  :description "jannet wrapper around libcurl"
  :author "Sepehr Aryani"
  :license "GPLv3"
  :url "https://github.com/sepisoad/jurl"
  :repo "https://github.com/sepisoad/jurl"
  )

(declare-native
  :name "curl"
  :lflags @["-lcurl" "-ljanet" "-shared" "-rdynamic" "-g"]
  :source @["base.c" "funcs.c" "opts.c" "curl.c" "easy.c"])