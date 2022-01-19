cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-1.14.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_STANDARD=20 \
    -S./ \
    -B./cmake-build
