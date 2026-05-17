#include <string.h>
#include "system.h"
#include "session.h"
#include "storage.h"
#include "term.h"
#include "perm.h"

static int system_check_access(int fd)
{
    struct session *s = session_get(fd);
    if (s->level < 3) {
        term_write(fd, "System access denied.\n");
        return 0;
    }
    return 1;
}

int system_cmd_help(int fd)
{
    term_write(fd, "System commands:\n");
    term_write(fd, "  users   - list users\n");
    term_write(fd, "  stats   - system stats\n");
    term_write(fd, "  help    - this help\n");
    return 0;
}

int system_cmd_users(int fd)
{
    /* Platzhalter: später SQLite‑Userliste */
    term_write(fd, "User listing not implemented yet.\n");
    return 0;
}

int system_cmd_stats(int fd)
{
    /* Platzhalter: später Sessions, Uptime, etc. */
    term_write(fd, "System stats not implemented yet.\n");
    return 0;
}

int system_input(int fd, const char *line)
{
    if (!system_check_access(fd))
        return 0;

    if (line[0] == 0)
        return 0;

    if (strcmp(line, "help") == 0)
        return system_cmd_help(fd);

    if (strcmp(line, "users") == 0)
        return system_cmd_users(fd);

    if (strcmp(line, "stats") == 0)
        return system_cmd_stats(fd);

    term_write(fd, "Unknown system command. Type 'help'.\n");
    return 0;
}

int nodes_count_status(const char *status)
{
    int count = 0;

    for (int i = 0; i < node_limit; i++) {
        const char *s = storage_node_get_status(i);
        if (strcmp(s, status) == 0)
            count++;
    }

    return count;
}

int system_cmd_sysinfo(int fd)
{
    if (!system_check_access(fd))
        return 0;

    char utc[32];
    char arch[64];
    char os[64];
    char buf[256];

    sysinfo_get_utc(utc, sizeof(utc));
    sysinfo_get_os(arch, sizeof(arch), os, sizeof(os));

    int online  = nodes_count_status("online");
    int offline = nodes_count_status("offline");
    int locked  = nodes_count_status("locked");

    term_write(fd, config_get("sysinfo_header"));
    term_write(fd, "\n");

    snprintf(buf, sizeof(buf), "%s %s\n",
             config_get("sysinfo_arch"), arch);
    term_write(fd, buf);

    snprintf(buf, sizeof(buf), "%s %s\n",
             config_get("sysinfo_os"), os);
    term_write(fd, buf);

    snprintf(buf, sizeof(buf), "%s %s\n",
             config_get("sysinfo_utc"), utc);
    term_write(fd, buf);

    snprintf(buf, sizeof(buf), "%s %d\n",
             config_get("sysinfo_node_limit"), node_limit);
    term_write(fd, buf);

    snprintf(buf, sizeof(buf),
             "%s %d %s, %d %s, %d %s\n",
             config_get("sysinfo_nodes"),
             online,  config_get("sysinfo_nodes_online"),
             offline, config_get("sysinfo_nodes_offline"),
             locked,  config_get("sysinfo_nodes_locked"));
    term_write(fd, buf);

    return 0;
}
