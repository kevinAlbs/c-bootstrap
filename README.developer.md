Prefer configuring the `failCommand` failpoint with [`appName`](https://github.com/mongodb/mongo/wiki/The-%22failCommand%22-fail-point#failcommand-parameters). This isolates the failpoint from being triggered by operations on other clients.