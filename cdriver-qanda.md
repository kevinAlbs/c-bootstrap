# C Driver Q&A

## Q18: When is it preferable to create a ticket, rather than submit a PR?
A:

Create a ticket for any change that may impact downstream users.

Changes impacting downstream users may include:
- Build changes that may result in the PHP driver needing to update their autotools build.
- Any non-test code change. This may result in an unexpected build or behavior change.

Rationale:
- A ticket includes a "Fix Version/s". The "Fix Version/s" may help identify which C driver version a change is applied.
- A ticket may help to group all related commits. If the ticket identifier (e.g. "CDRIVER-????") is in the commit message, the "Githook User" will automatically add a comment linking to the commit ([example](https://jira.mongodb.org/browse/CDRIVER-4691?focusedCommentId=5641449&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-5641449)). This is helpful to identify commits that have been cherry-picked on to release branches.

When in doubt, err towards creating a ticket. The ticket can be minimal with the description: `Tracks PR <PR link>`. Example: https://jira.mongodb.org/browse/CDRIVER-4709

## Q17: What do I put for the fixVersion when closing a ticket?

A: If this is a bug fix, consider backporting to the most recent release branch
To backport, cherry pick the commit onto the minor version branch. Example: a fix planned for the `1.23.6` patch release would be cherry-picked onto the branch `r1.23`.

Otherwise, put the fixVersion of the next upcoming minor release. Fix versions are listed on the project Releases pages in Jira: [CDRIVER](https://jira.mongodb.org/projects/CDRIVER?selectedItem=com.atlassian.jira.jira-projects-plugin%3Arelease-page&status=unreleased_).

When in doubt, ask in #dbx-c-cxx.

## Q16: How do I run the `/client_side_encryption/*` tests?
A:

Running the `/client_side_encryption/*` tests in `test-libmongoc` requires additional steps. The C driver evergreen configuration may be the best reference. But here are proposed steps for testing locally:

### Build

The C driver requires `libmongocrypt` to enable In-Use Encryption. Install libmongocrypt:

```sh
# Install version 1.8.1
git clone git@github.com:mongodb/libmongocrypt.git --branch 1.8.1
cd libmongocrypt
cmake -DCMAKE_INSTALL_PREFIX=/Users/kevin.albertson/install/libmongocrypt-1.8.1 -S. -Bcmake-build
cmake --build cmake-build --target install
```

Configure the C driver with `-DENABLE_CLIENT_SIDE_ENCRYPTION=ON` to require `libmongocrypt` be found:
```
cd mongo-c-driver
cmake \
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/install/libmongocrypt-1.8.1 \
    -DENABLE_CLIENT_SIDE_ENCRYPTION=ON \
    -S. -Bcmake-build
```

Build `test-libmongoc`:

```
cmake --build cmake-build --target test-libmongoc -- -j16
```

### Set Environment Variables

Tests require credentials to AWS, GCP, and Azure test instances. These credentials are sensitive. Copy them from the C driver Evergreen Project Variables page: https://spruce.mongodb.com/project/mongo-c-driver/settings/variables. The variables start with `client_side_encryption_*`. To make setting the environment variables easy, I suggest creating a `kms_providers.json` file in a local directory. Here is the redacted contents of my `~/.csfle/kms_provider.json`:

```json
{
    "aws": {
        "accessKeyId": "(sensitive - copy from Evergreen Project Variables)",
        "secretAccessKey": "(sensitive - copy from Evergreen Project Variables)"
    },
    "azure": {
        "tenantId": "(sensitive - copy from Evergreen Project Variables)",
        "clientId": "(sensitive - copy from Evergreen Project Variables)",
        "clientSecret": "(sensitive - copy from Evergreen Project Variables)"
    },
    "gcp": {
        "email": "(sensitive - copy from Evergreen Project Variables)",
        "privateKey": "(sensitive - copy from Evergreen Project Variables)"
    }
}
```

Set environment variables from the kms_providers.json with a script:

```sh
#!/bin/bash
export AWS_ACCESS_KEY_ID=$(cat ~/.csfle/kms_providers.json | jq .aws.accessKeyId -r)
export AWS_SECRET_ACCESS_KEY=$(cat ~/.csfle/kms_providers.json | jq .aws.secretAccessKey -r)

# Set DRIVERS_TOOLS to a local checkout of: https://github.com/mongodb-labs/drivers-evergreen-tools
DRIVERS_TOOLS=~/code/drivers-evergreen-tools
# set-temp-creds.sh requires boto3 installed. Create a virtual environment and install with: `pip install boto3`.
. ${DRIVERS_TOOLS}/.evergreen/csfle/set-temp-creds.sh

# Export variables for testing the C driver.
export MONGOC_TEST_AWS_TEMP_SECRET_ACCESS_KEY="$CSFLE_AWS_TEMP_ACCESS_KEY_ID"
export MONGOC_TEST_AWS_TEMP_ACCESS_KEY_ID="$CSFLE_AWS_TEMP_SECRET_ACCESS_KEY"
export MONGOC_TEST_AWS_TEMP_SESSION_TOKEN="$CSFLE_AWS_TEMP_SESSION_TOKEN"
export MONGOC_TEST_AWS_SECRET_ACCESS_KEY=$AWS_SECRET_ACCESS_KEY
export MONGOC_TEST_AWS_ACCESS_KEY_ID=$AWS_ACCESS_KEY_ID
export MONGOC_TEST_AZURE_TENANT_ID=$(cat ~/.csfle/kms_providers.json | jq .azure.tenantId -r)
export MONGOC_TEST_AZURE_CLIENT_ID=$(cat ~/.csfle/kms_providers.json | jq .azure.clientId -r)
export MONGOC_TEST_AZURE_CLIENT_SECRET=$(cat ~/.csfle/kms_providers.json | jq .azure.clientSecret -r)
export MONGOC_TEST_GCP_EMAIL=$(cat ~/.csfle/kms_providers.json | jq .gcp.email -r)
export MONGOC_TEST_GCP_PRIVATEKEY=$(cat ~/.csfle/kms_providers.json | jq .gcp.privateKey -r)
export MONGOC_TEST_CSFLE_TLS_CA_FILE="$DRIVERS_TOOLS/.evergreen/x509gen/ca.pem"
export MONGOC_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE="$DRIVERS_TOOLS/.evergreen/x509gen/client.pem"
```

If libmongocrypt was installed to a non-standard directory (as shown above), the runtime loader needs to be informed where to find the libmongocrypt shared library. On macOS: set `DYLD_LIBRARY_PATH` to the installed `lib` directory:
```sh
export DYLD_LIBRARY_PATH="/Users/kevin.albertson/install/libmongocrypt-1.8.1/lib" 
```
On Linux, set `LD_LIBRARY_PATH` to the path of libmongocrypt.so.

Tests require the use of a Query Analysis component. The Query Analysis component is either the `crypt_shared` library or `mongocryptd` process.
Testing with `crypt_shared` is [recommended in the specifications](https://github.com/mongodb/specifications/tree/840e6d49c354656bff11b2622f0d3001b39d9403/source/client-side-encryption/tests#using-crypt-shared).
Download `crypt_shared` from the "Crypt Shared" links of https://www.mongodb.com/download-center/enterprise/releases
Export the variable of the path to `crypt_shared`. Example:
```sh
export MONGOC_TEST_CRYPT_SHARED_LIB_PATH=/Users/kevin.albertson/bin/mongodl/crypt_shared/7.0.0-rc0/lib/mongo_crypt_v1.dylib
```

### Test

Run mongod version 4.2 or newer.

Run one test:
```sh
./cmake-build/src/libmongoc/test-libmongoc -d --no-fork --match "/client_side_encryption/legacy/basic"
```

Expect output to resemble:
```
Begin /client_side_encryption/legacy/basic, seed 1221838283
  - Insert with deterministic encryption, then find it
  - Insert with randomized encryption, then find it
    { "status": "pass", "test_file": "/client_side_encryption/legacy/basic", "seed": "1221838283", "start": 2647.612546, "end": 2654.986624, "elapsed": 7.374078  }
```


## Q15: What is the difference between `mongoc_client{_,_read,_write,_read_write}_command_with_opts` and `mongoc_client_command_simple`?
A: (Open)

## Q14: Do retryable writes and retryable reads apply to `mongoc_client{_,_read,_write,_read_write}_command_with_opts`?

A:
Retryable writes do not apply.
Retryable reads apply to `mongoc_client_read_command_with_opts`

## Q13: What is the difference between `mongoc_client_write_command_with_opts` and `mongoc_client_read_command_with_opts`?
A: (Open)

## Q12: What version of POSIX does the C driver confirm to?
A: POSIX.1-2008 plus the XSI extension and BSD-derived definitions. See https://github.com/mongodb/mongo-c-driver/blob/2a24fef34e985770abbe30794edb243079ea0cc8/CMakeLists.txt#L267


## Q11: How many connections are expected?
A:

[Connection Pooling](https://mongoc.org/libmongoc/current/connection-pooling.html) describes the two modes of connection handling (singled-threaded and pooled). Here is more information about expected connection counts:

A single-threaded `mongoc_client_t` creates one connection per server. These connections are used both for monitoring and operations.

A `mongoc_client_pool_t` pools many `mongoc_client_t` and shares connections for monitoring. One or two connections per server are created for monitoring depending on the server version. For server version < 4.4, one monitoring connection per server is created. For server version >= 4.4, two monitoring connections per server are created. Each `mongoc_client_t` from the pool opens its own connection per server for operations. Connections for operations are created on-demand. E.g. if the application has not performed any operations, only the monitoring connections are created. The maximum number of `mongoc_client_t` in the pool is bounded by the configurable `maxPoolSize` option (default 100). For S servers, the maximum number of connections to a 4.4+ MongoDB cluster is: (2 * S) + (maxPoolSize * S).

Configuring `maxPoolSize` can be done in the URI. See [mongoc_uri_t](https://mongoc.org/libmongoc/current/mongoc_uri_t.html#connection-pool-options) documentation.

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
