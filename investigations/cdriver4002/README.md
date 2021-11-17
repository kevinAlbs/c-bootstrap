## Running instructions

1. Install C driver 1.19.2 and master. From the root of this repository:
```
# Install libbson and libmongocrypt dependencies.
GITREF=master ./etc/install_libbson.sh
GITREF=master ./etc/install_libmongocrypt.sh
# Install C driver 1.19.2 and master.
GITREF=1.19.2 ./etc/install_mongo_c_driver.sh
GITREF=master ./etc/install_mongo_c_driver.sh
```

2. Compile, from this directory:
```
./compile.sh
```

3. Run the `run-1.19.2.sh` and `run-master.sh` scripts to compare.