cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-1.14.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="-fsanitize=address" \
    -S./ \
    -B./cmake-build
