set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

GITREF=${GITREF:-master}
PREFIX=${PREFIX:-$(pwd)/install/libbson-$GITREF}
TMPDIR=$(pwd)/tmp

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
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

echo "About to install libbson ($GITREF) into $PREFIX"

$CMAKE \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -DENABLE_SHM_COUNTERS=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DENABLE_STATIC=ON \
    -DCMAKE_MACOSX_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DENABLE_MONGOC=OFF ..

$CMAKE --build . --target install