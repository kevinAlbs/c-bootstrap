Install XCode to get Instruments.

Install the C driver:

```bash
./install-mongo-c-driver.sh
```

Configure and build the test runner:
```bash
./configure.sh
./build.sh
```

Use instruments to measure. Use `Time Profiler` to sample stack traces. `CPU Profiler` does not appear to include off-CPU network calls. Here is a sample result:

