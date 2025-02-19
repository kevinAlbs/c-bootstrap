# Q4: How long are data encryption keys (DEKs) cached?
A: The DEK is cached one minute after fetching. The time is not configurable. Using the key does not extend the lifetime.

# Q3: How much size increase is IntelDFP?
A: IntelDFP was introduced in libmongocrypt 1.7.0 as part of https://jira.mongodb.org/browse/MONGOCRYPT-483. The size increase is about 3.5 MB. It can be disabled with the CMake option `-DMONGOCRYPT_ENABLE_DECIMAL128=OFF`.

```
% ls -lh ./.install-1.7.2-RelWithDebInfo/lib/libmongocrypt.0.0.0.dylib
-rwxr-xr-x  1 kevin.albertson  staff   4.3M Sep 15 10:54 ./.install-1.7.2-RelWithDebInfo/lib/libmongocrypt.0.0.0.dylib
% ls -lh ./.install-1.7.2-RelWithDebInfo-nodecimal/lib/libmongocrypt.0.0.0.dylib
-rwxr-xr-x  1 kevin.albertson  staff   595K Sep 15 10:49 ./.install-1.7.2-RelWithDebInfo-nodecimal/lib/libmongocrypt.0.0.0.dylib
```

# Q2: What is the feature called?

The umbrella term is "In-Use" encryption. It consists of "Client-Side Field Level Encryption" and "Queryable Encryption". 

These terms are used in the [MongoDB documentation](https://www.mongodb.com/docs/compass/current/in-use-encryption-tutorial/)

> In-Use Encryption allows you to connect to your deployments using Queryable Encryption. This connection method allows you to encrypt a subset of fields in your collections.
> You can also use CSFLE to encrypt a subset of fields in your collection. CSFLE encryption is enabled through the schema editor.

CSFLE consists of these algorithms:
- AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic
- AEAD_AES_256_CBC_HMAC_SHA_512-Random

QE consists of these algorithms:
- Indexed
- Unindexed
- RangePreview

# Q1: Does query analysis support $project?
    A: It is pending https://jira.mongodb.org/browse/SERVER-65296.
