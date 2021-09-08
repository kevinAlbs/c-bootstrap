#include <mongoc/mongoc.h>

int main () {
   bson_t *doc;
   bool ok;
   mongoc_client_t *client;
   mongoc_collection_t *coll;
   bson_t *opts;
   bson_error_t error;

   mongoc_init ();

   doc = bson_new ();
   ok = bson_append_utf8 (doc, "foo\0bar", 7, "foo bar", 7);
   BSON_ASSERT (ok); // TODO: this should NOT be ok.

   client = mongoc_client_new ("mongodb://localhost:27017");
   coll = mongoc_client_get_collection (client, "test", "test");

#ifdef BYPASS_VALIDATION
   opts = BCON_NEW ("validate", BCON_BOOL (false));
#else
   opts = bson_new ();
#endif

   ok = mongoc_collection_insert_one (coll, doc, opts, NULL /* reply */, &error);
   if (!ok) {
      MONGOC_ERROR ("insert_one error: %s", error.message);
   }

   bson_destroy (opts);
   bson_destroy (doc);
   mongoc_collection_destroy (coll);
   mongoc_client_destroy (client);
   mongoc_cleanup ();
}
