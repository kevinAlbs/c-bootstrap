# Building

If you have not already, install vcpkg:

```bash
cd $HOME
git clone https://github.com/microsoft/vcpkg.git
VCPKG_ROOT=$HOME/vcpkg
pushd vcpkg && ./bootstrap-vcpkg.sh && popd
export PATH=$VCPKG_ROOT:$PATH
```

Install amongoc:

```bash
cd $HOME
AMONGOC_INSTALL="$HOME/mongo-c-driver-async-0.1.0"
git clone https://github.com/mongodb-labs/mongo-c-driver-async/
pushd mongo-c-driver-async
cmake -B_build 
cmake --build _build --parallel
cmake --install _build --prefix "$AMONGOC_INSTALL"
popd
```

Build this test client application:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=$AMONGOC_INSTALL \
      -Bcmake-build
cmake --build cmake-build --target all
```
