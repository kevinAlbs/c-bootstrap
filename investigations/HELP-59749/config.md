Use [parse_handshake_cfg.py](https://github.com/mongodb/mongo-c-driver/blob/9fe27e912525ca127ef889660cb9ebfa802bbe5c/src/libmongoc/examples/parse_handshake_cfg.py) to decode the handshake config:

```
% python ./src/libmongoc/examples/parse_handshake_cfg.py 0x06235620c9
Decimal value: 26362650825
MONGOC_MD_FLAG_ENABLE_CRYPTO                      : true
MONGOC_MD_FLAG_ENABLE_CRYPTO_CNG                  : false
MONGOC_MD_FLAG_ENABLE_CRYPTO_COMMON_CRYPTO        : false
MONGOC_MD_FLAG_ENABLE_CRYPTO_LIBCRYPTO            : true
MONGOC_MD_FLAG_ENABLE_CRYPTO_SYSTEM_PROFILE       : false
MONGOC_MD_FLAG_ENABLE_SASL                        : false
MONGOC_MD_FLAG_ENABLE_SSL                         : true
MONGOC_MD_FLAG_ENABLE_SSL_OPENSSL                 : true
MONGOC_MD_FLAG_ENABLE_SSL_SECURE_CHANNEL          : false
MONGOC_MD_FLAG_ENABLE_SSL_SECURE_TRANSPORT        : false
MONGOC_MD_FLAG_EXPERIMENTAL_FEATURES              : false
MONGOC_MD_FLAG_HAVE_SASL_CLIENT_DONE              : false
MONGOC_MD_FLAG_HAVE_WEAK_SYMBOLS                  : false
MONGOC_MD_FLAG_NO_AUTOMATIC_GLOBALS               : true
MONGOC_MD_FLAG_ENABLE_SSL_LIBRESSL                : false
MONGOC_MD_FLAG_ENABLE_SASL_CYRUS                  : false
MONGOC_MD_FLAG_ENABLE_SASL_SSPI                   : false
MONGOC_MD_FLAG_HAVE_SOCKLEN                       : true
MONGOC_MD_FLAG_ENABLE_COMPRESSION                 : true
MONGOC_MD_FLAG_ENABLE_COMPRESSION_SNAPPY          : false
MONGOC_MD_FLAG_ENABLE_COMPRESSION_ZLIB            : true
MONGOC_MD_FLAG_ENABLE_SASL_GSSAPI                 : false
MONGOC_MD_FLAG_ENABLE_RES_NSEARCH                 : true
MONGOC_MD_FLAG_ENABLE_RES_NDESTROY                : false
MONGOC_MD_FLAG_ENABLE_RES_NCLOSE                  : true
MONGOC_MD_FLAG_ENABLE_RES_SEARCH                  : true
MONGOC_MD_FLAG_ENABLE_DNSAPI                      : false
MONGOC_MD_FLAG_ENABLE_RDTSCP                      : false
MONGOC_MD_FLAG_HAVE_SCHED_GETCPU                  : false
MONGOC_MD_FLAG_ENABLE_SHM_COUNTERS                : true
MONGOC_MD_FLAG_TRACE                              : false
MONGOC_MD_FLAG_ENABLE_ICU                         : false
MONGOC_MD_FLAG_ENABLE_CLIENT_SIDE_ENCRYPTION      : false
MONGOC_MD_FLAG_ENABLE_MONGODB_AWS_AUTH            : true
MONGOC_MD_FLAG_ENABLE_SRV                         : true
```


The handshake config is included in logs. Example:

```
{"t":{"$date":"2024-05-19T13:06:41.617+08:00"},"s":"I",  "c":"NETWORK",  "id":51800,   "ctx":"conn10488478","msg":"client metadata","attr":{"remote":"(REDACTED)","client":"conn10488478","doc":{"driver":{"name":"mongoc","version":"1.25.4"},"os":{"type":"Linux","name":"Anolis OS","version":"8.8","architecture":"x86_64"},"platform":"cfg=0x06235620c9 posix=200809 stdc=199901 CC=GCC 11.3.0 CFLAGS=\"\" LDFLAGS=\"\"","mongos":{"host":"(REDACTED)","client":"(REDACTED)","version":"6.0.13"}}}}
```
