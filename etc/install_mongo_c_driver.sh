set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

LIBMONGOCRYPT_INSTALL_PREFIX=${LIBMONGOCRYPT_INSTALL_PREFIX:-$(pwd)/install/libmongocrypt-master}
export MONGO_C_DRIVER_EXTRA_CMAKE_OPTIONS=${MONGO_C_DRIVER_EXTRA_CMAKE_OPTIONS}
export MONGO_C_DRIVER_SUFFIX=$MONGO_C_DRIVER_SUFFIX;
export MONGO_C_DRIVER_GITREF=${MONGO_C_DRIVER_GITREF:-master}
export MONGO_C_DRIVER_PREFIX=${MONGO_C_DRIVER_PREFIX:-$(pwd)/install/mongo-c-driver-$MONGO_C_DRIVER_GITREF$MONGO_C_DRIVER_SUFFIX}
export MONGO_C_DRIVER_SRCDIR=$MONGO_C_DRIVER_PREFIX-src
export MONGO_C_DRIVER_CMAKE_BUILD_TYPE=${MONGO_C_DRIVER_CMAKE_BUILD_TYPE:-Debug}

if [[ "$MONGO_C_DRIVER_CMAKE_BUILD_TYPE" == "Release" ]]; then
    MONGO_C_DRIVER_PREFIX="$MONGO_C_DRIVER_PREFIX-release"
fi

. ./etc/find_os.sh
# Get latest cmake using C driver scripts.
. $HOME/code/mongo-c-driver/.evergreen/scripts/find-cmake-latest.sh
CMAKE=$(find_cmake_latest)

if [[ $OS == "WINDOWS" ]]; then
    LIBMONGOCRYPT_INSTALL_PREFIX=$(cygpath -w $LIBMONGOCRYPT_INSTALL_PREFIX)
    MONGO_C_DRIVER_PREFIX=$(cygpath -w $MONGO_C_DRIVER_PREFIX)
    # Tell Windows to build x64
    export CMAKE_GENERATOR_PLATFORM=x64
fi

mkdir -p $MONGO_C_DRIVER_PREFIX
rm -rf $MONGO_C_DRIVER_SRCDIR
mkdir -p $MONGO_C_DRIVER_SRCDIR

cd $MONGO_C_DRIVER_SRCDIR
git clone git@github.com:mongodb/mongo-c-driver.git
cd mongo-c-driver
git checkout $MONGO_C_DRIVER_GITREF
mkdir cmake-build
cd cmake-build

echo "About to install C driver ($MONGO_C_DRIVER_GITREF) into $MONGO_C_DRIVER_PREFIX"

$CMAKE $MONGO_C_DRIVER_EXTRA_CMAKE_OPTIONS \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -DENABLE_SHM_COUNTERS=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DENABLE_STATIC=ON \
    -DCMAKE_MACOSX_RPATH=ON \
    -DENABLE_CLIENT_SIDE_ENCRYPTION=ON \
    -DCMAKE_BUILD_TYPE=$MONGO_C_DRIVER_CMAKE_BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$MONGO_C_DRIVER_PREFIX \
    -DCMAKE_PREFIX_PATH=$LIBMONGOCRYPT_INSTALL_PREFIX ..

$CMAKE --build . --target install -- -j8
