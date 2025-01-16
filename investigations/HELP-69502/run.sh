: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"
: "${COMMIT:?"Set to commit of C driver to test"}"
: "${MONGODB_URI:?"Set to the URI to connect"}"

echo "Testing commit $COMMIT"

# Use `LD_LIBRARY_PATH` for Linux and `DYLD_LIBRARY_PATH` for macOS:
export LD_LIBRARY_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT/lib"
export DYLD_LIBRARY_PATH="$INSTALL_PREFIX/mongo-c-driver-$COMMIT/lib"
./cmake-build/run-workload
