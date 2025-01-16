: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"
: "${COMMIT:?"Set to commit of C driver to install"}"

cmake \
    -DCMAKE_PREFIX_PATH="$INSTALL_PREFIX/mongo-c-driver-ded9ae5e9f2897a283305175aae8e1bbf4021c36" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build

cmake --build cmake-build --target run-workload
