set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

LIBBSON_INSTALL_PREFIX=${LIBBSON_INSTALL_PREFIX:-$(pwd)/install/libbson-master}
export LIBMONGOCRYPT_GITREF=${LIBMONGOCRYPT_GITREF:-master}
export LIBMONGOCRYPT_INSTALL_PREFIX=${LIBMONGOCRYPT_INSTALL_PREFIX:-$(pwd)/install/libmongocrypt-$LIBMONGOCRYPT_GITREF}
export LIBMONGOCRYPT_SRCDIR=$LIBMONGOCRYPT_INSTALL_PREFIX-src
export LIBMONGOCRYPT_CMAKE_BUILD_TYPE=${LIBMONGOCRYPT_CMAKE_BUILD_TYPE:-Debug}

if [[ "$LIBMONGOCRYPT_CMAKE_BUILD_TYPE" == "Release" ]]; then
    LIBMONGOCRYPT_INSTALL_PREFIX="$LIBMONGOCRYPT_INSTALL_PREFIX-release"
fi

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    LIBBSON_INSTALL_PREFIX=$(cygpath -w $LIBBSON_INSTALL_PREFIX)
    LIBMONGOCRYPT_INSTALL_PREFIX=$(cygpath -w $LIBMONGOCRYPT_INSTALL_PREFIX)
fi

mkdir -p $LIBMONGOCRYPT_INSTALL_PREFIX
rm -rf $LIBMONGOCRYPT_SRCDIR
mkdir -p $LIBMONGOCRYPT_SRCDIR

cd $LIBMONGOCRYPT_SRCDIR
git clone git@github.com:mongodb/libmongocrypt.git
cd libmongocrypt
git checkout $LIBMONGOCRYPT_GITREF
mkdir cmake-build
cd cmake-build

echo "About to install libmongocrypt ($LIBMONGOCRYPT_GITREF) into $LIBMONGOCRYPT_INSTALL_PREFIX"

$CMAKE -DCMAKE_INSTALL_PREFIX=$LIBMONGOCRYPT_INSTALL_PREFIX \
    -DCMAKE_BUILD_TYPE=$LIBMONGOCRYPT_CMAKE_BUILD_TYPE \
    -DCMAKE_PREFIX_PATH=$LIBBSON_INSTALL_PREFIX ..

$CMAKE --build . --target install