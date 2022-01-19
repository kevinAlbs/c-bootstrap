`main.c` inserts one document into `db.coll`.

`run.sh` executes the C program 5000 times.

`after_test.py` checks the `db.coll` for ObjectIDs with duplicate `rand` sequences.

To run:

```
./configure.sh
./build.sh
./run.sh
```