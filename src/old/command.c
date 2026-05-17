#include <string.h>
#include "command.h"
#include "chat.h"

#include "login.h"
#include "registration.h"
#include "userdata.h"
#include "focus.h"
#include "session_api.h"
#include "removeme.h"
#include "chat_commands.h"
#include "mail_start.h"
#include "node.h"
#include "system_command.h"
#include "sysinfo.h"
#include "help.h"

/* ---------------------------------------------------------
   Hilfsfunktion: prüft ob Zeile mit Command beginnt
--------------------------------------------------------- */
static int cmd_is(const char *cmd, const char *line)
{
    return strncmp(line, cmd, strlen(cmd)) == 0;
}

/* ---------------------------------------------------------
   Haupt-Parser
--------------------------------------------------------- */
int command_process(int fd, const char *line)
{
    /* sysinfo */
    if (cmd_is("#sysinfo", line))
        return sysinfo_dispatch(fd, line);

    /* help */
    if (cmd_is("#help", line))
        return cmd_help(fd, line);

    /* login */
    if (cmd_is("#login", line))
        return login_process(fd, line);

    /* registration */
    if (cmd_is("#registration", line))
        return registration_process(fd, line);

    /* userdata */
    if (cmd_is("#userdata", line))
        return userdata_process(fd);

    /* focus */
    if (cmd_is("#focus", line))
        return focus_process(fd, line);

    /* logoff */
    if (cmd_is("#logoff", line))
        return session_logoff(fd);

    /* exit */
    if (cmd_is("#exit", line))
        return session_exit(fd);

    /* removeme */
    if (cmd_is("#removeme", line))
        return removeme_process(fd, line);

    /* chat commands */
    if (cmd_is("#join", line)) return chat_join(fd);
    if (cmd_is("#leave", line)) return chat_leave(fd);
    if (cmd_is("#say", line)) return chat_say(fd, line);
    if (cmd_is("#who", line)) return chat_who(fd);

    /* mail */
    if (cmd_is("#mail", line))
        return mail_start(fd, line);

    /* node (owner only) */
    if (cmd_is("node", line))
        return node_command(fd, line);

    /* system commands (owner+admin) */
    if (cmd_is("system", line))
        return system_command(fd, line);

    /* fallback: focus engine */
    return focus_input(fd, line);
}
