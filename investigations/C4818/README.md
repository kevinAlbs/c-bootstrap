This directory includes information about MongoDB C driver dependencies.

The C driver [install instructions](https://mongoc.org/libmongoc/current/learn/get/index.html) describe installing from package managers and from source.

## When installing via package manager

The C driver is available in [multiple repositories](https://mongoc.org/libmongoc/current/ref/packages.html): homebrew, Debian, Fedora, and more. Each repository has its own specification of dependencies. The dependency versions installed may differ by OS version. MongoDB owns the Debian packaging, but not the others (though we help maintain them).

This directory contains listings of dependencies of MongoDB C driver packages linked in [Package Installation Reference](https://mongoc.org/libmongoc/current/ref/packages.html) for the latest stable releases of the operating systems.

The Debian packaging is owned by MongoDB. The others are community maintained (though MongoDB employees sometimes contribute).

As of CDRIVER-4680, libicu is no longer a required depencency. Though some package managers still list libicu.

## Bundled dependencies

Many dependencies in the C driver are optional and expected to be system installed rather than bundled (e.g. OpenSSL, zstd). However, the C driver (as of 1.25.4) bundles the following dependencies (which are optionally used):

- zlib 1.2.13
- utf8proc 2.8.0

## When installing from source

When building from source, the C driver can be configured with most dependencies disabled. This is intended to make the C driver portable among various package repositories.
From surveying the package managers, and reading the CMakeLists.txt, here is a list of of optional (though likely commonly used) runtime dependencies of the C driver:

- OpenSSL or libressl
- zlib
- zstd (>=0.8.0)
- snappy
- Cyrus SASL (>=2.0) (non-Windows) or SSPI (Windows)
- libutf8proc (replaces former libicu dependency)

Many requirements do not include version constraints.
