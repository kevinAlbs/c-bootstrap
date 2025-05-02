Start mongod:
```
# Fix paths in start-mongod.sh and mongodb.conf
cd repro && ./start-mongod.sh
```

Connect with C driver:
```
# Fix paths in configure.sh and main.c.
./configure.sh
./build.sh
./run.sh
# Expect "Connected and pinged MongoDB successfully over TLS."
```
