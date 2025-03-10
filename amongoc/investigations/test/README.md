# Quick start

The following are steps using vc-pkg to install mongo-c-driver-async and build a test client. These steps were run on Debian 12.

Install vcpkg (if not already installed):
```bash
cd $HOME
git clone https://github.com/microsoft/vcpkg.git
VCPKG_ROOT=$HOME/vcpkg
cd vcpkg
./bootstrap-vcpkg.sh
export PATH=$VCPKG_ROOT:$PATH
```

Install amongoc:
```bash
cd $HOME
AMONGOC_INSTALL="$HOME/mongo-c-driver-async-0.1.0"
git clone https://github.com/mongodb-labs/mongo-c-driver-async/
cd mongo-c-driver-async
cmake -B_build 
cmake --build _build --parallel
cmake --install _build --prefix "$AMONGOC_INSTALL"
```

Start a local MongoDB server:
```bash
./mongod
```

Build this test client application:
```bash
# cd to this directory.
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=$AMONGOC_INSTALL \
      -Bcmake-build
cmake --build cmake-build --target hello-amongoc
```


Run this application
```bash
./cmake-build/hello-amongoc
```
