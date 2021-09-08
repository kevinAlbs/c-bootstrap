/* Conversation in:
 * https://github.com/mongodb/mongo-cxx-driver/pull/784/files#diff-137762f0429f0f527a7ce4920ed7f3e792e84297e4fb071bf116815418afbaf7R586
 * Q: Why does C++ findOneAndDelete sometimes return an empty document?
 * Theory: because unacknowleged writes return no server response.
 *
 * Compile with:
 * clang -o fam_w0 fam_w0.c $(pkg-config --cflags --libs libmongoc-1.0)
 */

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
   mongoc_init ();

   mongoc_write_concern_t *wc; 
   mongoc_client_t *client;
   mongoc_find_and_modify_opts_t *fam_opts;
   mongoc_collection_t *coll;
   bson_t reply;
   bson_t *query;
   bson_t extra;

   client = mongoc_client_new ("mongodb://localhost:27017");
   fam_opts = mongoc_find_and_modify_opts_new ();
   mongoc_find_and_modify_opts_set_flags (fam_opts, MONGOC_FIND_AND_MODIFY_REMOVE);
   /* Append an unacknowledged write concern.
    * This is only supported in findAndModify on 3.2+ servers. */
   wc = mongoc_write_concern_new ();
   mongoc_write_concern_set_w (wc, MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED);
   bson_init (&extra);
   mongoc_write_concern_append (wc, &extra);
   /* Append the write concern as BSON, since the fam opt helpers do not have
    * an explicit write concern setter. */
   mongoc_find_and_modify_opts_append (fam_opts, &extra);

   coll = mongoc_client_get_collection (client, "db", "test");
   query = BCON_NEW ("x", BCON_INT32(1));
   mongoc_collection_find_and_modify_with_opts (coll, query, fam_opts, &reply, NULL);
   printf ("got back: %s", bson_as_json (&reply, NULL));

   /* TODO: free things lol */

   mongoc_cleanup ();
}
