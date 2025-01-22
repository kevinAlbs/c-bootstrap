: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"

COMMIT="ded9ae5e9f2897a283305175aae8e1bbf4021c36"
cmake \
    -DCMAKE_PREFIX_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build-$COMMIT
cmake --build "cmake-build-$COMMIT" --target all


COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d"
cmake \
    -DCMAKE_PREFIX_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build-$COMMIT
cmake --build "cmake-build-$COMMIT" --target all
