// Q: Does `maxTimeMS` on `aggregate` apply to the total of all operations, or each individual operation? A: Total.

#include <mongoc/mongoc.h>

#define ABORT(...)                                       \
   do {                                                  \
      printf ("Error occurred on line: %d\n", __LINE__); \
      printf (__VA_ARGS__);                              \
      printf ("\n");                                     \
      abort ();                                          \
   } while (0)
#define ASSERT(stmt) BSON_ASSERT (stmt);

#define ITERATE_DOC(cursor)                                    \
   do {                                                        \
      const bson_t *doc;                                       \
      bool has_next = mongoc_cursor_next (cursor, &doc);       \
      if (!has_next) {                                         \
         bson_error_t err;                                     \
         if (mongoc_cursor_error (cursor, &err)) {             \
            ABORT ("Cursor error: %s", err.message);           \
         }                                                     \
         ABORT ("Expected more documents from cursor");        \
      }                                                        \
      char *str = bson_as_canonical_extended_json (doc, NULL); \
      MONGOC_DEBUG ("Got document: %s", str);                  \
      bson_free (str);                                         \
   } while (0)

#define FROM_JSON(...) bson_new_from_json ((const uint8_t *) BSON_STR(__VA_ARGS__, -1, NULL)

static bson_t *
slow_pipeline (void)
{
   const char *pipeline_str = BSON_STR ({
      "pipeline" : [
         {"$match" : {}},
         {
            "$project" : {
               "output" :
                  {"$function" : {"body" : "function() { sleep(1000); return \"foo\"; }", "args" : [], "lang" : "js"}}
            }
         }
      ]
   });

   return bson_new_from_json ((const uint8_t *) pipeline_str, -1, NULL);
}

int
main ()
{
   bson_error_t error;

   mongoc_init ();

   mongoc_client_t *client = mongoc_client_new ("mongodb://localhost:27777");
   MONGOC_DEBUG ("mongoc_get_version=%s", mongoc_get_version ());

   // Insert three documents into a collection:
   mongoc_collection_t *coll = mongoc_client_get_collection (client, "testdb", "testcoll");
   {
      mongoc_collection_drop (coll, NULL);
      bson_t *doc = BCON_NEW ("x", "y");
      mongoc_collection_insert_one (coll, doc, NULL, NULL, NULL);
      mongoc_collection_insert_one (coll, doc, NULL, NULL, NULL);
      mongoc_collection_insert_one (coll, doc, NULL, NULL, NULL);
      bson_destroy (doc);
   }

   bson_t *pipeline = slow_pipeline (); // Forces 1 second processing per document.
   bson_t *opts = BCON_NEW (
        "maxTimeMS", BCON_INT32 (1500), // Set max aggregate time of 1.5s
        "batchSize", BCON_INT32 (1)     // To trigger getMore calls.
   );

   mongoc_cursor_t *cursor = mongoc_collection_aggregate (coll, MONGOC_QUERY_NONE, pipeline, opts, NULL);
   ITERATE_DOC (cursor); // Sends 'aggregate' ... OK
   ITERATE_DOC (cursor); // Sends 'getMore' ... OK (but hits timeout)
   ITERATE_DOC (cursor); // Cursor error: operation exceeded time limit

   bson_destroy (opts);
   mongoc_cursor_destroy (cursor);
   mongoc_collection_destroy (coll);
   mongoc_client_destroy (client);
   mongoc_cleanup ();
}
