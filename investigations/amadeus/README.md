A test environment with two mongos hosts (on port 27017 and 27018) was created with `mlaunch` as follows:

```bash
mlaunch init \
    --sharded=1 \
    --mongos=2 \
    --replicaset \
    --nodes=1 \
    --binarypath $HOME/bin/mongodl/archive/7.0.0/mongodb-macos-aarch64-enterprise-7.0.0/bin \
    --dir .menv
```
