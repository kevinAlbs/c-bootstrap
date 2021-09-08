#include <mongoc/mongoc.h>

#include <pthread.h>

int32_t val;

#define N_INCREMENTS 10000
#define N_THREADS 20

void* no_atomic_incrementor (void* unused_arg) {
   int i;

   for (i = 0; i < N_INCREMENTS; i++) {
      val += 1;
   }
   return NULL;
}

void* atomic_incrementor (void* unused_arg) {
   int i;

   for (i = 0; i < N_INCREMENTS; i++) {
      bson_atomic_int_add (&val, 1);
   }
   return NULL;
}

int main () {
   pthread_t threads[N_THREADS];
   int i;

   mongoc_init ();

   for (i = 0; i < N_THREADS; i++) {
      pthread_create (threads + i, NULL /* attr */, atomic_incrementor, NULL /* arg */);
   }
   
   for (i = 0; i < N_THREADS; i++) {
      pthread_join (threads[i], NULL /* *ptr */);
   }
   
   MONGOC_DEBUG ("got  = %" PRId32, val);
   MONGOC_DEBUG ("want = %" PRId32, N_THREADS * N_INCREMENTS);
   int* p = malloc(123);
   mongoc_cleanup ();
}
