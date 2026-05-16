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
