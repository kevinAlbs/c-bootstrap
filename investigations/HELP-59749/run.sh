MONGOC_VERSION=${MONGOC_VERSION:-"1.25.4"}
export DYLD_LIBRARY_PATH=/Users/kevin.albertson/code/c-bootstrap/install/mongo-c-driver-$MONGOC_VERSION/lib
./cmake-build/main.out
