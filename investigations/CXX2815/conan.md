```bash
curl -o mongo-cxx-driver.py https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/mongo-cxx-driver/all/conanfile.py
conan graph info mongo-cxx-driver.py
```

Produces
```
======== Computing dependency graph ========
Graph root
    mongo-cxx-driver.py (mongo-cxx-driver/None): /Users/kevin.albertson/code/vcpkg/mongo-cxx-driver.py
Requirements
    boost/1.82.0#c7d44b59c1738ecd5db1679ed58e9ce7 - Cache
    bzip2/1.0.8#457c272f7da34cb9c67456dd217d36c4 - Cache
    icu/73.2#2ac6430d9b0164fb3da98b705c65b3be - Cache
    libbacktrace/cci.20210118#ec1aa63bbc10145c6a299e68e711670c - Cache
    libiconv/1.17#73fefc1b696e069df90fd1d18aa63edd - Cache
    mongo-c-driver/1.24.3#992e062cdefd12d295e6117e0b74d270 - Cache
    openssl/3.2.1#ff3c0b15c72c3be49969acdf20ee345d - Cache
    snappy/1.1.10#916523630083f6d855cb2977de8eefb6 - Cache
    zlib/1.2.11#fca992a7d96a1b92bd956caa8a97d18f - Cache
    zstd/1.5.5#b87dc3b185caa4b122979ac4ae8ef7e8 - Cache
Build requirements
    b2/4.10.1#1b290403d8648c79f468f5a6496f829a - Cache
    meson/1.2.2#aace9dcc1db58fa42ecb5292f724092d - Cache
    ninja/1.11.1#77587f8c8318662ac8e5a7867eb4be21 - Cache
    pkgconf/2.0.3#f996677e96e61e6552d85e83756c328b - Cache
Resolved version ranges
    openssl/[>=1.1 <4]: openssl/3.2.1
    zlib/[>=1.2.11 <2]: zlib/1.2.11
```
