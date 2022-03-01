cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-master \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -S./ \
    -B./cmake-build
