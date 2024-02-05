```bash
docker run --rm -it alpine:3.19
$ apk add mongo-c-driver-dev
$ apk dot mongo-c-driver-dev
```

Produces:
```
/ # apk dot mongo-c-driver-dev
digraph "apkindex" {
  rankdir=LR;
  node [shape=box];
  "libbson-1.23.4-r0" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libbson-dev-1.23.4-r0" -> "libbson-1.23.4-r0"[];
  "pkgconf-1.9.5-r0" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libbson-dev-1.23.4-r0" -> "pkgconf-1.9.5-r0"[arrowhead=inv,label="pkgconfig",];
  "mongo-c-driver-dev-1.23.4-r0" -> "libbson-dev-1.23.4-r0"[];
  "mongo-c-driver-1.23.4-r0" -> "libbson-1.23.4-r0"[arrowhead=inv,label="so:libbson-1.0.so.0",];
  "mongo-c-driver-1.23.4-r0" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libcrypto3-3.1.4-r5" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "mongo-c-driver-1.23.4-r0" -> "libcrypto3-3.1.4-r5"[arrowhead=inv,label="so:libcrypto.so.3",];
  "snappy-1.1.10-r1" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libstdc++-12.2.1_git20220924-r10" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libgcc-12.2.1_git20220924-r10" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libstdc++-12.2.1_git20220924-r10" -> "libgcc-12.2.1_git20220924-r10"[arrowhead=inv,label="so:libgcc_s.so.1",];
  "snappy-1.1.10-r1" -> "libstdc++-12.2.1_git20220924-r10"[arrowhead=inv,label="so:libstdc++.so.6",];
  "mongo-c-driver-1.23.4-r0" -> "snappy-1.1.10-r1"[arrowhead=inv,label="so:libsnappy.so.1",];
  "libssl3-3.1.4-r5" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "libssl3-3.1.4-r5" -> "libcrypto3-3.1.4-r5"[arrowhead=inv,label="so:libcrypto.so.3",];
  "mongo-c-driver-1.23.4-r0" -> "libssl3-3.1.4-r5"[arrowhead=inv,label="so:libssl.so.3",];
  "zlib-1.2.13-r1" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "mongo-c-driver-1.23.4-r0" -> "zlib-1.2.13-r1"[arrowhead=inv,label="so:libz.so.1",];
  "zstd-libs-1.5.5-r4" -> "musl-1.2.4-r2"[arrowhead=inv,label="so:libc.musl-aarch64.so.1",];
  "mongo-c-driver-1.23.4-r0" -> "zstd-libs-1.5.5-r4"[arrowhead=inv,label="so:libzstd.so.1",];
  "mongo-c-driver-dev-1.23.4-r0" -> "mongo-c-driver-1.23.4-r0"[];
  "mongo-c-driver-dev-1.23.4-r0" -> "libbson-dev-1.23.4-r0"[arrowhead=inv,label="pc:libbson-1.0",];
  "mongo-c-driver-dev-1.23.4-r0" -> "pkgconf-1.9.5-r0"[arrowhead=inv,label="pkgconfig",];
}
```
