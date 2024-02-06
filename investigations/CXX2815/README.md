This directory includes information about MongoDB C++ driver dependencies.

The C++ driver [install instructions](https://mongocxx.org/mongocxx-v3/installation/) describe installing from package managers and from source.

The C++ driver wraps the C driver.

## When installing via package manager

The C++ driver is available in homebrew, vc-pkg, and Conan. Each repository has its own specification of dependencies. The dependency versions installed may differ by OS version. MongoDB does not own the packages (though we may contribute).

## When installing from source

The C++ driver optionally requires a C++17 polyfill when building from source with compilers that do not have C++17 support. This polyfill may be satisfied by Boost or MNMLSTC.

Many requirements do not include version constraints.
