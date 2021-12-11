rm cmake-build/CMakeCache.txt
cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-master \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_C_FLAGS="-fsanitize=address -Wall -Werror -pedantic -Wimplicit-function-declaration" \
    -S./ \
    -B./cmake-build $@