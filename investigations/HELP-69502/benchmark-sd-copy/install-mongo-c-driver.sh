set -o errexit # Exit if any command fails

: "${INSTALL_PREFIX:?"Set to destination path prefix for C driver installs"}"

if [ ! -d mongo-c-driver ]; then
    git clone https://github.com/mongodb/mongo-c-driver.git
fi
pushd mongo-c-driver || exit

# Install commit with observed performance regression:
COMMIT="ded9ae5e9f2897a283305175aae8e1bbf4021c36"
EXTRA_MESSAGE="with observed performance regression"
# Clear build.
rm -rf cmake-build || true
git reset --hard
git checkout "$COMMIT"
# Add an identifiable printf in mongoc_init:
sed -i '' "/_mongoc_handshake_init ();/ s/\$/ printf(\"mongo-c-driver using commit: $COMMIT $EXTRA_MESSAGE\");/" src/libmongoc/src/mongoc/mongoc-init.c
cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX/mongo-c-driver-$COMMIT" \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -Bcmake-build
cmake --build cmake-build --target install --parallel

# Install commit before observed performance regression:
COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d"
EXTRA_MESSAGE="before observed performance regression"
# Clear build.
rm -rf cmake-build || true
git reset --hard
git checkout "$COMMIT"
# Add an identifiable printf in mongoc_init:
sed -i '' "/_mongoc_handshake_init ();/ s/\$/ printf(\"mongo-c-driver using commit: $COMMIT $EXTRA_MESSAGE\");/" src/libmongoc/src/mongoc/mongoc-init.c
cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX/mongo-c-driver-$COMMIT" \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -Bcmake-build
cmake --build cmake-build --target install --parallel
popd || exit # mongo-c-driver
