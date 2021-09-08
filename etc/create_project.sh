PROJECT_NAME=$1

if [ -z "$PROJECT_NAME" ]; then
    echo "required argument PROJECT_NAME not supplied"
    exit 1
fi

cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project($PROJECT_NAME LANGUAGES C)
find_package(mongoc-1.0 REQUIRED)
add_executable (main.out main.c)
target_link_libraries(main.out mongo::mongoc_static)
EOF

cat <<EOF > configure.sh
cmake \\
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/install/mongo-c-driver-master \\
    -DCMAKE_BUILD_TYPE="Debug" \\
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \\
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \\
    -DCMAKE_C_FLAGS="-fsanitize=address" \\
    -S$(pwd) \\
    -B$(pwd)/cmake-build
EOF

chmod u+x configure.sh

cat <<EOF > build.sh
cmake --build cmake-build --target main.out
EOF

chmod u+x build.sh

cat <<EOF > run.sh
./cmake-build/main.out
EOF

chmod u+x run.sh

cat <<EOF > main.c
#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();

    mongoc_cleanup ();
}
EOF

cat <<EOF > setenv.sh
export DYLD_LIBRARY_PATH=/Users/kevin.albertson/install/mongo-c-driver-master/lib
export PKG_CONFIG_PATH=/Users/kevin.albertson/install/mongo-c-driver-master/lib/pkgconfig
EOF