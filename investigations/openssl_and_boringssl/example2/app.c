// Q: What happens when using two libraries depending on different versions of a library?
// A: On Ubuntu 18.04 both use the poc_ssl_fn defined in boringssl.


#include <stdio.h>

extern void poc_tensorflow_fn ();
extern void poc_mongoc_fn ();

int main () {
    poc_tensorflow_fn ();
    poc_mongoc_fn ();
}
