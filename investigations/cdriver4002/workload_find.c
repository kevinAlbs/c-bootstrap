/* Run as follows:
 * workload_find <thread count (max 100)>
 * Use environment variables to test flags:
 * FLAG_EXPLICIT_SESSION=ON tests passing a single explicit session to each thread.
 * FLAG_SINGLE_THREADED=ON tests using a separate single-threaded mongoc_client_t per thread.
 */

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define URI "mongodb://localhost:27017"
#define DB "test"
#define COLL "coll"
#define ENV_FLAG_EXPLICIT_SESSION "FLAG_EXPLICIT_SESSION"
#define ENV_FLAG_SINGLE_THREADED "FLAG_SINGLE_THREADED"
typedef struct {
   int tid;
   mongoc_client_pool_t *pool;
} thread_args_t;

bool flag_isset (char* name) {
   char* flag = getenv(name);
   if (flag && 0 == strcmp(flag, "ON")) {
      return true;
   }
   return false;
}
/* thread_find repeatedly runs find with a filter {_id: 0} */
void *thread_find (void *arg) {
   mongoc_client_pool_t *pool;
   bson_t filter;
   int64_t ops = 0;
   int64_t running_ops = 0;
   bson_error_t error;
   thread_args_t *args = (thread_args_t*) arg;
   int64_t start_time;
   mongoc_client_session_t *session = NULL;
   bson_t opts;

   pool = args->pool;

   bson_init (&opts);
   bson_init (&filter);
   BSON_APPEND_INT32 (&filter, "_id", 0);
   start_time = bson_get_monotonic_time();

   mongoc_client_t* client;

   if (flag_isset (ENV_FLAG_SINGLE_THREADED)) {
      client = mongoc_client_new (URI);
      if (!client) {
         MONGOC_ERROR ("[tid=%d] error creating client", args->tid);
         return NULL;
      }
   } else {
      client = mongoc_client_pool_pop (pool);
   }

   if (flag_isset (ENV_FLAG_EXPLICIT_SESSION)) {
      session = mongoc_client_start_session (client, NULL /* opts */, &error);
      if (!session) {
         MONGOC_ERROR ("[tid=%d] error starting session: %s", args->tid, error.message);
         return NULL;
      }
      if (!mongoc_client_session_append (session, &opts, &error)) {
         MONGOC_ERROR ("[tid=%d] error appending session to opts: %s", args->tid, error.message);
         return NULL;
      }
   }
   
   while (true) {
      mongoc_collection_t *coll;
      mongoc_cursor_t *cursor;
      const bson_t *doc;

      coll = mongoc_client_get_collection (client, DB, COLL);
      cursor = mongoc_collection_find_with_opts (coll, &filter, &opts, NULL /* read_prefs */);
      if (mongoc_cursor_error (cursor, &error)) {
         MONGOC_ERROR ("[tid=%d] find returned error: %s", args->tid, error.message);
         return NULL;
      }
      /* iterate once */
      if (mongoc_cursor_next(cursor, &doc)) {
         MONGOC_ERROR ("unexpected document returned. %s.%s must be an empty collection", DB, COLL);
         return NULL;
      }
      if (mongoc_cursor_error (cursor, &error)) {
         MONGOC_ERROR ("[tid=%d] next returned error: %s", args->tid, error.message);
         return NULL;
      }
      ops += 1;
      if (ops >= 3000) {
         double ops_s;
         int64_t current_time;

         running_ops += ops;
         current_time = bson_get_monotonic_time ();
         ops_s = (double) running_ops / (((double)(current_time - start_time)) / (1000 * 1000));
         ops = 0;
         MONGOC_INFO ("[tid=%d] ran %" PRId64 " ops, ops/s=%f", args->tid, running_ops, ops_s);
      }

      mongoc_cursor_destroy (cursor);
      mongoc_collection_destroy (coll);
   }

   bson_destroy (&filter);
   bson_destroy (&opts);
   mongoc_client_session_destroy (session);

   if (flag_isset(ENV_FLAG_SINGLE_THREADED)) {
      mongoc_client_destroy (client);
   } else {
      mongoc_client_pool_push (pool, client);
   }
   return NULL;
}

int
main (int argc, char *argv[])
{
   mongoc_client_pool_t *pool;
   mongoc_uri_t *uri;
   pthread_t* threads;
   thread_args_t* thread_args;
   int n = 10;
   int i;

   mongoc_init ();

   uri = mongoc_uri_new (URI);
   if (!uri) {
      MONGOC_ERROR ("invalid URI %s", URI);
      return 1;
   }
   pool = mongoc_client_pool_new (uri);

   if (argc == 2) {
      n = atoi (argv[1]);
      if (n <= 0) {
         MONGOC_ERROR ("invalid thread count: %d", n);
         return 1;
      }
   }

   threads = bson_malloc0 (sizeof(pthread_t) * n);
   thread_args = bson_malloc0 (sizeof(thread_args_t) * n);

   for (i = 0; i < n; i++) {
      thread_args[i].pool = pool;
      thread_args[i].tid = i;
      pthread_create(threads + i, NULL /* pthread_attr_t */, thread_find, thread_args + i);
   }

   MONGOC_INFO ("running with %d threads", n);
   MONGOC_INFO ("testing with explicit session? %d", (int) flag_isset(ENV_FLAG_EXPLICIT_SESSION));
   MONGOC_INFO ("testing with single threaded? %d", (int) flag_isset(ENV_FLAG_SINGLE_THREADED));

   for (i = 0; i < n; i++) {
      pthread_join (threads[i], NULL);
   }
   
   mongoc_cleanup ();
}