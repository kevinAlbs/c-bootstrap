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

The `serverMonitoringMode` option is planned in [CDRIVER-5577](https://jira.mongodb.org/browse/CDRIVER-5577)
