cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-1.23.2 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/opt/homebrew/opt/llvm/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="-fsanitize=address,leak" \
    -S./ \
    -B./cmake-build
