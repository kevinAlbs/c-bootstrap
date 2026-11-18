~/bin/mongodl/archive/7.0.0/mongodb-macos-aarch64-enterprise-7.0.0/bin/mongod \
    --setParameter enableTestCommands=1 \
    --wiredTigerCacheSizeGB=0.25 --oplogSize=100 \
    --replSet=rs0 \
    --port 27019 \
    --dbpath .menv-s2