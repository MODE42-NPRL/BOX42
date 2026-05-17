#ifndef BOX42_SYSTEM_H
#define BOX42_SYSTEM_H

int system_input(int fd, const char *line);

/* einzelne System‑Aktionen (optional öffentlich) */
int system_cmd_help(int fd);
int system_cmd_users(int fd);
int system_cmd_stats(int fd);

#endif
