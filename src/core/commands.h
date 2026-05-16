#ifndef BOX42_COMMANDS_H
#define BOX42_COMMANDS_H

/* sysinfo */
int sysinfo_dispatch(int fd, const char *line);
int sysinfo_basic(int fd);
int sysinfo_help(int fd);
int sysinfo_stats(int fd);
int sysinfo_users(int fd);
int sysinfo_nodes(int fd);
int sysinfo_mynode(int fd);

/* help */
int cmd_help(int fd, const char *line);

/* role check */
int command_is_allowed(int fd, const char *cmd);

#endif
