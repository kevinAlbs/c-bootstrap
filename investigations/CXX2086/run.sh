INSTALLDIR=mongo-c-driver-1.13.0-nossl
export DYLD_LIBRARY_PATH=$HOME/code/c-bootstrap/install/$INSTALLDIR/lib
echo "Using $INSTALLDIR"
./cmake-build/main.out
