cmake \
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/install/mongo-c-driver-master \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="-fsanitize=address" \
    -S/Users/kevin.albertson/code/c-bootstrap/investigations/mongos_pinning \
    -B/Users/kevin.albertson/code/c-bootstrap/investigations/mongos_pinning/cmake-build
