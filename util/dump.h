#ifndef KEVINALBS_DUMP_H
#define KEVINALBS_DUMP_H

#include "mongoc.h"

static inline void
host_list_dump (mongoc_host_list_t *hosts)
{
   mongoc_host_list_t *host;

   MONGOC_DEBUG ("mongoc_host_list_t dump:");

   for (host = hosts; host; host = host->next)
   {
      MONGOC_DEBUG ("- %s", host->host_and_port);
   }
}

static inline void
topology_description_dump (const mongoc_topology_description_t *td)
{
   size_t i;
   const mongoc_server_description_t *sd;
   const mongoc_set_t *servers = mc_tpld_servers_const (td);

   MONGOC_DEBUG ("mongoc_topology_description_t dump:");
   for (i = 0; i < servers->items_len; ++i) {
      sd = mongoc_set_get_item_const (servers, (int) i);
      MONGOC_DEBUG ("- %s [%s]", mongoc_server_description_host(sd)->host_and_port, mongoc_server_description_type (sd));
   }
}

#endif /* KEVINALBS_DUMP_H */
