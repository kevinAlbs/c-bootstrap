Prefer configuring the `failCommand` failpoint with [`appName`](https://github.com/mongodb/mongo/wiki/The-%22failCommand%22-fail-point#failcommand-parameters).
- Rationale: This isolates the failpoint from being triggered by operations on other clients.

Use `#include <...>` instead of `#include "..."`, even within internal files.
- Rationale: will use consistent search paths. The consistent use of `<>` also ensures include directories are being specified correctly via the build system, rather than bypassing valid include directory specifications through relative path lookup.
