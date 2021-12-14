#include <mongoc/mongoc.h>
#include "../../util/monitoring.h"

static void exhaust_cursor(mongoc_cursor_t *cursor) 
{
    const bson_t *result;
    int rescount=0;

    while (mongoc_cursor_next(cursor, &result)) {
        rescount++;
        printf("\rfound: %i, more: %s",rescount, mongoc_cursor_more(cursor)?"True":"False");
    }
    printf("\n");

    printf("Found: %d documents\n", rescount);
}

int main()
{
   mongoc_client_t *client;
   char *uristr;
   mongoc_collection_t *coll;
   bson_error_t error;
   char *strbuff;
   bson_t *doc;
   bson_t *filter;
   bson_t *opts;
   mongoc_bulk_operation_t *bulk;
   int buffsize = 16 * 1024;
   int i;
   mongoc_cursor_t *cursor;
   mongoc_apm_callbacks_t *cbs;

   mongoc_init();
   mongoc_log_trace_disable ();

   uristr = getenv ("MONGODB_URI");
   client = mongoc_client_new (uristr);
   coll = mongoc_client_get_collection (client, "db", "foo");
   mongoc_collection_drop (coll, NULL /* error */);

   /* Generate documents large enought to require 3 batches in the results. */
   strbuff = malloc(buffsize);
   memset(strbuff, 'A', buffsize);
   strbuff[buffsize - 1] = 0;

   doc = bson_new();
   BSON_APPEND_UTF8(doc, "key", strbuff);
   bulk = mongoc_collection_create_bulk_operation_with_opts (coll, NULL);

   MONGOC_DEBUG ("Insert 2000 documents with %d-byte string\n", buffsize);
   for (i = 0; i < 2000; i++) {
      mongoc_bulk_operation_insert (bulk, doc);
   }

   if (!mongoc_bulk_operation_execute (bulk, NULL, &error)) {
      MONGOC_ERROR ("bulk insert failed: %s", error.message);
      return EXIT_FAILURE;
   }

   filter = bson_new();
   opts = bson_new();
   // BSON_APPEND_BOOL(opts,"exhaust",true);

   //test with find_with_opts
   printf("--------- mongoc_collection_find_with_opts -----------\n");
   cursor = mongoc_collection_find_with_opts(coll, filter, opts, NULL);
   exhaust_cursor(cursor);
   mongoc_cursor_destroy(cursor);

   bson_destroy (opts);
   mongoc_bulk_operation_destroy (bulk);
   bson_destroy (doc);
   mongoc_collection_destroy (coll);
   mongoc_apm_callbacks_destroy (cbs);
   mongoc_client_destroy (client);
   bson_free (uristr);

   mongoc_cleanup();
}

/*
4.2.12
"maxBsonObjectSize" : 16777216,
"maxMessageSizeBytes" : 48000000,
"maxWriteBatchSize" : 100000,

4.0.22
"maxBsonObjectSize" : 16777216,
"maxMessageSizeBytes" : 48000000,
"maxWriteBatchSize" : 100000,
*/