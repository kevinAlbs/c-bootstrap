set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

export LIBBSON_SUFFIX=${LIBBSON_SUFFIX}
export LIBBSON_EXTRA_CMAKE_OPTIONS=${LIBBSON_EXTRA_CMAKE_OPTIONS}
export LIBBSON_GITREF=${LIBBSON_GITREF:-master}
export LIBBSON_INSTALL_PREFIX=${LIBBSON_INSTALL_PREFIX:-$(pwd)/install/libbson-$LIBBSON_GITREF$LIBBSON_SUFFIX}
export LIBBSON_SRCDIR=$LIBBSON_INSTALL_PREFIX-src
export LIBBSON_CMAKE_BUILD_TYPE=${LIBBSON_CMAKE_BUILD_TYPE:-Debug}

if [[ "$LIBBSON_CMAKE_BUILD_TYPE" == "Release" ]]; then
    LIBBSON_INSTALL_PREFIX="$LIBBSON_INSTALL_PREFIX-release"
fi

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    LIBBSON_INSTALL_PREFIX=$(cygpath -w $LIBBSON_INSTALL_PREFIX)
fi

mkdir -p $LIBBSON_INSTALL_PREFIX
rm -rf $LIBBSON_SRCDIR
mkdir -p $LIBBSON_SRCDIR

cd $LIBBSON_SRCDIR
git clone git@github.com:mongodb/mongo-c-driver.git
cd mongo-c-driver
git checkout $LIBBSON_GITREF
mkdir cmake-build
cd cmake-build

echo "About to install libbson ($LIBBSON_GITREF) into $LIBBSON_INSTALL_PREFIX"

$CMAKE $LIBBSON_EXTRA_CMAKE_OPTIONS \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -DENABLE_SHM_COUNTERS=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DENABLE_STATIC=ON \
    -DCMAKE_MACOSX_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$LIBBSON_INSTALL_PREFIX \
    -DCMAKE_BUILD_TYPE=$LIBBSON_CMAKE_BUILD_TYPE \
    -DENABLE_MONGOC=OFF ..

$CMAKE --build . --target install