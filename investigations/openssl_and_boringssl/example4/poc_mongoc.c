#include <stdio.h>

extern void poc_ssl_fn ();

void poc_mongoc_fn () {
   printf ("poc_mongoc_fn... begin\n");
   poc_ssl_fn ();
   printf ("poc_mongoc_fn... end\n");
}
