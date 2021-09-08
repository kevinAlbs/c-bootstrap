cmake \
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/install/mongo-c-driver-1.19.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -S/Users/kevin.albertson/code/c-bootstrap/investigations/cve_embedded_null \
    -B/Users/kevin.albertson/code/c-bootstrap/investigations/cve_embedded_null/cmake-build
