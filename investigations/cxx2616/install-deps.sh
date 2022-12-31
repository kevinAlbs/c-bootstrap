export LIBMONGOCRYPT_EXTRA_CMAKE_OPTIONS='-G "Visual Studio 17 2022" -A x64'
export LIBMONGOCRYPT_GITREF="1.6.0"
export LIBMONGOCRYPT_INSTALL_PREFIX="$(pwd)/install/libmongocrypt-1.6.0"
./etc/install_libmongocrypt.sh


export MONGO_C_DRIVER_EXTRA_CMAKE_OPTIONS='-G "Visual Studio 17 2022" -A x64'
export MONGO_C_DRIVER_GITREF="1.23.1"
./etc/install_mongo_c_driver.sh
