// Repeatedly copies a server description and measures cost.
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>

#include "ubench.h"

UBENCH_EX(mongoc_server_description_new_copy, sdcopy)
{
    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");
    bson_error_t error;
    mongoc_server_description_t *sd = mongoc_client_select_server(client, false, NULL, &error);

    UBENCH_DO_BENCHMARK()
    {
        for (size_t i = 0; i < 120; i++)
        {
            mongoc_server_description_t *sd_copy = mongoc_server_description_new_copy(sd);
            UBENCH_DO_NOTHING(sd_copy);
            mongoc_server_description_destroy(sd_copy);
        }
    }

    mongoc_server_description_destroy(sd);
    mongoc_client_destroy(client);
}

UBENCH_STATE();

int main(int argc, const char *const argv[])
{
    mongoc_init();
    ubench_main(argc, argv);
    mongoc_cleanup();
}
