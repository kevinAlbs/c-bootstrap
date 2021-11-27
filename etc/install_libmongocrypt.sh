set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

LIBBSON_PATH=${LIBBSON_PATH:-$(pwd)/install/libbson-master}
GITREF=${GITREF:-master}
PREFIX=${PREFIX:-$(pwd)/install/libmongocrypt-$GITREF}
TMPDIR=$(pwd)/tmp
CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Debug}

if [[ "$CMAKE_BUILD_TYPE" == "Release" ]]; then
    PREFIX="$PREFIX-release"
fi

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    LIBBSON_PATH=$(cygpath -w $LIBBSON_PATH)
    PREFIX=$(cygpath -w $PREFIX)
fi

mkdir -p $PREFIX
rm -rf $TMPDIR
mkdir -p $TMPDIR

cd $TMPDIR
git clone git@github.com:mongodb/libmongocrypt.git
cd libmongocrypt
git checkout $GITREF
mkdir cmake-build
cd cmake-build

echo "About to install libmongocrypt ($GITREF) into $PREFIX"

$CMAKE -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DCMAKE_PREFIX_PATH=$LIBBSON_PATH ..

$CMAKE --build . --target install