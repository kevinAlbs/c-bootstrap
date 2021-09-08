cmake \
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/install/mongo-c-driver-master \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -S/Users/kevin.albertson/code/c-bootstrap/investigations/drivers1621 \
    -B/Users/kevin.albertson/code/c-bootstrap/investigations/drivers1621/cmake-build
