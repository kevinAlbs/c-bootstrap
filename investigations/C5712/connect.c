// Build with   : clang -o connect connect.c $(pkg-config --libs --cflags libmongoc-1.0)
// Run with     : ./connect <MongoDB URI>
#include <mongoc/mongoc.h>
#include <stdio.h>

int
main (int argc, char **argv)
{
   if (argc != 2) {
      printf ("Expected URI. Run with %s <URI string>\n", argv[0]);
      return 1;
   }

   mongoc_init ();
   mongoc_client_t *client = mongoc_client_new (argv[1]);
   bson_t *ping = BCON_NEW ("ping", BCON_INT32 (1));
   bson_error_t error;
   bool ok = mongoc_client_command_simple (client, "admin", ping, NULL, NULL, &error);
   if (!ok) {
      printf ("Got error: %s\n", error.message);
   } else {
      printf ("Successfully ran 'ping' command\n");
   }

   bson_destroy (ping);
   mongoc_client_destroy (client);
   mongoc_cleanup ();
}
