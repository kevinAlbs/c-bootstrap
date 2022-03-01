Q: What is the error when attempting to link to Tensorflow with BoringSSL and libmongoc with OpenSSL?
    A: 

I ran a small experiment with stub libraries.

`app` links to `tensorflow.so` and `mongoc.so`
    `tensorflow.so` links to `boringssl.a`
    `mongoc.so` links to `openssl.so`

`boringssl.a` and `openssl.so` define the same function `ssn_fn`. 

`tensorflow.so` and `mongoc.so` define functions that call `ssl_fn`.

`app` calls a function in `tensorflow.so` and `mongoc.so`. Both calls use the `ssl_fn` defined in `boringssl.a`.

Here is sample output:
```
poc_tensorflow_fn... begin
 poc_boringssl.c :: poc_ssl_fn ... begin
 poc_boringssl.c :: poc_ssl_fn ... end
poc_tensorflow_fn... end
poc_mongoc_fn... begin
 poc_boringssl.c :: poc_ssl_fn ... begin
 poc_boringssl.c :: poc_ssl_fn ... end
poc_mongoc_fn... end
```