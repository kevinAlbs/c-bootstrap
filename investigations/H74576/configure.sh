MONGO_C_DRIVER_INSTALL_PATH="/Users/kevin.albertson/code/c-bootstrap/investigations/H74576/repro/install/mongo-c-driver-1.30.3"

cmake \
    -DCMAKE_PREFIX_PATH="$MONGO_C_DRIVER_INSTALL_PATH" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="/opt/homebrew/opt/llvm/bin/clang" \
    -DCMAKE_C_FLAGS="-fsanitize=address" \
    -S./ \
    -B./cmake-build
