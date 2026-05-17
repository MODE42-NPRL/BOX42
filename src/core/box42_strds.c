#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "session.h"
#include "mail.h"

/* ------------------------------------------------------------
   Init
------------------------------------------------------------ */
void strds_init(void) {
    /* nothing needed yet */
}

static const char *prompt_symbol = "> ";

/* ------------------------------------------------------------
   Prompt
------------------------------------------------------------ */
static void strds_prompt(Session *s) {
    if (!s) return;

    if (s->mode == SESSION_MODE_COMMAND) {
        session_write(s, prompt_symbol);
    }
}

/* ------------------------------------------------------------
   MAIL COMMANDS (SQLite backend)
------------------------------------------------------------ */
static void strds_mail(Session *s, const char *args)
{
    /* --------------------------------------------------------
       GUEST RESTRICTIONS
    -------------------------------------------------------- */
    if (s->role == 0) {  /* guest */
        /* guest darf nur: #mail write admin */
        if (strncmp(args, "write ", 6) == 0) {
            const char *target = args + 6;

            if (strcmp(target, "admin") != 0) {
                session_write(s, "Guests may only write mail to admin.\r\n");
                return;
            }

            strncpy(s->mail_target, "admin", sizeof(s->mail_target)-1);
            s->mail_target[sizeof(s->mail_target)-1] = 0;

            mailbox_start(s);
            return;
        }

        session_write(s,
            "Guest permissions:\r\n"
            "  #mail write admin\r\n"
        );
        return;
    }

    /* --------------------------------------------------------
       NORMAL USERS AND HIGHER
    -------------------------------------------------------- */

    /* no args → list */
    if (!args || strlen(args) == 0 || strcmp(args, "list") == 0) {
        mail_list(s, s->username);
        return;
    }

    /* read */
    if (strncmp(args, "read ", 5) == 0) {
        int id = atoi(args + 5);
        mail_read(s, id);
        return;
    }

    /* delete */
    if (strncmp(args, "del ", 4) == 0) {
        int id = atoi(args + 4);
        mail_delete(s, id);
        return;
    }

    /* write */
    if (strncmp(args, "write ", 6) == 0) {
        const char *target = args + 6;

        strncpy(s->mail_target, target, sizeof(s->mail_target)-1);
        s->mail_target[sizeof(s->mail_target)-1] = 0;

        mailbox_start(s);
        return;
    }

    /* fallback */
    session_write(s,
        "Usage:\r\n"
        "  #mail list\r\n"
        "  #mail read <id>\r\n"
        "  #mail del <id>\r\n"
        "  #mail write <user>\r\n"
    );
}

/* ------------------------------------------------------------
   Line processing
------------------------------------------------------------ */
static void strds_process(Session *s, const char *line) {
    if (!s || !line) return;

    /* ESC → Session beenden */
    if (strcmp(line, "#ESC") == 0) {
        s->active = 0;
        return;
    }

    /* MAIL MODE */
    if (s->mode == SESSION_MODE_MAIL) {
        mailbox_process_line(s, line);
        return;
    }

    /* COMMAND MODE */

    /* MAIL COMMANDS */
    if (strncmp(line, "#mail", 5) == 0) {
        const char *args = line + 5;
        while (*args == ' ') args++;
        strds_mail(s, args);
        return;
    }

    /* QUIT */
    if (strcmp(line, "#quit") == 0) {
        s->active = 0;
        return;
    }

    /* DEFAULT: echo */
    session_write(s, "You said: ");
    session_write(s, line);
    session_write(s, "\n");
}

/* ------------------------------------------------------------
   Session start
------------------------------------------------------------ */
void strds_session_start(int fd) {
    Session *s = session_create(fd, "guest", 0);
    if (!s) return;

    s->mode   = SESSION_MODE_COMMAND;
    s->active = 1;

    strds_prompt(s);

    char buf[512];
    int n;
    while (s->active) {
        n = read(fd, buf, sizeof(buf)-1);
        if (n <= 0) {
            s->active = 0;
            break;
        }

        buf[n] = '\0';

        /* primitive line split */
        char *p = strchr(buf, '\n');
        if (p) *p = '\0';

        strds_process(s, buf);

        if (s->active)
            strds_prompt(s);
    }

    session_destroy(fd);
}
