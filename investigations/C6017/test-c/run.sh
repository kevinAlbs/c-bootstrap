./cmake-build/main.out

echo ""

export DYLD_LIBRARY_PATH
DYLD_LIBRARY_PATH=/Users/kevin.albertson/review/mongo-c-driver-2019/cmake-build/src/libmongoc:$DYLD_LIBRARY_PATH
DYLD_LIBRARY_PATH=/Users/kevin.albertson/review/mongo-c-driver-2019/cmake-build/src/libbson:$DYLD_LIBRARY_PATH
./cmake-build/main.out
