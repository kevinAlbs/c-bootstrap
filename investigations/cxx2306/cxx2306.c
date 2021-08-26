/*
Compile with:
clang cxx2306.c -o cxx2306 $(pkg-config --cflags --libs libmongoc-1.0) -fsanitize=address

ASAN_OPTIONS='detect_leaks' ./cxx2306
*/

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
   mongoc_client_pool_t* pool;
   mongoc_uri_t *uri;
   mongoc_client_t *client;
   mongoc_collection_t *coll;
   bson_t *filter;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   char *doc_str;

   mongoc_init ();

   uri = mongoc_uri_new ("mongodb://localhost:27017");
   pool = mongoc_client_pool_new (uri);
   client = mongoc_client_pool_pop (pool);
   coll = mongoc_client_get_collection (client, "db", "coll");
   filter = bson_new ();
   cursor = mongoc_collection_find_with_opts (coll, filter, NULL, NULL);
   while (mongoc_cursor_next (cursor, &doc)) {
      doc_str = bson_as_json (doc, NULL);
      MONGOC_DEBUG ("Got result: %s", doc_str);
      bson_free (doc_str);
   }

   bson_destroy (filter);
   mongoc_cursor_destroy (cursor);
   mongoc_collection_destroy (coll);
   mongoc_client_pool_push (pool, client);
   mongoc_client_pool_destroy (pool);
   mongoc_uri_destroy (uri);

   mongoc_cleanup ();

   return EXIT_SUCCESS;
}
