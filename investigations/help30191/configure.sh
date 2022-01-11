cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-1.19.2 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="" \
    -DOPENSSL_ROOT_DIR="/Users/kevin.albertson/bin/openssl-1.0.2u-debug" \
    -S./ \
    -B./cmake-build

