# Repro
Install BoringSSL
```
install_path=$(pwd)/boringssl-3945
git clone https://boringssl.googlesource.com/boringssl --branch 3945 boringssl-src
cd boringssl-src
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=1 -DCMAKE_INSTALL_PREFIX="$install_path" ..
cmake --build . --target all -- -j8
cd ..
mkdir build-with-default-symver
cd build-with-default-symver
cmake -DBUILD_SHARED_LIBS=1 -DCMAKE_INSTALL_PREFIX="$install_path" -DCMAKE_C_FLAGS="-Wl,--default-symver" ..
cmake --build . --target all -- -j8
```


I built a repro.
    `main.out` dynamically links to the C driver.
    `main.out` dynamically links to BoringSSL.
    The C driver dynamically links to OpenSSL.

At runtime it crashes in the C driver:

```
Starting program: /home/ubuntu/code/c-bootstrap/investigations/openssl_and_boringssl/repro/cmake-build/main.out
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Program received signal SIGSEGV, Segmentation fault.
0x00007ffff77a5512 in OPENSSL_strcasecmp (a=0x0, b=0x7ffff7888d41 "rc4") at /home/ubuntu/code/c-bootstrap/install/boringssl-src/crypto/mem.c:232
232	    const int aa = OPENSSL_tolower(a[i]);
(gdb) bt
#0  0x00007ffff77a5512 in OPENSSL_strcasecmp (a=0x0, b=0x7ffff7888d41 "rc4") at /home/ubuntu/code/c-bootstrap/install/boringssl-src/crypto/mem.c:232
#1  0x00007ffff777d167 in EVP_get_cipherbyname (name=0x0) at /home/ubuntu/code/c-bootstrap/install/boringssl-src/crypto/cipher_extra/cipher_extra.c:92
#2  0x00007ffff6c7ed28 in ?? () from /usr/lib/x86_64-linux-gnu/libssl.so.1.1
#3  0x00007ffff6c81f6c in ?? () from /usr/lib/x86_64-linux-gnu/libssl.so.1.1
#4  0x00007ffff56a1907 in __pthread_once_slow (once_control=0x7ffff6eda928, init_routine=0x7ffff6c81d90) at pthread_once.c:116
#5  0x00007ffff69629f9 in CRYPTO_THREAD_run_once () from /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1
#6  0x00007ffff6c8209b in OPENSSL_init_ssl () from /usr/lib/x86_64-linux-gnu/libssl.so.1.1
#7  0x00007ffff7b9d94a in _mongoc_openssl_init () at /home/ubuntu/code/c-bootstrap/tmp/mongo-c-driver/src/libmongoc/src/mongoc/mongoc-openssl.c:77
#8  0x00007ffff7b62f39 in _mongoc_do_init () at /home/ubuntu/code/c-bootstrap/tmp/mongo-c-driver/src/libmongoc/src/mongoc/mongoc-init.c:101
#9  0x00007ffff56a1907 in __pthread_once_slow (once_control=0x7ffff7dd2ab8 <once>, init_routine=0x7ffff7b62f30 <_mongoc_do_init>) at pthread_once.c:116
#10 0x0000555555554d54 in main () at /home/ubuntu/code/c-bootstrap/investigations/openssl_and_boringssl/repro/main.c:20
```

After building BoringSSL with `--default-symver`, the crash no longer appears:

```
git clone https://boringssl.googlesource.com/boringssl
cd boringssl
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=1 -DCMAKE_C_FLAGS="-Wl,--default-symver" ..
cmake --build . --target all
```

# Example 4
Q: Does passing --default-symver resolve the conflict in Example 2?
    A: Yes. Under some conditions.
    My experiment only resolved when:
        - poc_boringssl.out was dynamically linked.
        - poc_boringssl.out and poc_openssl.out were built with the linker flag `--default-symver`.

# Example 3
Q: What does -fvisibility=hidden do?
    A: Sets the default symbol visibility for all global declarations.
# Example 2
Q: What is the error when attempting to link to Tensorflow with BoringSSL and libmongoc with OpenSSL?
    A: When we try to use mongocxx driver to make a ssl call, we get this
*** Aborted at 1646157175 (unix time) try "date -d @1646157175" if you are using GNU date ***
PC: @                0x0 (unknown)
*** SIGSEGV (@0x0) received by PID 1098676 (TID 0x7f0613444840) from PID 0; stack trace: ***
    @     0x7f061583d3c0 (unknown)
    @     0x7f06153199fc OPENSSL_sk_dup
    @     0x7f061544e2c0 SSL_new
    @     0x7f061543122b BIO_new_ssl
    @     0x7f0615ace77b mongoc_stream_tls_openssl_new
    @     0x7f0615abd120 _mongoc_topology_scanner_tcp_initiate
    @     0x7f0615a770ac _mongoc_async_cmd_phase_initiate
    @     0x7f0615a76b17 mongoc_async_cmd_run
    @     0x7f0615a76743 mongoc_async_run
    @     0x7f0615abda11 mongoc_topology_scanner_work
    @     0x7f0615ab7a30 _mongoc_topology_do_blocking_scan
    @     0x7f0615ab7d77 mongoc_topology_select_server_id
    @     0x7f0615a837d1 _mongoc_cluster_stream_for_optype
    @     0x7f0615a79a44 mongoc_bulk_operation_execute
    @     0x7f0615b65b40 mongocxx::v0::bulk_write::execute()
    @     0x7f0615b70205 mongocxx::v0::collection::_insert_one()
    @     0x7f0615b70705 mongocxx::v0::collection::insert_one()
    @     0x557d0b765dfc main
    @     0x7f06154ea0b3 __libc_start_main
    @     0x557d0b76592e _start
    @                0x0 (unknown)

I ran a small experiment with stub libraries to try to reproduce a similar problematic scenario. I think I have one.

`app` links to `tensorflow.so` and `mongoc.so`
    `tensorflow.so` links to `boringssl.a`
    `mongoc.so` links to `openssl.so`

`boringssl.a` and `openssl.so` define the same function `ssn_fn`. 

`tensorflow.so` and `mongoc.so` define functions that call `ssl_fn`.

`app` calls a function in `tensorflow.so` and `mongoc.so`.

With the GNU linker, compilation and linking succeeds. But both calls use the `ssl_fn` defined in `boringssl.a`.

I have some possible solutions to investigate (e.g. a linker script).

But I have a few open questions:

1. Does the applications that dynamically link to the C/C++ driver also link to Tensorflow (or something else) that statically links to BoringSSL?
2. What error occurs when linking to the C/C++ driver?
3. Have the suggestions listed here been tried? https://boringssl.googlesource.com/boringssl/+/HEAD/INCORPORATING.md#symbols



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

# Example 1
Q: What happens if two files in the same executable have the same static symbol?
    A: nm reports two symbols with "t". "t" indicates a local symbol.