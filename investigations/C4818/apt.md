```bash
docker run --rm -it debian:12
$ apt install apt-rdepends
$ apt-rdepends libmongoc-1.0-0
```

Produces:
```
apt-rdepends libmongoc-dev
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
libmongoc-dev
  Depends: libbson-dev (= 1.23.1-1+b1)
  Depends: libmongoc-1.0-0 (= 1.23.1-1+b1)
  Depends: libmongocrypt-dev
  Depends: libsasl2-dev
  Depends: libsnappy-dev
  Depends: libssl-dev
  Depends: libzstd-dev
  Depends: zlib1g-dev
libbson-dev
  Depends: libbson-1.0-0 (= 1.23.1-1+b1)
libbson-1.0-0
  Depends: libc6 (>= 2.34)
libc6
  Depends: libgcc-s1
libgcc-s1
  Depends: gcc-12-base (= 12.2.0-14)
  Depends: libc6 (>= 2.35)
gcc-12-base
libmongoc-1.0-0
  Depends: libbson-1.0-0 (>= 1.23.1)
  Depends: libc6 (>= 2.34)
  Depends: libicu72 (>= 72.1~rc-1~)
  Depends: libmongocrypt0
  Depends: libsasl2-2 (>= 2.1.28+dfsg)
  Depends: libsnappy1v5 (>= 1.1.9)
  Depends: libssl3 (>= 3.0.0)
  Depends: libzstd1 (>= 1.5.2)
  Depends: zlib1g (>= 1:1.2.0)
libicu72
  Depends: libc6 (>= 2.34)
  Depends: libgcc-s1 (>= 3.0)
  Depends: libstdc++6 (>= 12)
libstdc++6
  Depends: gcc-12-base (= 12.2.0-14)
  Depends: libc6 (>= 2.36)
  Depends: libgcc-s1 (>= 4.5)
libmongocrypt0
  Depends: libbson-1.0-0 (>= 1.23.1)
  Depends: libc6 (>= 2.34)
  Depends: libssl3 (>= 3.0.0)
libssl3
  Depends: libc6 (>= 2.34)
libsasl2-2
  Depends: libc6 (>= 2.34)
  Depends: libsasl2-modules-db (>= 2.1.28+dfsg-10)
libsasl2-modules-db
  Depends: libc6 (>= 2.17)
  Depends: libdb5.3
libdb5.3
  Depends: libc6 (>= 2.34)
libsnappy1v5
  Depends: libc6 (>= 2.17)
  Depends: libstdc++6 (>= 5.2)
libzstd1
  Depends: libc6 (>= 2.34)
zlib1g
  Depends: libc6 (>= 2.17)
libmongocrypt-dev
  Depends: libbson-dev
  Depends: libintelrdfpmath-dev (>= 2.0u2-6)
  Depends: libmongocrypt0 (= 1.7.2-1)
libintelrdfpmath-dev
libsasl2-dev
  Depends: libc-dev
  Depends: libsasl2-2 (= 2.1.28+dfsg-10)
libc-dev
libsnappy-dev
  Depends: libsnappy1v5 (= 1.1.9-3)
libssl-dev
  Depends: libssl3 (= 3.0.11-1~deb12u2)
libzstd-dev
  Depends: libzstd1 (= 1.5.4+dfsg2-5)
zlib1g-dev
  Depends: libc-dev
  Depends: libc6-dev
  Depends: zlib1g (= 1:1.2.13.dfsg-1)
libc6-dev
  Depends: libc-dev-bin (= 2.36-9+deb12u4)
  Depends: libc6 (= 2.36-9+deb12u4)
  Depends: libcrypt-dev
  Depends: libnsl-dev
  Depends: linux-libc-dev
  Depends: rpcsvc-proto
libc-dev-bin
  Depends: libc6 (<< 2.37)
libcrypt-dev
  Depends: libcrypt1 (= 1:4.4.33-2)
libcrypt1
  Depends: libc6 (>= 2.36)
libnsl-dev
  Depends: libnsl2 (= 1.3.0-2)
  Depends: libtirpc-dev
libnsl2
  Depends: libc6 (>= 2.17)
  Depends: libtirpc3 (>= 1.0.2)
libtirpc3
  Depends: libc6 (>= 2.34)
  Depends: libgssapi-krb5-2 (>= 1.17)
  Depends: libtirpc-common (>= 1.3.3+ds-1)
libgssapi-krb5-2
  Depends: libc6 (>= 2.33)
  Depends: libcom-err2 (>= 1.43.9)
  Depends: libk5crypto3 (>= 1.20)
  Depends: libkrb5-3 (= 1.20.1-2+deb12u1)
  Depends: libkrb5support0 (>= 1.15~beta1)
libcom-err2
  Depends: libc6 (>= 2.17)
libk5crypto3
  Depends: libc6 (>= 2.33)
  Depends: libkrb5support0 (>= 1.20)
libkrb5support0
  Depends: libc6 (>= 2.34)
libkrb5-3
  Depends: libc6 (>= 2.34)
  Depends: libcom-err2 (>= 1.43.9)
  Depends: libk5crypto3 (>= 1.20)
  Depends: libkeyutils1 (>= 1.5.9)
  Depends: libkrb5support0 (= 1.20.1-2+deb12u1)
  Depends: libssl3 (>= 3.0.0)
libkeyutils1
  Depends: libc6 (>= 2.17)
libtirpc-common
libtirpc-dev
  Depends: libtirpc3 (= 1.3.3+ds-1)
linux-libc-dev
rpcsvc-proto
  Depends: libc6 (>= 2.34)
```
