#include <mongoc/mongoc.h>
#include "hex.h"

int main () {
    bson_t *embedded_null_key;
    bson_t *embedded_null_json;
    bson_error_t error;
    char *str;
    mongoc_client_t *client;
    mongoc_collection_t *coll;
    bson_t *opts;
    bool ok;
    bson_t reply;

    mongoc_init ();

    embedded_null_key = bson_new ();
    bson_append_int32 (embedded_null_key, "a\0b", 3, 123);
    str = bin_to_hex (bson_get_data(embedded_null_key), embedded_null_key->len);
    printf ("embedded_null_key =%s\n", str);
    bson_free (str);
    bson_destroy (embedded_null_key);

    embedded_null_json = bson_new_from_json ("{\"a\\u0000b\": 123}", -1 /* length */, &error);
    if (!embedded_null_json) {
        printf ("error parsing JSON: %s", error.message);
        return EXIT_FAILURE;
    }
    str = bin_to_hex (bson_get_data(embedded_null_json), embedded_null_json->len);
    printf ("embedded_null_json=%s\n", str);
    bson_free (str);

    str = bson_as_json (embedded_null_json, NULL);
    printf ("embedded_null_json as JSON: %s\n", str);
    bson_free (str);

    client = mongoc_client_new ("mongodb://localhost:27017");
    coll = mongoc_client_get_collection (client, "db", "coll");
    opts = BCON_NEW ("validate", BCON_BOOL(false));
    ok = mongoc_collection_insert_one (coll, embedded_null_json, opts, &reply, &error);
    if (!ok) {
        printf ("error in mongoc_collection_insert_one: %s\n", error.message);
        str = bson_as_json (&reply, NULL);
        printf ("full server reply: %s\n", str);
        bson_free (str);
    }
    bson_destroy (&reply);
    bson_destroy (opts);
    mongoc_collection_destroy (coll);
    mongoc_client_destroy (client);

    bson_destroy (embedded_null_json);

    mongoc_cleanup ();
}

/* Sample output against libmongoc 1.19.0 and a 5.1.0-alpha-414-g69ca8b9 MongoDB server:
 * $ ./run.sh
 * embedded_null_key =0e00000010610062007b00000000
 * embedded_null_json=0e00000010610062007b00000000
 * embedded_null_json as JSON: (null)
 * error in mongoc_collection_insert_one: incorrect BSON length in element with field name 'a' in object with _id: ObjectId('6138b43c2b64937a047f02a2')
 * full server reply: { "insertedCount" : 0 }
 * 
 * Parsing with bsonview shows the embedded NULL in the key:
 * echo "0e00000010610062007b00000000" | bsonview -x
 * 0e000000 10 "a" 00 62007B00 00 0000
 */ 