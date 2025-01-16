Benchmark the C driver running find operations with various thread counts.

# Instructions

Install versions of the C driver to compare:
```bash
export INSTALL_PREFIX="$HOME"
COMMIT="ded9ae5e9f2897a283305175aae8e1bbf4021c36" ./install-mongo-c-driver.sh
COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d" ./install-mongo-c-driver.sh
```

Compile, from this directory:
```bash
# Use either commit to build.
COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d" ./build.sh
```

Run with:
```bash
export MONGODB_URI="mongodb://localhost:27017"
# COMMIT selects the library at runtime
COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d" ./run.sh
COMMIT="ded9ae5e9f2897a283305175aae8e1bbf4021c36" ./run.sh

# Use THREAD_COUNT to test different thread counts
THREAD_COUNT=5 COMMIT="0eeff0a44ea08a789ce01560ec1e9947b405828d" ./run.sh
```
