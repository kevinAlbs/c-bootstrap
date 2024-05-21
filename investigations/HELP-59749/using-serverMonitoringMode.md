# Installing C driver with `serverMonitoringMode` URI option

Install a modified C driver 1.25 with a `serverMonitoringMode` URI option:

```bash
git clone --depth=1 https://github.com/kevinAlbs/mongo-c-driver.git --branch serverMonitoringMode.r1.25
cd mongo-c-driver
cmake -S. -Bcmake-build \
    -D BUILD_VERSION=1.25.5-pre \
    -D CMAKE_INSTALL_PREFIX=$HOME/mongo-c-driver-1.25.5-pre
cmake --build cmake-build --target install
```

A client can use this option to force polling monitoring:

```c
mongoc_uri_t *uri = mongoc_uri_new("mongodb://localhost:27017,localhost:27018,localhost:27019/?maxPoolSize=64&serverMonitoringMode=poll");
mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);
```

The `serverMonitoringMode` option is planned in [CDRIVER-5577](https://jira.mongodb.org/browse/CDRIVER-5577)
