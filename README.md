# Q & A
## When did libbson and libmongoc merge?
In CDRIVER-2416. C driver 1.10.0. See NEWS.

## What is the oldest version of OpenSSL libmongocrypt compiles with?
1.0.1e-fips. On the RHEL 6.2 variant.
Proof: grep logs for "openssl" of the [build-and-test-and-upload task on rhel62-small](https://spruce.mongodb.com/task/libmongocrypt_rhel_62_64_bit_build_and_test_and_upload_bee26c6e0f16ddfead9699526cc99a092dd34a29_22_02_16_18_42_47).

## How does libmongocrypt link to libbson?

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