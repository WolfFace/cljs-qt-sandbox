(defproject qml-cljs "0.1.0-SNAPSHOT"
  :description "FIXME: write this!"
  :url "http://example.com/FIXME"

  :dependencies [[org.clojure/clojure "1.5.1"]
                 [org.clojure/clojurescript "0.0-2080"]]

  :plugins [[lein-cljsbuild "1.0.0"]]

  :source-paths ["src"]

  :cljsbuild { 
    :builds [{:id "qml-cljs"
              :source-paths ["src"]
              :compiler {
                :output-to "qml_cljs.js"
		:output-dir "out"
                :optimizations :simple}}]})
