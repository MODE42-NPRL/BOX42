#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "session.h"
#include "chat.h"
#include "focus.h"
#include "storage.h"
#include "util.h"
#include "term.h"

/* ---------------------------------------------------------
   Helper
--------------------------------------------------------- */

int cmd_is(const char *cmd, const char *line)
{
    return strncmp(line, cmd, strlen(cmd)) == 0;
}

/* ---------------------------------------------------------
   #login
--------------------------------------------------------- */

int cmd_login(int fd, const char *line)
{
    char user[64], pass[128];

    if (sscanf(line, "#login %63s %127s", user, pass) != 2)
        return term_write(fd, "Usage: #login <user> <pass>\n");

    str_tolower(user);

    if (!storage_user_exists(user))
        return term_write(fd, "Invalid login.\n");

    if (!storage_user_auth(user, pass))
        return term_write(fd, "Invalid login.\n");

    int level = storage_user_level(user);

    session_set_username(fd, user);
    session_set_level(fd, level);

    term_write(fd, "Login successful.\n");
    return 0;
}

/* ---------------------------------------------------------
   #registration
--------------------------------------------------------- */

int cmd_registration(int fd, const char *line)
{
    char user[64], pass[128];

    if (sscanf(line, "#registration %63s %127s", user, pass) != 2)
        return term_write(fd, "Usage: #registration <user> <pass>\n");

    str_tolower(user);

    if (storage_user_exists(user))
        return term_write(fd, "User already exists.\n");

    if (!password_valid(pass))
        return term_write(fd, "Password too weak.\n");

    storage_user_add(user, pass, 1);

    term_write(fd, "Registration complete.\n");
    return 0;
}

/* ---------------------------------------------------------
   #userdata
--------------------------------------------------------- */

int cmd_userdata(int fd, const char *line)
{
    struct session *s = session_get(fd);

    if (s->level == 0) {
        term_write(fd, "User: Guest\nLevel: 0\n");
        return 0;
    }

    char buf[256];
    snprintf(buf, sizeof(buf), "User: %s\nLevel: %d\nPassword: ********\n",
             s->username, s->level);

    term_write(fd, buf);
    return 0;
}

/* ---------------------------------------------------------
   #focus
--------------------------------------------------------- */

int cmd_focus(int fd, const char *line)
{
    return focus_handle(fd, line);
}

/* ---------------------------------------------------------
   Chat Commands
--------------------------------------------------------- */

int cmd_join(int fd, const char *line)
{
    chat_join(fd);
    return 0;
}

int cmd_leave(int fd, const char *line)
{
    chat_leave(fd);
    return 0;
}

int cmd_say(int fd, const char *line)
{
    const char *msg = line + 5; /* skip "#say " */
    chat_say(fd, msg);
    return 0;
}

int cmd_who(int fd, const char *line)
{
    chat_userlist(fd);
    return 0;
}

/* ---------------------------------------------------------
   #logoff / #exit
--------------------------------------------------------- */

int cmd_logoff(int fd, const char *line)
{
    chat_leave(fd);
    session_destroy(fd);
    term_write(fd, "Goodbye.\n");
    os_close(fd);
    return 0;
}

/* ---------------------------------------------------------
   #removeme true
--------------------------------------------------------- */

int cmd_removeme(int fd, const char *line)
{
    if (strcmp(line, "#removeme true") != 0)
        return term_write(fd, "Confirmation required: #removeme true\n");

    struct session *s = session_get(fd);

    if (s->level == 0)
        return term_write(fd, "Guests cannot be removed.\n");

    storage_user_delete(s->username);

    term_write(fd, "Your account has been removed.\n");
    session_destroy(fd);
    os_close(fd);
    return 0;
}
