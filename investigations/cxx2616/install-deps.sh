export LIBMONGOCRYPT_EXTRA_CMAKE_OPTIONS='-A x64'
export LIBMONGOCRYPT_GITREF="1.6.0"
export LIBMONGOCRYPT_INSTALL_PREFIX="$(pwd)/install/libmongocrypt-1.6.0"
./etc/install_libmongocrypt.sh
# Install will fail with VS 2022 due to building csfle-markup.
# Apply a patch to remove csfle-markup, and try to install again.
root=$(pwd)
cd ./install/libmongocrypt-1.6.0/libmongocrypt
git apply $root/etc/remove-csfle-markup.patch
. ./etc/find_cmake.sh
$CMAKE --build . --target install -j8


export MONGO_C_DRIVER_EXTRA_CMAKE_OPTIONS='-A x64'
export MONGO_C_DRIVER_GITREF="1.23.1"
./etc/install_mongo_c_driver.sh
