: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"
: "${COMMIT:?"Set to commit of C driver to install"}"

cmake \
    -DCMAKE_PREFIX_PATH="$HOME/code/tasks/mongo-c-driver-HELP-69502/.install-0eeff0a44-callcount" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build

cmake --build cmake-build --target run-workload
