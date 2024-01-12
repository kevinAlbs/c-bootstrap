mongo-c-driver-rhel7.spec is an unsupported spec file to build an RPM on RHEL 7.

# Building the RPM

Download cmake separately and add to the PATH (RHEL 7 packaged cmake is too old).
Then build with:
```bash
#!/usr/bin/env bash

# Create ~/rpmbuild with `rpmdev-setuptree`. Refer: https://www.redhat.com/sysadmin/create-rpm-package
rpmdev-setuptree
cp mongo-c-driver-rhel7.spec ~/rpmbuild/SPECS/
wget https://github.com/mongodb/mongo-c-driver/archive/refs/tags/1.25.4.tar.gz -O ~/rpmbuild/SOURCES/1.25.4.tar.gz
rpmbuild -bb ~/rpmbuild/SPECS/mongo-c-driver-rhel7.spec
```

# Installing the RPM

I tested installing the built RPM on a RHEL7 EC2 instance with `ami-0bd83a2aad0515b35`.


```bash
#!/usr/bin/env bash

# Enable EPEL 7 to get libzstd-devel dependency. Refer: https://docs.fedoraproject.org/en-US/epel/#_rhel_7:
sudo yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

# Install libzstd-devel dependency
sudo yum install -y libzstd-devel

# Install the C driver RPM
sudo rpm -ivh mongo-c-driver-1.25.4-1.el7.x86_64.rpm

# Install gcc to compile a test
sudo yum install -y gcc

# Create a sample test
cat <<EOF > test-mongoc.c
#include <stdio.h>
#include <mongoc/mongoc.h>
#include <bson/bson.h>

int main () {
        printf ("Using libmongoc: %s and libbson: %s\n", mongoc_get_version(), bson_get_version());
}
EOF

# Compile the test with shared C driver libraries.
gcc test-mongoc.c -o test-mongoc $(pkg-config --libs --cflags libmongoc-1.0)
./test-mongoc
# Prints: Using libmongoc: 1.25.4 and libbson: 1.25.4
```
