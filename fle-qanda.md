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
