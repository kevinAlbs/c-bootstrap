export ASAN_OPTIONS='detect_leaks=1'
export LSAN_OPTIONS='suppressions=/Users/kevin.albertson/code/mongo-c-driver/.h/lsan-suppressions.txt'
./cmake-build/main.out
