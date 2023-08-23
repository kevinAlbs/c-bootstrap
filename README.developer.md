Develop with `-Werror`. Do not require consumers to build with `-Werror`.

Rationale: Be strict on development, lenient on consumption. This may improve portability on untested compilers or environments.

---

Use `#include <...>` instead of `#include "..."`, even within internal files.

Rationale: will use consistent search paths. The consistent use of `<>` also ensures include directories are being specified correctly via the build system, rather than bypassing valid include directory specifications through relative path lookup.

---

Use `BSON_ASSERT_PARAM` and `BSON_ASSERT` on pointer parameters.
Example:
```c
int
mcommon_thread_create (bson_thread_t *thread,
                    BSON_THREAD_FUN_TYPE (func),
                    void *arg,
                    int *errno_out)
{
BSON_ASSERT_PARAM (thread);
BSON_ASSERT_PARAM (func);
BSON_ASSERT (arg || true);       // optional.
BSON_ASSERT (errno_out || true); // optional.
// ...
}
```

Rationale: Documents and verifies expected handling of `NULL`.

---

Prefer configuring the `failCommand` failpoint with [`appName`](https://github.com/mongodb/mongo/wiki/The-%22failCommand%22-fail-point#failcommand-parameters).

Rationale: This isolates the failpoint from being triggered by operations on other clients.
