set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

MONGO_C_DRIVER_INSTALL_PREFIX=${MONGO_C_DRIVER_INSTALL_PREFIX:-$(pwd)/install/mongo-c-driver-master}
export MONGO_CXX_DRIVER_EXTRA_CMAKE_OPTIONS=${MONGO_CXX_DRIVER_EXTRA_CMAKE_OPTIONS}
export MONGO_CXX_DRIVER_SUFFIX=$MONGO_CXX_DRIVER_SUFFIX;
export MONGO_CXX_DRIVER_GITREF=${MONGO_CXX_DRIVER_GITREF:-master}
export MONGO_CXX_DRIVER_INSTALL_PREFIX=${MONGO_CXX_DRIVER_INSTALL_PREFIX:-$(pwd)/install/mongo-cxx-driver-$MONGO_CXX_DRIVER_GITREF$MONGO_CXX_DRIVER_SUFFIX}
export MONGO_CXX_DRIVER_SRCDIR=$MONGO_CXX_DRIVER_INSTALL_PREFIX-src
export MONGO_CXX_DRIVER_CMAKE_BUILD_TYPE=${MONGO_CXX_DRIVER_CMAKE_BUILD_TYPE:-Debug}

TMPDIR=$(pwd)/tmp

. ./etc/find_os.sh
# Get latest cmake using C driver scripts.
. $HOME/code/mongo-c-driver/.evergreen/scripts/find-cmake-latest.sh
CMAKE=$(find_cmake_latest)

if [[ $OS == "WINDOWS" ]]; then
    MONGO_C_DRIVER_INSTALL_PREFIX=$(cygpath -w $MONGO_C_DRIVER_INSTALL_PREFIX)
    MONGO_CXX_DRIVER_INSTALL_PREFIX=$(cygpath -w $MONGO_CXX_DRIVER_INSTALL_PREFIX)
    # Tell Windows to build x64
    export CMAKE_GENERATOR_PLATFORM=x64
fi

mkdir -p $MONGO_CXX_DRIVER_INSTALL_PREFIX
rm -rf $MONGO_CXX_DRIVER_SRCDIR
mkdir -p $MONGO_CXX_DRIVER_SRCDIR

cd $MONGO_CXX_DRIVER_SRCDIR
git clone git@github.com:mongodb/mongo-cxx-driver.git
cd mongo-cxx-driver
git checkout $MONGO_CXX_DRIVER_GITREF
mkdir cmake-build
cd cmake-build

echo "About to install C++ driver ($MONGO_CXX_DRIVER_GITREF) into $MONGO_CXX_DRIVER_INSTALL_PREFIX"

$CMAKE \
    -DCMAKE_CXX_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_INSTALL_PREFIX=$MONGO_CXX_DRIVER_INSTALL_PREFIX \
    -DENABLE_TESTS=OFF \
    -DCMAKE_PREFIX_PATH="$MONGO_C_DRIVER_INSTALL_PREFIX" ..

$CMAKE --build . --target install --parallel
