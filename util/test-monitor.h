
typedef struct {
   mongoc_apm_callbacks_t *callbacks;
   mongoc_array_t cmds;
   bool debug;
} test_monitor_t;

static void
test_monitor_command_started (const mongoc_apm_command_started_t *event)
{
   test_monitor_t *tm = mongoc_apm_command_started_get_context (event);
   if (tm->debug) {
      char *cmd_str = bson_as_relaxed_extended_json (
         mongoc_apm_command_started_get_command (event), NULL);
      MONGOC_DEBUG ("Command %s started on %s: %s",
                    mongoc_apm_command_started_get_command_name (event),
                    mongoc_apm_command_started_get_host (event)->host,
                    cmd_str);

      bson_free (cmd_str);
   }

   bson_t *to_append =
      bson_copy (mongoc_apm_command_started_get_command (event));
   _mongoc_array_append_val (&tm->cmds, to_append);
}

test_monitor_t *
test_monitor_new (void)
{
   test_monitor_t *tm = bson_malloc0 (sizeof (test_monitor_t));
   _mongoc_array_init (&tm->cmds, sizeof (bson_t *));
   tm->callbacks = mongoc_apm_callbacks_new ();
   mongoc_apm_set_command_started_cb (tm->callbacks,
                                      test_monitor_command_started);
   return tm;
}

// test_monitor_debug prints events as they are captured.
void
test_monitor_debug (test_monitor_t *tm)
{
   tm->debug = true;
}

void
test_monitor_apply (test_monitor_t *tm, mongoc_client_t *client)
{
   ASSERT (mongoc_client_set_apm_callbacks (client, tm->callbacks, tm));
}


const bson_t *
test_monitor_get_last (const test_monitor_t *tm)
{
   ASSERT_WITH_MSG (
      tm->cmds.len > 0,
      "expected to have a captured a command started event, but have none");
   return _mongoc_array_index (&tm->cmds, bson_t *, tm->cmds.len - 1);
}

void
test_monitor_destroy (test_monitor_t *tm)
{
   if (!tm) {
      return;
   }
   for (size_t i = 0; i < tm->cmds.len; i++) {
      bson_t *el = _mongoc_array_index (&tm->cmds, bson_t *, i);
      bson_destroy (el);
   }
   _mongoc_array_destroy (&tm->cmds);
}

#define test_monitor_assert_last_matches(tm, pattern) \
   assert_match_bson (test_monitor_get_last (tm), pattern, false);
