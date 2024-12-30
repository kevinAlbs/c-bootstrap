# Building

If you have not already, install vcpkg:
```bash
git clone https://github.com/microsoft/vcpkg.git
VCPKG_ROOT=$(pwd)/vcpkg
pushd vcpkg && ./bootstrap-vcpkg.sh && popd
export PATH=$VCPKG_ROOT:$PATH
```

Install amongoc:

```
AMONGOC_INSTALL="$(pwd)/mongo-c-driver-async-0.1.0"
git clone https://github.com/mongodb-labs/mongo-c-driver-async/
pushd mongo-c-driver-async
cmake -B_build 
cmake --build _build --parallel
cmake --install _build --prefix "$AMONGOC_INSTALL"
popd
```

Build the client application:
```
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=$AMONGOC_INSTALL \
      -Bcmake-build 
```
