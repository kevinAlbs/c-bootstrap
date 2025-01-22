#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bson/bson.h>

/*
 * "warm-up" `num_clients` clients by sending a "ping" command to each server.
 *
 * Returns 'false' on error
 */
bool client_pool_warmup(
    mongoc_client_pool_t *pool, size_t num_clients, bson_error_t *error)
{
    mongoc_client_t **clients = bson_malloc0(num_clients * sizeof(*clients));
    bool ok = false;
    bson_t *ping_command = BCON_NEW("ping", BCON_INT32(1));
    mongoc_client_t *client = NULL;

    // Get the set of server IDs.
    uint32_t *server_ids = NULL;
    size_t num_servers;
    {
        // Send a "ping" on one client to force initial server discovery.
        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        if (!mongoc_client_command_simple(client, "admin", ping_command, NULL, NULL, error))
        {
            mongoc_client_pool_push(pool, client);
            goto done;
        }
        mongoc_server_description_t **descriptions =
            mongoc_client_get_server_descriptions(client, &num_servers);
        server_ids = bson_malloc(sizeof(uint32_t) * num_servers);
        for (size_t i = 0; i < num_servers; i++)
        {
            server_ids[i] = mongoc_server_description_id(descriptions[i]);
        }
        mongoc_server_descriptions_destroy_all(descriptions, num_servers);
        mongoc_client_pool_push(pool, client);
    }

    for (size_t i = 0; i < num_clients; i++)
    {
        client = mongoc_client_pool_pop(pool);
        clients[i] = client;
        // Send a "ping" command to each known server.
        for (size_t i = 0; i < num_servers; i++)
        {
            uint32_t server_id = server_ids[i];
            if (!mongoc_client_command_simple_with_server_id(client, "admin", ping_command, NULL, server_id, NULL, error))
            {
                goto done;
            }
        }
    }

    ok = true;

done:
    for (size_t i = 0; i < num_clients; i++)
    {
        if (!clients[i])
        {
            break;
        }
        mongoc_client_pool_push(pool, clients[i]);
    }

    bson_free(server_ids);
    bson_destroy(ping_command);
    bson_free(clients);
    return ok;
}

int main()
{
    bson_error_t error;
    int rc = EXIT_SUCCESS;

    mongoc_init();

    mongoc_uri_t *uri = mongoc_uri_new("mongodb://localhost:27030,localhost:27031,localhost:27032/?maxPoolSize=100");
    mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);
    if (!client_pool_warmup(pool, 20 /* num clients */, &error))
    {
        fprintf(stderr, "Error warming up connections: %s", error.message);
        rc = EXIT_FAILURE;
        goto done;
    }

    printf("Connections are warmed up.\n");

done:
    mongoc_uri_destroy(uri);
    mongoc_client_pool_destroy(pool);

    mongoc_cleanup();

    return rc;
}
