PROJECT_NAME=${PROJECT_NAME:-}

if [ -z "$PROJECT_NAME" ]; then
    echo "required environment variable PROJECT_NAME not supplied"
    exit 1
fi

if [[ -d "investigations/$PROJECT_NAME" ]]; then
    echo "investigations/$PROJECT_NAME already exists"
    exit 1
fi

mkdir -p investigations/$PROJECT_NAME
pushd investigations/$PROJECT_NAME

cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project($PROJECT_NAME LANGUAGES C)
find_package(mongoc-1.0 REQUIRED)
add_executable (main.out main.c)
target_link_libraries(main.out mongo::mongoc_static)
EOF

cat <<EOF > configure.sh
cmake \\
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-master \\
    -DCMAKE_BUILD_TYPE="Debug" \\
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \\
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \\
    -DCMAKE_C_FLAGS="-fsanitize=address" \\
    -S./ \\
    -B./cmake-build
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
    char *uristr;

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    mongoc_cleanup ();
}
EOF

cat <<EOF > setenv.sh
export DYLD_LIBRARY_PATH=/Users/kevin.albertson/install/mongo-c-driver-master/lib
export PKG_CONFIG_PATH=/Users/kevin.albertson/install/mongo-c-driver-master/lib/pkgconfig
EOF

popd