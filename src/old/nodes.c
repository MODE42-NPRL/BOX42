#include <string.h>
#include "nodes.h"
#include "config.h"
#include "storage.h"
#include "term.h"

int node_limit = 64;

/* ---------------------------------------------------------
   Initialisierung
--------------------------------------------------------- */

int nodes_init(void)
{
    /* max_nodes aus Config lesen */
    int cfg_limit = config_get_int("nodes", "max_nodes");

    if (cfg_limit < 1)
        cfg_limit = 1;

    if (cfg_limit > BOX42_MAX_NODES)
        cfg_limit = BOX42_MAX_NODES;

    node_limit = cfg_limit;

    /* Nodes in SQLite anlegen falls nicht vorhanden */
    for (int i = 0; i < node_limit; i++) {
        if (!storage_node_exists(i)) {
            storage_node_add(
                i,
                config_get("node_default_name"),
                config_get("node_default_status")
            );
        }
    }

    return 0;
}

/* ---------------------------------------------------------
   Node‑Name setzen
--------------------------------------------------------- */

int node_set_name(int id, const char *name)
{
    if (id < 0 || id >= node_limit)
        return -1;

    return storage_node_set_name(id, name);
}

/* ---------------------------------------------------------
   Node‑Status setzen
--------------------------------------------------------- */

int node_set_status(int id, const char *status)
{
    if (id < 0 || id >= node_limit)
        return -1;

    return storage_node_set_status(id, status);
}

/* ---------------------------------------------------------
   Node‑Name holen
--------------------------------------------------------- */

const char *node_get_name(int id)
{
    if (id < 0 || id >= node_limit)
        return NULL;

    return storage_node_get_name(id);
}

/* ---------------------------------------------------------
   Node‑Status holen
--------------------------------------------------------- */

const char *node_get_status(int id)
{
    if (id < 0 || id >= node_limit)
        return NULL;

    return storage_node_get_status(id);
}

/* ---------------------------------------------------------
   Node‑Liste ausgeben (System‑Engine)
--------------------------------------------------------- */

int nodes_list(int fd)
{
    char buf[256];

    for (int i = 0; i < node_limit; i++) {
        snprintf(buf, sizeof(buf),
                 "Node %d: %s (%s)\n",
                 i,
                 storage_node_get_name(i),
                 storage_node_get_status(i));
        term_write(fd, buf);
    }

    return 0;
}

/* ---------------------------------------------------------
   Einzelnen Node anzeigen
--------------------------------------------------------- */

int node_show(int fd, int id)
{
    if (id < 0 || id >= node_limit) {
        term_write(fd, "Invalid node.\n");
        return -1;
    }

    char buf[256];
    snprintf(buf, sizeof(buf),
             "Node %d\nName: %s\nStatus: %s\n",
             id,
             storage_node_get_name(id),
             storage_node_get_status(id));

    term_write(fd, buf);
    return 0;
}
