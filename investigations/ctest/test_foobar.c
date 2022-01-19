#include <assert.h>

extern int foo(void);
extern int bar(void);

void test_foo (void) {
    assert (1 == foo());
}

void test_bar (void) {
    assert (2 == bar());
}
