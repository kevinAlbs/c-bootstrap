cmake \
    -DCMAKE_PREFIX_PATH=_deps/mongo-c-driver-1.27.1 \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build
