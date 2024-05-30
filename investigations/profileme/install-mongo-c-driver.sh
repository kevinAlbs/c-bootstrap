set -o errexit

# Check for required commands.
if ! command -v cmake > /dev/null; then
    echo "cmake not detected. Try: 'brew install cmake'"
    exit 1
fi

VERSION=1.27.1

# Recreate the `_deps` directory if exists.
if [ -d _deps ]; then
    rm -rf _deps
fi
mkdir _deps

DEPS=$(pwd)/_deps
INSTALL_PATH="$DEPS/mongo-c-driver-$VERSION"
if [ -d "$INSTALL_PATH" ]; then
    echo "Detected install path $INSTALL_PATH. To reinstall, remove directory first with: 'rm -rf $INSTALL_PATH'"
    exit 1
fi

echo "Installing C driver $VERSION ... begin"
pushd "$DEPS" || exit
git clone https://github.com/mongodb/mongo-c-driver --branch "$VERSION" "mongo-c-driver-$VERSION-src"
cmake \
    -D CMAKE_BUILD_TYPE=RelWithDebInfo \
    -D ENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -D ENABLE_SHM_COUNTERS=OFF \
    -D ENABLE_TESTS=OFF \
    -D BUILD_TESTING=OFF \
    -D ENABLE_EXAMPLES=OFF \
    -D ENABLE_STATIC=ON \
    -D CMAKE_MACOSX_RPATH=ON \
    -D "CMAKE_INSTALL_PREFIX=$INSTALL_PATH" \
    -B "mongo-c-driver-$VERSION-build" \
    -S "mongo-c-driver-$VERSION-src"
cmake --build "mongo-c-driver-$VERSION-build" --target install -- -j16
echo "Installing C driver $VERSION ... end"

