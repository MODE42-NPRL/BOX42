#include <string.h>
#include <unistd.h>

#include "session.h"
#include "mail.h"

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
   Line processing
------------------------------------------------------------ */
static void strds_process(Session *s, const char *line) {
    if (!s || !line) return;

    if (strcmp(line, "#ESC") == 0) {
        s->active = 0;      /* Session sauber beenden */
        return;
    }

    /* Mail mode: delegate to mailbox */
    if (s->mode == SESSION_MODE_MAIL) {
        mailbox_process_line(s, line);
        return;
    }

    /* Command mode */
    if (strcmp(line, "mail") == 0) {
        s->mode = SESSION_MODE_MAIL;
        mailbox_start(s);
        return;
    }

    if (strcmp(line, "quit") == 0) {
        s->active = 0;
        return;
    }

    /* Default: echo */
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

        /* primitive line split: assume one line per read */
        char *p = strchr(buf, '\n');
        if (p) *p = '\0';

        strds_process(s, buf);

        if (s->active)
            strds_prompt(s);
    }

    session_destroy(fd);
}
