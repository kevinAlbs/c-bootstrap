set -o errexit

: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"

COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d"
echo "Testing commit $COMMIT"
# Use `LD_LIBRARY_PATH` for Linux and `DYLD_LIBRARY_PATH` for macOS:
export LD_LIBRARY_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT/lib"
export DYLD_LIBRARY_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT/lib"
./cmake-build-$COMMIT/benchmark-sd-copy

COMMIT="ded9ae5e9f2897a283305175aae8e1bbf4021c36"
echo "Testing commit $COMMIT"
# Use `LD_LIBRARY_PATH` for Linux and `DYLD_LIBRARY_PATH` for macOS:
export LD_LIBRARY_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT/lib"
export DYLD_LIBRARY_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT/lib"
./cmake-build-$COMMIT/benchmark-sd-copy
