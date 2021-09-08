set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

MONGOCRYPT_PATH=${MONGOCRYPT_PATH:-$(pwd)/install/libmongocrypt-master}
GITREF=${GITREF:-master}
PREFIX=${PREFIX:-$(pwd)/install/mongo-c-driver-$GITREF}
TMPDIR=$(pwd)/tmp

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    MONGOCRYPT_PATH=$(cygpath -w $MONGOCRYPT_PATH)
    PREFIX=$(cygpath -w $PREFIX)
fi

mkdir -p $PREFIX
rm -rf $TMPDIR
mkdir -p $TMPDIR

cd $TMPDIR
git clone git@github.com:mongodb/mongo-c-driver.git
cd mongo-c-driver
git checkout $GITREF
mkdir cmake-build
cd cmake-build

echo "About to install C driver ($GITREF) into $PREFIX"

$CMAKE \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -DENABLE_SHM_COUNTERS=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DENABLE_STATIC=ON \
    -DCMAKE_MACOSX_RPATH=ON \
    -DENABLE_CLIENT_SIDE_ENCRYPTION=ON \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DCMAKE_PREFIX_PATH=$MONGOCRYPT_PATH ..

$CMAKE --build . --target install