# Q & A
## When did libbson and libmongoc merge?
In CDRIVER-2416. C driver 1.10.0. See NEWS.

## What is the oldest version of OpenSSL libmongocrypt compiles with?
1.0.1e-fips. On the RHEL 6.2 variant.
Proof: grep logs for "openssl" of the [build-and-test-and-upload task on rhel62-small](https://spruce.mongodb.com/task/libmongocrypt_rhel_62_64_bit_build_and_test_and_upload_bee26c6e0f16ddfead9699526cc99a092dd34a29_22_02_16_18_42_47).