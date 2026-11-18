cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-2.5.6 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/opt/homebrew/opt/llvm/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="-fsanitize=address" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -S./ \
    -B./cmake-build
