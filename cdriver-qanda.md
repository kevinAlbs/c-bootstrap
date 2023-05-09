# C Driver Q&A

## Q12: What version of POSIX does the C driver confirm to?
A: POSIX.1-2008 plus the XSI extension and BSD-derived definitions. See https://github.com/mongodb/mongo-c-driver/blob/2a24fef34e985770abbe30794edb243079ea0cc8/CMakeLists.txt#L267


## Q11: How many connections are expected?
A:

A single-threaded `mongoc_client_t` creates one connection per server. These connections are used both for monitoring and application operations.

https://mongoc.org/libmongoc/current/connection-pooling.html#single-mode

> A single client opens one connection per server in your topology: these connections are used both for scanning the topology and performing normal operations.

A `mongoc_client_pool_t` creates one or two connections per server for monitoring depending on the server version. For server version < 4.4, one monitoring connection per server is created. For server version >= 4.4, two monitoring connections per server are created. Each `mongoc_client_t` opens its own connection to each server it uses for application operations. Connections for application operations are created on-demand. E.g. if the application has not performed any application operations, only the monitoring connections are created.

The maximum number of `mongoc_client_t` that can be checked out at a given time is bounded by the URI option `maxPoolSize`. `maxPoolSize` is default 100.

## Q10: What version of the C driver is packaged on platform X?
A: See https://repology.org/project/mongo-c-driver/versions

## Q9: How do I install C and C++ driver from source?

Replace directories as needed.

```
# Exit on first error.
set -o errexit
# Print each line.
set -o xtrace

INSTALL_DIR="/Users/kevin.albertson/install"

echo "Download C driver source ... begin"
wget https://github.com/mongodb/mongo-c-driver/releases/download/1.23.2/mongo-c-driver-1.23.2.tar.gz
tar xzf mongo-c-driver-1.23.2.tar.gz
cd mongo-c-driver-1.23.2
mkdir cmake-build
cd cmake-build
echo "Download C driver source ... end"

echo "Configure C driver ... begin"
cmake \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR/mongo-c-driver-1.23.2" \
    ..
echo "Configure C driver ... end"

echo "Install C driver ... begin"
cmake --build . --target install
echo "Install C driver ... end"

cd ../..

echo "Download the C++ driver source ... begin"
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.0/mongo-cxx-driver-r3.7.0.tar.gz
tar -xzf mongo-cxx-driver-r3.7.0.tar.gz
cd mongo-cxx-driver-r3.7.0/build
echo "Download the C++ driver source ... end"

echo "Configure the C++ driver source ... begin"
cmake                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR/mongo-cxx-driver-3.7.0"   \
    -DCMAKE_PREFIX_PATH="$INSTALL_DIR/mongo-c-driver-1.23.2" \
    ..
echo "Configure the C++ driver source ... end"

echo "Install C++ driver ... begin"
cmake --build . --target install
echo "Install C++ driver ... end"
```

## Q8: What version of C driver is in Ubuntu repositories?
See https://packages.ubuntu.com/search?keywords=libmongoc-dev

## Q7: What is the request rate for AWS KMS operations?
See https://docs.aws.amazon.com/kms/latest/developerguide/requests-per-second.html

## Q6: What is the process for adding evergreen environment variables?

They can be added to https://spruce.mongodb.com/project/drivers-atlas-testing-dev/settings/variables. Evergreen variables are referenced as expansions in the config.yml. One gotcha is that an expansion (unlike shell variables) require curly braces `${foo}`. See also: https://wiki.corp.mongodb.com/pages/viewpage.action?pageId=143705019
## Q5: How can CI tasks be run?

Creating a Draft PR on GitHub will run a larger subset of tasks to run test-* tests against live servers. This can be done with the Evergreen CLI using the required alias. Here is an example command I use:
```
evergreen patch --project=mongo-c-driver --description="CDRIVER-1234 my ticket $(git log --oneline HEAD~1..HEAD)" \
    --yes --finalize \
    --alias required
```

## Q4: What is the difference between the bson_t flags: BSON_FLAG_INLINE, BSON_FLAG_STATIC, BSON_FLAG_RDONLY, BSON_FLAG_NO_FREE?

BSON_FLAG_STATIC means the bson_t struct is not freed on bson_destroy. It is independent of whether the data is heap allocated.
bson_destroy will destroy the bson_t if BSON_FLAG_STATIC is not set. bson_init sets BSON_FLAG_INLINE | BSON_FLAG_STATIC.

BSON_FLAG_INLINE means the underlying data is embedded in the bson_t. bson_impl_inline_t is used. bson_new sets BSON_FLAG_INLINE.

BSON_FLAG_RDONLY means the underlying data is not owned by the bson_t. The data should not be modified or freed. bson_init_static sets BSON_FLAG_STATIC | BSON_FLAG_RDONLY.

BSON_FLAG_NO_FREE means not to free the underlying data. BSON_FLAG_NO_FREE is used for appending a child bson_t. The child points to the parent buffer. It is not read only. bson_new_from_buffer sets BSON_FLAG_NO_FREE.

## Q3: When did libbson and libmongoc merge?
In CDRIVER-2416. C driver 1.10.0. See NEWS.

## Q2: What is the oldest version of OpenSSL libmongocrypt compiles with?
1.0.1e-fips. On the RHEL 6.2 variant.
Proof: grep logs for "openssl" of the [build-and-test-and-upload task on rhel62-small](https://spruce.mongodb.com/task/libmongocrypt_rhel_62_64_bit_build_and_test_and_upload_bee26c6e0f16ddfead9699526cc99a092dd34a29_22_02_16_18_42_47).

## Q1: How does libmongocrypt link to libbson?

See [libmongocrypt linking](https://docs.google.com/document/d/1YnRDmNoBZG_Ip4JmIw9bB4tItFt5mIBHLu4Ex73WSGY/edit#heading=h.xyup819je3bw).
Also see https://github.com/mongodb/libmongocrypt/pull/365#discussion_r896041414:

```
1. If a user wants to statically link with libmongocrypt, then:
    i. Static libmongocrypt always requires libbson definitions to be provided when CMake generates the runtime binaries.
    ii. If the user wishes to link libbson statically, then mongo::mongocrypt_static must link_libraries to a static libbson.
    iii. If the user wishes to link libbson dynamically, then mongo::mongocrypt_static must link_libraries to a dynamic libbson.
    iv. If the user doesn't care about linking to libbson for their final application: Too bad: the static libmongocrypt requires that the libbson definitions be provided. We can attempt to do this transparently for the user in mongocrypt-config.cmake, but should the default be to statically link to libbson? Or dynamically? In the current changeset, this defaults to try mongo::bson_static, or to fall back and link against a libbson-static-1.0 via find_library().
2. If the user wants to dynamically link libmongocrypt:
    i. If the libmongocrypt was built with USE_SHARED_LIBBSON, then mongo::mongocrypt requires that the final application link to a libbson dynamic library, as the mongo::mongocrypt binary contains an SONAME reference to libbson. Statically linking with libbson is not an option for the user in this case.
    ii. If the libmongocrypt was build with USE_SHARED_LIBBSON=OFF (the default), then there are no requirements on transitively linking libbson, and the user can do as they wish.
```
