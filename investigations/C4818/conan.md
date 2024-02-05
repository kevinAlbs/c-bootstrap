```bash
curl -o mongo-c-driver.py https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/mongo-c-driver/all/conanfile.py
conan graph info mongo-c-driver.py
```

Produces
```
======== Computing dependency graph ========
Graph root
    mongo-c-driver.py (mongo-c-driver/None): /Users/kevin.albertson/code/tasks/mongo-c-driver-C4818/mongo-c-driver.py
Requirements
    icu/73.2#2ac6430d9b0164fb3da98b705c65b3be - Cache
    openssl/3.2.1#ff3c0b15c72c3be49969acdf20ee345d - Cache
    snappy/1.1.10#916523630083f6d855cb2977de8eefb6 - Cache
    zlib/1.2.11#fca992a7d96a1b92bd956caa8a97d18f - Cache
    zstd/1.5.5#b87dc3b185caa4b122979ac4ae8ef7e8 - Cache
Build requirements
    meson/1.2.2#aace9dcc1db58fa42ecb5292f724092d - Cache
    ninja/1.11.1#77587f8c8318662ac8e5a7867eb4be21 - Cache
    pkgconf/2.0.3#f996677e96e61e6552d85e83756c328b - Cache
Resolved version ranges
    openssl/[>=1.1 <4]: openssl/3.2.1
    zlib/[>=1.2.11 <2]: zlib/1.2.11
```
