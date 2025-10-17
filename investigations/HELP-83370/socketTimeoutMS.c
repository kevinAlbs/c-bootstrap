// Example of using socketTimeoutMS and killCursors to kill a cursor after a timeout.

#include <mongoc/mongoc.h>

#define ABORT(...)                                       \
   do {                                                  \
      printf ("Error occurred on line: %d\n", __LINE__); \
      printf (__VA_ARGS__);                              \
      printf ("\n");                                     \
      abort ();                                          \
   } while (0)
#define ASSERT(stmt) BSON_ASSERT (stmt);

#define ITERATE_ONCE(cursor)                                       \
   do {                                                           \
      const bson_t *doc;                                          \
      bool has_next = mongoc_cursor_next (cursor, &doc);          \
      if (!has_next) {                                            \
         bson_error_t err;                                        \
         if (mongoc_cursor_error (cursor, &err)) {                \
            printf ("Cursor error: %s\n", err.message);           \
         } else {                                                 \
            printf ("No document returned\n");                    \
         }                                                        \
      } else {                                                    \
         char *str = bson_as_canonical_extended_json (doc, NULL); \
         printf ("Got document: %s\n", str);                  \
         bson_free (str);                                         \
      }                                                           \
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
               "output" : {
                  "$cond" : {
                     "if" : {"$gt" : [ "$x", 1 ]},
                     "then" : {
                        "$function" :
                           {"body" : "function() { sleep(1000); return \"foo\"; }", "args" : [], "lang" : "js"}
                     },
                     "else" : "fast"
                  }
               }
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

   mongoc_client_t *client = mongoc_client_new ("mongodb://localhost:27777/?socketTimeoutMS=1000");

   // Insert three documents into a collection:
   mongoc_collection_t *coll = mongoc_client_get_collection (client, "testdb", "testcoll");
   {
      mongoc_collection_drop (coll, NULL);
      bson_t *one = BCON_NEW ("x", BCON_INT32(1));
      bson_t *two = BCON_NEW ("x", BCON_INT32(2));
      bson_t *three = BCON_NEW ("x", BCON_INT32(3));
      mongoc_collection_insert_one (coll, one, NULL, NULL, NULL);
      mongoc_collection_insert_one (coll, two, NULL, NULL, NULL);
      mongoc_collection_insert_one (coll, three, NULL, NULL, NULL);
      bson_destroy (three);
      bson_destroy (two);
      bson_destroy (one);
   }

   bson_t *pipeline = slow_pipeline (); // Forces 1 second processing per document after first.
   bson_t *opts = BCON_NEW (
        "batchSize", BCON_INT32 (1)     // To trigger getMore calls.
   );

   mongoc_cursor_t *cursor = mongoc_collection_aggregate (coll, MONGOC_QUERY_NONE, pipeline, opts, NULL);
   ITERATE_ONCE (cursor); // 1st fast ... OK.
   int64_t cursor_id = mongoc_cursor_get_id (cursor); // Save established cursor ID.
   ITERATE_ONCE (cursor); // 2nd slow ... "socket error or timeout".
   // Send killCursors to clean-up server cursor:
   bson_t *cmd = BCON_NEW (
      "killCursors", BCON_UTF8 ("testdb.testcoll"),
      "cursors", "[", BCON_INT64 (cursor_id), "]"
   );
   ASSERT (mongoc_client_command_simple (client, "admin", cmd, NULL, NULL, NULL));
   printf ("Sent killCursors for cursor ID %" PRId64 "\n", cursor_id);

   bson_destroy (cmd);
   bson_destroy (opts);
   mongoc_cursor_destroy (cursor);
   mongoc_collection_destroy (coll);
   mongoc_client_destroy (client);
   mongoc_cleanup ();
}
