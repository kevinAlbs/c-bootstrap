#include <mongoc/mongoc.h>
#include <stdio.h>

static BSON_INLINE void
command_started (const mongoc_apm_command_started_t *event)
{
   char *s;

   s = bson_as_relaxed_extended_json (
      mongoc_apm_command_started_get_command (event), NULL);
   MONGOC_DEBUG ("Command %s started on %s: %s",
           mongoc_apm_command_started_get_command_name (event),
           mongoc_apm_command_started_get_host (event)->host,
           s);

   bson_free (s);
}


static BSON_INLINE void
command_succeeded (const mongoc_apm_command_succeeded_t *event)
{
   char *s;

   s = bson_as_relaxed_extended_json (
      mongoc_apm_command_succeeded_get_reply (event), NULL);
   MONGOC_DEBUG ("Command %s succeeded: %s",
           mongoc_apm_command_succeeded_get_command_name (event),
           s);

   bson_free (s);
}


static BSON_INLINE void
command_failed (const mongoc_apm_command_failed_t *event)
{
   bson_error_t error;
   const bson_t *reply;

   mongoc_apm_command_failed_get_error (event, &error);
   MONGOC_DEBUG ("Command %s failed: %s",
           mongoc_apm_command_failed_get_command_name (event),
           error.message);
   reply = mongoc_apm_command_failed_get_reply (event);
   if (reply) {
      char *str = bson_as_relaxed_extended_json (reply, NULL);
      MONGOC_DEBUG ("failed reply: %s", str);
      bson_free (str);
   }
   
}

static BSON_INLINE void set_command_monitors (mongoc_apm_callbacks_t *cbs, bool started_only) {
    mongoc_apm_set_command_started_cb (cbs, command_started);
    if (!started_only) {
        mongoc_apm_set_command_failed_cb (cbs, command_failed);
        mongoc_apm_set_command_succeeded_cb (cbs, command_succeeded);
    }
}


static BSON_INLINE void
set_command_monitors_on_client (mongoc_client_t *client)
{
   mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new ();
   mongoc_apm_set_command_started_cb (cbs, command_started);
   mongoc_apm_set_command_failed_cb (cbs, command_failed);
   mongoc_apm_set_command_succeeded_cb (cbs, command_succeeded);
   mongoc_client_set_apm_callbacks (client, cbs, NULL);
   mongoc_apm_callbacks_destroy (cbs);
}

static BSON_INLINE void
server_changed (const mongoc_apm_server_changed_t *event)
{
   const mongoc_server_description_t *prev_sd, *new_sd;

   prev_sd = mongoc_apm_server_changed_get_previous_description (event);
   new_sd = mongoc_apm_server_changed_get_new_description (event);

   MONGOC_DEBUG ("server changed: %s %s -> %s",
           mongoc_apm_server_changed_get_host (event)->host_and_port,
           mongoc_server_description_type (prev_sd),
           mongoc_server_description_type (new_sd));
}


static BSON_INLINE void
server_opening (const mongoc_apm_server_opening_t *event)
{
   MONGOC_DEBUG ("server opening: %s", mongoc_apm_server_opening_get_host (event)->host_and_port);
}


static BSON_INLINE void
server_closed (const mongoc_apm_server_closed_t *event)
{
   MONGOC_DEBUG ("server closed: %s", mongoc_apm_server_closed_get_host (event)->host_and_port);
}


static BSON_INLINE void
topology_changed (const mongoc_apm_topology_changed_t *event)
{
   const mongoc_topology_description_t *td;
   mongoc_server_description_t **sds;
   size_t i, n;

   MONGOC_DEBUG ("topology_changed");
   MONGOC_DEBUG ("old topology description:");
   td = mongoc_apm_topology_changed_get_previous_description (event);
   sds = mongoc_topology_description_get_servers (td, &n);
   for (i = 0; i < n; i++) {
      mongoc_server_description_t *sd = sds[i];
      MONGOC_DEBUG ("- %s : %s",
         mongoc_server_description_host (sd)->host_and_port,
         mongoc_server_description_type (sd));
   }
   mongoc_server_descriptions_destroy_all (sds, n);
   MONGOC_DEBUG ("new topology description:");
   td = mongoc_apm_topology_changed_get_new_description (event);
   sds = mongoc_topology_description_get_servers (td, &n);
   for (i = 0; i < n; i++) {
      mongoc_server_description_t *sd = sds[i];
      MONGOC_DEBUG ("- %s : %s",
         mongoc_server_description_host (sd)->host_and_port,
         mongoc_server_description_type (sd));
   }
   mongoc_server_descriptions_destroy_all (sds, n);
}


static BSON_INLINE void
topology_opening (const mongoc_apm_topology_opening_t *event)
{
   MONGOC_DEBUG ("topology opening");
}


static BSON_INLINE void
topology_closed (const mongoc_apm_topology_closed_t *event)
{
   MONGOC_DEBUG ("topology closed");
}


static BSON_INLINE void
server_heartbeat_started (const mongoc_apm_server_heartbeat_started_t *event)
{
   MONGOC_DEBUG ("%s heartbeat started", mongoc_apm_server_heartbeat_started_get_host (event)->host_and_port);
}


static BSON_INLINE void
server_heartbeat_succeeded (
   const mongoc_apm_server_heartbeat_succeeded_t *event)
{
   char *reply;

   reply = bson_as_canonical_extended_json (
      mongoc_apm_server_heartbeat_succeeded_get_reply (event), NULL);

   MONGOC_DEBUG (
      "%s heartbeat succeeded: %s",
      mongoc_apm_server_heartbeat_succeeded_get_host (event)->host_and_port,
      reply);

   bson_free (reply);
}


static BSON_INLINE void
server_heartbeat_failed (const mongoc_apm_server_heartbeat_failed_t *event)
{
   bson_error_t error;

   mongoc_apm_server_heartbeat_failed_get_error (event, &error);

   MONGOC_DEBUG ("%s heartbeat failed: %s",
           mongoc_apm_server_heartbeat_failed_get_host (event)->host_and_port,
           error.message);
}

static BSON_INLINE void set_sdam_monitors (mongoc_apm_callbacks_t *cbs) {
   mongoc_apm_set_server_changed_cb (cbs, server_changed);
   mongoc_apm_set_server_opening_cb (cbs, server_opening);
   mongoc_apm_set_server_closed_cb (cbs, server_closed);
   mongoc_apm_set_topology_changed_cb (cbs, topology_changed);
   mongoc_apm_set_topology_opening_cb (cbs, topology_opening);
   mongoc_apm_set_topology_closed_cb (cbs, topology_closed);
   mongoc_apm_set_server_heartbeat_started_cb (cbs, server_heartbeat_started);
   mongoc_apm_set_server_heartbeat_succeeded_cb (cbs,
                                                 server_heartbeat_succeeded);
   mongoc_apm_set_server_heartbeat_failed_cb (cbs, server_heartbeat_failed);
}
