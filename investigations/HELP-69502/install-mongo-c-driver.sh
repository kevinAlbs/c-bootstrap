set -o errexit # Exit if any command fails

: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"
: "${COMMIT:?"Set to commit of C driver to install"}"

if [ ! -d mongo-c-driver ]; then
    git clone https://github.com/mongodb/mongo-c-driver.git
fi
pushd mongo-c-driver || exit

# Clear build.
rm -rf cmake-build || true

# Install commit with observed performance regression:
git checkout "$COMMIT"
cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX/mongo-c-driver-$COMMIT" \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -Bcmake-build
cmake --build cmake-build --target install --parallel

popd || exit # mongo-c-driver
