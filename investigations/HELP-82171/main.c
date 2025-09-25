#include <mongoc/mongoc.h>

static void
command_started (const mongoc_apm_command_started_t *event)
{
   printf ("%s started\n", mongoc_apm_command_started_get_command_name (event));
}


static void
command_succeeded (const mongoc_apm_command_succeeded_t *event)
{
   printf ("%s succeeded\n", mongoc_apm_command_succeeded_get_command_name (event));
   double duration_ms = (double)mongoc_apm_command_succeeded_get_duration (event) / 1000.0;
   printf ("  with duration: %.02gms\n", duration_ms);
}


static void
command_failed (const mongoc_apm_command_failed_t *event)
{
   printf ("%s failed\n", mongoc_apm_command_failed_get_command_name (event));
   double duration_ms = (double)mongoc_apm_command_failed_get_duration (event) / 1000.0;
   printf ("  with duration: %.02gms\n", duration_ms);
   bson_error_t error;
   mongoc_apm_command_failed_get_error (event, &error);
   printf ("  with error: %s\n", error.message);
}

// set_command_monitors sets command monitors to print latency.
static void
set_command_monitors (mongoc_client_t *client)
{
   mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new ();
   mongoc_apm_set_command_started_cb (cbs, command_started);
   mongoc_apm_set_command_failed_cb (cbs, command_failed);
   mongoc_apm_set_command_succeeded_cb (cbs, command_succeeded);
   mongoc_client_set_apm_callbacks (client, cbs, NULL);
   mongoc_apm_callbacks_destroy (cbs);
}


int main () {
    mongoc_init ();

    printf ("C driver version: %s\n", mongoc_get_version ());
    mongoc_client_t *client = mongoc_client_new ("mongodb://localhost:27017");
    set_command_monitors (client);

    bson_t cmd = BSON_INITIALIZER;
    BCON_APPEND (&cmd, "ping", BCON_INT32 (1));

    mongoc_client_command_simple (client, "admin", &cmd, NULL, NULL, NULL);
    
    bson_destroy (&cmd);
    mongoc_client_destroy (client);
    mongoc_cleanup ();
}
