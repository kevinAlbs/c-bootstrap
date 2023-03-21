cmake \
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/code/c-bootstrap/install/mongo-c-driver-1.23.2 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER="clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="-fsanitize=address" \
    -S/Users/kevin.albertson/code/c-bootstrap/tools/kmip_dump \
    -B/Users/kevin.albertson/code/c-bootstrap/tools/kmip_dump/cmake-build
