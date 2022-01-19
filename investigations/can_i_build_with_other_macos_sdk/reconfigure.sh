rm -rf cmake-build
cmake \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -S./ \
    -B./cmake-build
