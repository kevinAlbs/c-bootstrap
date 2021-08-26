/* Conversation in:
 * Compile with:
 * clang -o long_runner.out long_runner.c $(pkg-config --cflags --libs libmongoc-1.0) -pthread
 */

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define URI "mongodb://localhost:27017"
#define DB "test"
#define COLL "coll"

static int global_tid;

/* thread_count repeatedly runs "countDocuments" */
void *thread_count (void *arg) {
   mongoc_client_pool_t *pool;
   bson_t filter;
   int64_t ret;
   int64_t ops = 0;
   int64_t running_ops = 0;
   bson_error_t error;
   int tid = global_tid++; // TODO use atomic.

   bson_init (&filter);
   pool = (mongoc_client_pool_t*) arg;
   while (true) {
      mongoc_client_t* client;
      mongoc_collection_t *coll;

      client = mongoc_client_pool_pop (pool);
      coll = mongoc_client_get_collection (client, DB, COLL);
      ret = mongoc_collection_count_documents (coll, &filter, NULL /* opts */, NULL /* read prefs */, NULL /* reply */, &error);
      if (ret == -1) {
         MONGOC_ERROR ("got error: %s", error.message);
         return NULL;
      }
      ops += 1;
      if (ops >= 10000) {
         running_ops += ops;
         ops = 0;
         MONGOC_INFO ("tid %d ran %" PRId64 " ops", tid, running_ops);
      }

      mongoc_collection_destroy (coll);
      mongoc_client_pool_push (pool, client);
   }
   return NULL;
}

int
main (int argc, char *argv[])
{
   mongoc_client_pool_t *pool;
   mongoc_uri_t *uri;
   pthread_t threads[2];

   mongoc_init ();

   uri = mongoc_uri_new (URI);
   pool = mongoc_client_pool_new (uri);

   pthread_create(threads + 0, NULL /* attr */, thread_count, pool);
   pthread_create(threads + 1, NULL /* attr */, thread_count, pool);

   pthread_join (threads[0], NULL);
   pthread_join (threads[1], NULL);
   mongoc_cleanup ();
}
