```bash
docker run --rm -it archlinux
$ pacman install pactree
$ pacman -Syu
$ pacman -S pacman-contrib
$ pacman -S mongo-c-driver
$ pactree mongo-c-driver
```

Produces:
```
mongo-c-driver
├─libsasl
│ ├─gdbm
│ │ ├─glibc
│ │ │ ├─linux-api-headers>=4.10
│ │ │ ├─tzdata
│ │ │ └─filesystem
│ │ │   └─iana-etc
│ │ ├─bash provides sh
│ │ │ ├─readline
│ │ │ │ ├─glibc
│ │ │ │ ├─ncurses
│ │ │ │ │ ├─glibc
│ │ │ │ │ └─gcc-libs
│ │ │ │ │   └─glibc>=2.27
│ │ │ │ └─ncurses provides libncursesw.so=6-64
│ │ │ ├─readline provides libreadline.so=8-64
│ │ │ ├─glibc
│ │ │ └─ncurses
│ │ ├─readline
│ │ └─readline provides libreadline.so=8-64
│ ├─gdbm provides libgdbm.so=6-64
│ ├─glibc
│ └─openssl
│   └─glibc
├─snappy
│ ├─glibc
│ └─gcc-libs
└─zstd
  ├─glibc
  ├─gcc-libs
  ├─zlib
  │ └─glibc
  ├─xz
  │ └─bash provides sh
  └─lz4
    └─glibc
```
