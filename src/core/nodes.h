#ifndef BOX42_NODES_H
#define BOX42_NODES_H

#define BOX42_MAX_NODES 64

extern int node_limit;

int nodes_init(void);
int nodes_list(int fd);
int node_show(int fd, int id);

int node_set_name(int id, const char *name);
int node_set_status(int id, const char *status);

const char *node_get_name(int id);
const char *node_get_status(int id);

#endif
