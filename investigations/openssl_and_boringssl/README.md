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