#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "session.h"
#include "nodes.h"
#include "storage.h"
#include "term.h"
#include "system.h"

/* ---------------------------------------------------------
   Rollen:
   0 Owner
   1 Admin
   2 Mod
   3 User
   4 Guest
--------------------------------------------------------- */

int command_is_allowed(int fd, const char *cmd)
{
    struct session *s = session_get(fd);
    int lvl = s->level;

    /* Owner darf alles */
    if (lvl == 0)
        return 1;

    /* Node commands nur Owner */
    if (!strcmp(cmd, "node"))
        return 0;

    /* System commands nur Owner+Admin */
    if (!strcmp(cmd, "system"))
        return (lvl <= 1);

    /* Mail: Guests nicht */
    if (!strcmp(cmd, "mail"))
        return (lvl <= 3);

    /* Removeme: Guests nicht */
    if (!strcmp(cmd, "removeme"))
        return (lvl <= 3);

    /* Alles andere erlaubt */
    return 1;
}

/* ---------------------------------------------------------
   #help und #help <command>
--------------------------------------------------------- */

struct help_entry {
    const char *cmd;
    const char *shorttext;
    const char *longtext;
};

static struct help_entry help_table[] = {

    { "login", "Login to an account.",
      "#login <username>\nLogs into an existing account." },

    { "registration", "Create a new account.",
      "#registration <username>\nCreates a new user account." },

    { "focus", "Change active focus area.",
      "#focus <chat|mail|box|system|idle>\nChanges the active focus area." },

    { "mail", "Send a message.",
      "#mail <user|admin|owner|mod>\nStarts composing a mail.\nEnd with a single dot." },

    { "sysinfo", "System information.",
      "#sysinfo\nBasic info.\n"
      "#sysinfo help\nList subcommands.\n"
      "#sysinfo stats\nSystem statistics.\n"
      "#sysinfo users\nUser role counts.\n"
      "#sysinfo nodes\nNode status summary.\n"
      "#sysinfo mynode\nYour current node." },

    { "node", "Owner-only node management.",
      "node list\nnode <id>\nnode <id> up|down\nnode <id> name <text>" },

    { "join", "Join chat.", "#join\nJoins the chat." },
    { "leave", "Leave chat.", "#leave\nLeaves the chat." },
    { "say", "Send chat message.", "#say <text>\nSends a chat message." },
    { "who", "List active users.", "#who\nLists active users." },

    { "logoff", "Log off.", "#logoff\nLogs off the current session." },
    { "exit", "Exit session.", "#exit\nCloses the connection." },

    { "removeme", "Delete your account.",
      "#removeme true\nDeletes your account permanently." },

    { NULL, NULL, NULL }
};

static int help_short(int fd)
{
    term_write(fd, "Available commands:\n");

    struct session *s = session_get(fd);

    for (int i = 0; help_table[i].cmd; i++) {
        if (!command_is_allowed(fd, help_table[i].cmd))
            continue;

        char buf[128];
        snprintf(buf, sizeof(buf), "#%s - %s\n",
                 help_table[i].cmd,
                 help_table[i].shorttext);
        term_write(fd, buf);
    }

    return 0;
}

static int help_long(int fd, const char *cmd)
{
    for (int i = 0; help_table[i].cmd; i++) {
        if (!strcmp(help_table[i].cmd, cmd)) {

            if (!command_is_allowed(fd, cmd)) {
                term_write(fd, "Access denied.\n");
                return 0;
            }

            term_write(fd, help_table[i].longtext);
            term_write(fd, "\n");
            return 0;
        }
    }

    term_write(fd, "No help for this command.\n");
    return 0;
}

int cmd_help(int fd, const char *line)
{
    char cmd[32];

    if (sscanf(line, "#help %31s", cmd) == 1)
        return help_long(fd, cmd);

    return help_short(fd);
}

/* ---------------------------------------------------------
   #sysinfo dispatcher
--------------------------------------------------------- */

int sysinfo_dispatch(int fd, const char *line)
{
    char sub[32];

    if (sscanf(line, "#sysinfo %31s", sub) != 1)
        return sysinfo_basic(fd);

    if (!strcmp(sub, "help"))   return sysinfo_help(fd);
    if (!strcmp(sub, "stats"))  return sysinfo_stats(fd);
    if (!strcmp(sub, "users"))  return sysinfo_users(fd);
    if (!strcmp(sub, "nodes"))  return sysinfo_nodes(fd);
    if (!strcmp(sub, "mynode")) return sysinfo_mynode(fd);

    term_write(fd, "Unknown sysinfo command.\n");
    return 0;
}
