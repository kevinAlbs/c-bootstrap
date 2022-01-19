#include <stdio.h>
#include <string.h>

extern void test_foo (void);
extern void test_bar (void);

int main (int argc, char** argv) {
   if (argc < 2) {
      printf ("error: test name not passed\n");
      return 1;
   }

   printf ("running test: %s\n", argv[1]);
   if (0 == strcmp ("foo", argv[1])) {
      test_foo ();
   }
   if (0 == strcmp ("bar", argv[1])) {
      test_bar ();
   }
}