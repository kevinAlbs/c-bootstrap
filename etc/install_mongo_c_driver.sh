set -o errexit

if [[ "$(basename "$(pwd)")" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

: "${LIBMONGOCRYPT_GITREF:?}"
: "${MONGO_C_DRIVER_GITREF:?}"

LIBMONGOCRYPT_INSTALL_PREFIX=$(pwd)/install/libmongocrypt-$LIBMONGOCRYPT_GITREF
if [ ! -d "$LIBMONGOCRYPT_INSTALL_PREFIX" ]; then
    echo "Did not find libmongocrypt install at: $LIBMONGOCRYPT_INSTALL_PREFIX";
    echo "Try running: LIBMONGOCRYPT_GITREF=$LIBMONGOCRYPT_GITREF ./etc/install_libmongocrypt.sh";
    exit 1;
fi

MONGO_C_DRIVER_PREFIX="$(pwd)/install/mongo-c-driver-$MONGO_C_DRIVER_GITREF"
MONGO_C_DRIVER_SRCDIR=$MONGO_C_DRIVER_PREFIX-src

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    LIBMONGOCRYPT_INSTALL_PREFIX=$(cygpath -w "$LIBMONGOCRYPT_INSTALL_PREFIX")
    MONGO_C_DRIVER_PREFIX=$(cygpath -w "$MONGO_C_DRIVER_PREFIX")
fi

mkdir -p "$MONGO_C_DRIVER_PREFIX"
rm -rf "$MONGO_C_DRIVER_SRCDIR"
mkdir -p "$MONGO_C_DRIVER_SRCDIR"

cd "$MONGO_C_DRIVER_SRCDIR"
git clone https://github.com/mongodb/mongo-c-driver.git
cd mongo-c-driver
git checkout "$MONGO_C_DRIVER_GITREF"
mkdir cmake-build
cd cmake-build

echo "Installing C driver ($MONGO_C_DRIVER_GITREF) to $MONGO_C_DRIVER_PREFIX ..."

$CMAKE "$MONGO_C_DRIVER_EXTRA_CMAKE_OPTIONS" \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -DENABLE_SHM_COUNTERS=OFF \
    -DENABLE_TESTS=OFF \
    -DBUILD_TESTING=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DENABLE_STATIC=ON \
    -DCMAKE_MACOSX_RPATH=ON \
    -DENABLE_CLIENT_SIDE_ENCRYPTION=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX="$MONGO_C_DRIVER_PREFIX" \
    -DCMAKE_PREFIX_PATH="$LIBMONGOCRYPT_INSTALL_PREFIX" ..

$CMAKE --build . --target install -j8

echo "Installing C driver ($MONGO_C_DRIVER_GITREF) to $MONGO_C_DRIVER_PREFIX ... done"
