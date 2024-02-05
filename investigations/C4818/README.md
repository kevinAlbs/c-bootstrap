This directory includes information about MongoDB C driver dependencies.

Many dependencies in the C driver are optional and expected to be system installed rather than bundled (e.g. OpenSSL, zstd).

The C driver (as of 1.25.4) bundles the following dependencies (which are optionally used):

- zlib 1.2.13
- utf8proc 2.8.0

The C driver is available in [multiple repositories](https://mongoc.org/libmongoc/current/ref/packages.html): homebrew, Debian, Fedora, and more. Each repository has its own specification of dependencies. The dependency versions installed may differ by OS version. MongoDB owns the Debian packaging, but not the others (though we help maintain them).

This directory contains listings of dependencies of MongoDB C driver packages linked in [Package Installation Reference](https://mongoc.org/libmongoc/current/ref/packages.html) for the latest stable releases of the operating systems.

The Debian packaging is owned by MongoDB. The others are community maintained (though MongoDB employees sometimes contribute).



