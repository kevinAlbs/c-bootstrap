// Q: What happens when using two libraries depending on different versions of a library?
// A:

#include <stdio.h>

extern void poc_tensorflow_fn ();
extern void poc_mongoc_fn ();

int main () {
    poc_tensorflow_fn ();
    poc_mongoc_fn ();
}
