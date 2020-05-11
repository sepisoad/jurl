(declare-project
  :name "jurl"
  :description "Janet wrapper around libcurl"
  :author "Sepehr Aryani"
  :license "GPLv3"
  :url "https://github.com/sepisoad/jurl"
  :repo "https://github.com/sepisoad/jurl")

(def lflags (case (os/which)
              :windows @["libcurl.lib"]
              :linux @["-lcurl"]
              #default
              @["-lcurl"]))

(declare-native
  :name "curl"
  :lflags lflags
  :source ["src/curl.c"])
