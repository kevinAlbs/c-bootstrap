#include <stdio.h>

extern void poc_ssl_fn ();

void poc_tensorflow_fn () {
   printf ("poc_tensorflow_fn... begin\n");
   poc_ssl_fn ();
   printf ("poc_tensorflow_fn... end\n");
}
