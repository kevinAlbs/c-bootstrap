#include "mongoc/mongoc.h"


#define USE_POOL 1
//#define USE_THREADS 1
#define MAX_THREADS 2

int iterations=1e6;

DWORD WINAPI TestUpdates(LPVOID param)
{
   bson_t *selector = BCON_NEW ("_id", "{", "$gt", BCON_INT32 (0), "}");
   bson_t *update = BCON_NEW ("$set", "{", "x", BCON_INT32 (1), "y", BCON_INT32 (2), "}");
   time_t now;
   int duration;
   int i;
   mongoc_collection_t *coll=(mongoc_collection_t*)param;
   bson_error_t error = {0};
   bson_t *to_insert = BCON_NEW ("_id", BCON_INT32 (1));
   char *to_str;

   now=time(NULL);// test update speed *************************************************
   for(i=0; i<iterations; i++)
   {
	   if (!mongoc_collection_update_one (
			  coll, selector, update, NULL, NULL, &error)) {
		  fprintf (stderr, "update failed: %s\n", error.message);
		  return EXIT_FAILURE;
	   }
   }
   duration=time(NULL)-now;
   printf("update/s %f over %d s\n", (float)iterations/duration, duration);
   to_str = bson_as_relaxed_extended_json (to_insert, NULL);
   printf ("inserted: %s\n", to_str);
   bson_free (to_str);
   return 0;
}

DWORD WINAPI TestQueries(LPVOID param)
{
   bson_t *selector = BCON_NEW ("_id", "{", "$gt", BCON_INT32 (0), "}");
   time_t now;
   int duration;
   int i;
   mongoc_collection_t *coll=(mongoc_collection_t*)param;
   bson_error_t error = {0};
   mongoc_cursor_t *cursor;
   const bson_t *next_doc;
   char *to_str;
   const bson_t *doc = NULL;
	bson_iter_t iter;
	bson_type_t type;

   now=time(NULL);// test query speed ************************************************
   for(i=0; i<iterations; i++)
   {
	   cursor = mongoc_collection_find_with_opts (coll, selector, NULL, NULL);
	   //BSON_ASSERT (mongoc_cursor_next (cursor, &next_doc));
	   if(cursor)
	   {
			while(mongoc_cursor_next(cursor, &doc))// the query is only executed here ?
			{
				if (bson_iter_init (&iter, doc))
				{
					while (bson_iter_next(&iter)) 
					{
						//type = bson_iter_type (&iter);
						//if(type == BSON_TYPE_UTF8)
						{
							const char* key=bson_iter_key (&iter);
							//printf("queried key %s",key);
						}
					}
				}
			}
			mongoc_cursor_destroy(cursor);
	   }
   }
   duration=time(NULL)-now;
   printf("query/s %f over %d s\n", (float)iterations/duration, duration);
   printf ("document queried:\n");
/*
   to_str = bson_as_relaxed_extended_json (next_doc, NULL);
   printf ("%s\n", to_str);
   bson_free (to_str);*/
   return 0;
}

DWORD WINAPI TestQueries_socket_leak(LPVOID param)
{
   bson_t *selector = BCON_NEW ("_id", "{", "$gt", BCON_INT32 (0), "}");
   time_t now;
   int duration;
   int i;
   mongoc_collection_t *coll=(mongoc_collection_t*)param;
   bson_error_t error = {0};
   mongoc_cursor_t *cursor;
   const bson_t *next_doc;
   char *to_str;
   const bson_t *doc = NULL;
	bson_iter_t iter;
	bson_type_t type;
	bson_t *fields=bson_new();

   now=time(NULL);// test query speed ************************************************
   for(i=0; i<iterations; i++)
   {
	   //cursor = mongoc_collection_find_with_opts (coll, selector, NULL, NULL);
	   //BSON_APPEND_INT32(fields, fieldstring, 1);// only return this single field
		cursor=mongoc_collection_find (coll, // this leaks sockets in mongoc_socket_new ?
            MONGOC_QUERY_EXHAUST,//MONGOC_QUERY_NONE;
            0,// skip
            1,// limit
            0,// batch_size
            selector,
            fields,
            NULL);//const mongoc_read_prefs_t *read_prefs);*/

	   if(cursor)
	   {
			while(mongoc_cursor_next(cursor, &doc))// the query is only executed here ?
			{
				/*if (bson_iter_init (&iter, doc))
				{
					while (bson_iter_next(&iter)) 
					{
						//type = bson_iter_type (&iter);
						//if(type == BSON_TYPE_UTF8)
						{
							const char* key=bson_iter_key (&iter);
							printf("queried key %s",key);
						}
					}
				}*/
			}
			mongoc_cursor_destroy(cursor);
	   }
   }
   duration=time(NULL)-now;
   printf("query/s %f over %d s\n", (float)iterations/duration, duration);
   printf ("document queried:\n");
   bson_destroy (selector);
   bson_destroy (fields);
   return 0;
}

int
main (int argc, char **argv)
{
   bson_t *selector = BCON_NEW ("_id", "{", "$gt", BCON_INT32 (0), "}");
   bson_t *update = BCON_NEW ("$set", "{", "x", BCON_INT32 (1), "y", BCON_INT32 (2), "}");
   bson_error_t error = {0};
   mongoc_client_t *client[MAX_THREADS];// client and collection are thread local
   mongoc_collection_t *coll[MAX_THREADS];
   const char *uri_string = "mongodb://ECLASS-MGFRW01:27017/?appname=example-update";//10.157.131.249
   mongoc_uri_t *uri = mongoc_uri_new_with_error (uri_string, &error);
   bson_t *to_insert = BCON_NEW ("_id", BCON_INT32 (1));
   DWORD thid[MAX_THREADS];
   HANDLE handles[MAX_THREADS];
   int i;

#ifdef USE_POOL
      mongoc_client_pool_t *pool;
#endif

   mongoc_init ();

   if (!uri) {
      fprintf (stderr,
               "failed to parse URI: %s\n"
               "error message:       %s\n",
               uri_string,
               error.message);
      return EXIT_FAILURE;
   }

#ifdef USE_POOL
   pool = mongoc_client_pool_new (uri);
   mongoc_client_pool_set_error_api (pool, 2);
   for(i=0; i< MAX_THREADS; i++)
		client[i] = mongoc_client_pool_pop (pool);
#else
   client[0] = mongoc_client_new_from_uri (uri);
   mongoc_client_set_error_api (client[0], 2);
#endif
   if (!client[0]) {
      return EXIT_FAILURE;
   }

   // from mongosh : db.example_coll.insertOne( {x:1} )
   for(i=0; i< MAX_THREADS; i++)
		coll[i] = mongoc_client_get_collection (client[i], "db", "example_coll");


    BSON_ASSERT (mongoc_collection_drop (coll[0], NULL));

   // insert a document 
   if (!mongoc_collection_insert_one (coll[0], to_insert, NULL, NULL, &error)) {
      fprintf (stderr, "insert failed: %s\n", error.message);
      //return EXIT_FAILURE;
   }


   #ifdef USE_THREADS
   handles[0]=CreateThread(NULL,//default security
	   0,// default stack
	   TestQueries,// write
	   coll[0],
	   0,// default flags
	   &thid[0]);// thread id
   handles[1]=CreateThread(NULL,//default security
	   0,// default stack
	   TestQueries,// read
	   coll[1],
	   0,// default flags
	   &thid[1]);// thread id
   WaitForMultipleObjects(2,handles, TRUE,INFINITE);// wait for all threads to end
   #else
   //TestUpdates((void*)coll[0]);
   //TestQueries((void*)coll[0]);
   TestQueries_socket_leak((void*)coll[0]);
   #endif


   bson_destroy (to_insert);
   bson_destroy (update);
   bson_destroy (selector);
   for(i=0; i< MAX_THREADS; i++)
		mongoc_collection_destroy (coll[i]);
   mongoc_uri_destroy (uri);
#ifdef USE_POOL
   for(i=0; i< MAX_THREADS; i++)
		mongoc_client_pool_push (pool, client[i]);
   mongoc_client_pool_destroy (pool);
#else
   mongoc_client_destroy (client);
#endif

   return EXIT_SUCCESS;
}