# This spec file is intended to build an RPM on RHEL 7.
# It is copied and modified from: https://src.fedoraproject.org/rpms/mongo-c-driver/blob/rawhide/f/mongo-c-driver.spec

%global gh_owner     mongodb
%global gh_project   mongo-c-driver
%global libname      libmongoc
%global libver       1.0
%global up_version   1.25.4

Name:      mongo-c-driver
Summary:   Client library written in C for MongoDB
Version:   %{up_version}%{?up_prever:~%{up_prever}}
Release:   1%{?dist}
# See THIRD_PARTY_NOTICES
License:   Apache-2.0 AND ISC AND MIT AND Zlib
URL:       https://github.com/%{gh_owner}/%{gh_project}

Source0:   https://github.com/%{gh_owner}/%{gh_project}/archive/refs/tags/%{up_version}%{?up_prever:-%{up_prever}}.tar.gz

# There is notably no `BuildRequires: cmake`. RHEL 7 does not have a cmake package >= 3.15. Install cmake separately before running `rpmbuild`.
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: make
# pkg-config may pull compat-openssl10
BuildRequires: openssl-devel
BuildRequires: pkgconfig(libsasl2)
BuildRequires: pkgconfig(zlib)
BuildRequires: snappy-devel
BuildRequires: pkgconfig(libzstd)

Requires:   pkgconfig(libzstd)

%description
%{name} is a client library written in C for MongoDB.


%prep
%setup -q -n %{gh_project}-%{up_version}%{?up_prever:-%{up_prever}}


%build
cmake \
    -DBUILD_VERSION=%{up_version}%{?up_prever:-%{up_prever}} \
    -DCMAKE_INSTALL_PREFIX:PATH=/usr \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DENABLE_MONGOC:BOOL=ON \
    -DENABLE_SHM_COUNTERS:BOOL=ON \
    -DENABLE_SSL:STRING=OPENSSL \
    -DENABLE_SASL:STRING=CYRUS \
    -DENABLE_MONGODB_AWS_AUTH:STRING=ON \
    -DENABLE_AUTOMATIC_INIT_AND_CLEANUP:BOOL=OFF \
    -DENABLE_CRYPTO_SYSTEM_PROFILE:BOOL=ON \
    -DENABLE_MAN_PAGES:BOOL=OFF \
    -DENABLE_SHARED:BOOL=ON \
    -DENABLE_STATIC:STRING=OFF \
    -DENABLE_ZLIB:STRING=SYSTEM \
    -DENABLE_ZSTD:STRING=ON \
    -DENABLE_SNAPPY:STRING=ON \
    -DENABLE_TESTS:BOOL=OFF \
    -DENABLE_EXAMPLES:BOOL=OFF \
    -DENABLE_UNINSTALL:BOOL=OFF \
    -DENABLE_CLIENT_SIDE_ENCRYPTION:BOOL=OFF \
    -DCMAKE_SKIP_RPATH:BOOL=ON \
    -DUSE_BUNDLED_UTF8PROC:BOOL=ON \
    -DENABLE_SRV:BOOL=ON \
    -DENABLE_MONGODB_AWS_AUTH:STRING=ON \
    -S .

make


%install
make install DESTDIR=%{buildroot}

: Static library
rm -f  %{buildroot}%{_libdir}/*.a
rm -rf %{buildroot}%{_libdir}/cmake/*static*
rm -rf %{buildroot}%{_libdir}/pkgconfig/*static*
: Documentation
rm -rf %{buildroot}%{_datadir}/%{name}


%files
%{_bindir}/mongoc-stat
%{!?_licensedir:%global license %%doc}
%license COPYING
%license THIRD_PARTY_NOTICES
%{_libdir}/%{libname}-%{libver}.so.*
%doc src/%{libname}/examples
%doc NEWS
%{_includedir}/%{libname}-%{libver}
%{_libdir}/%{libname}-%{libver}.so
%{_libdir}/pkgconfig/%{libname}-*.pc
%{_libdir}/cmake/%{libname}-%{libver}
%{_libdir}/cmake/mongoc-%{libver}
%license COPYING
%license THIRD_PARTY_NOTICES
%{_libdir}/libbson*.so.*
%doc src/libbson/examples
%doc src/libbson/NEWS
%{_includedir}/libbson-%{libver}
%{_libdir}/libbson*.so
%{_libdir}/cmake/libbson-%{libver}
%{_libdir}/cmake/bson-%{libver}
%{_libdir}/pkgconfig/libbson-*.pc


%changelog
* Thu Jan 11 2024 Kevin Albertson
  - Initial RPM for RHEL 7.
