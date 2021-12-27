.menv creation command:

```
mlaunch init --replicaset --nodes=1 --sharded=1 --mongos=2 --priority --binarypath $(m bin 4.0.24-ent) --hostname localhost --enableMajorityReadConcern --setParameter enableTestCommands=1 --dir .menv --auth
```