// mailbox.c (BOX42 unified session version)

#include "mail.h"
#include "session.h"
#include "hystack.h"
#include "sha256.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/* ------------------------------------------------------------
   Mail buffer size (fits typical 1200-baud usage)
------------------------------------------------------------ */
#define MAIL_BUFFER_SIZE 4096

/* ------------------------------------------------------------
   Ensure Session has a mail buffer
------------------------------------------------------------ */
static void ensure_mail_buffer(Session *s) {
    if (!s->mail_buffer) {
        s->mail_buffer = malloc(MAIL_BUFFER_SIZE);
        if (s->mail_buffer)
            s->mail_buffer[0] = 0;
    }
}

/* ------------------------------------------------------------
   Start mail mode
------------------------------------------------------------ */
void mailbox_start(Session *s) {
    if (!s) return;

    ensure_mail_buffer(s);
    if (!s->mail_buffer) return;

    s->mode = SESSION_MODE_MAIL;
    s->mail_buffer[0] = 0;

    session_write(s,
        "Enter message. End with a single dot on a new/empty line.\r\n\r\n"
    );
}

/* ------------------------------------------------------------
   Process each line
------------------------------------------------------------ */
void mailbox_process_line(Session *s, const char *line) {
    if (!s || !line) return;

    ensure_mail_buffer(s);
    if (!s->mail_buffer) return;

    /* End of message */
    if (strcmp(line, ".") == 0) {
        mailbox_store(s);
        session_write(s, "Message sent.\r\n\r\n");
        s->mode = SESSION_MODE_COMMAND;
        return;
    }

    /* Append line */
    size_t used = strlen(s->mail_buffer);
    size_t free = MAIL_BUFFER_SIZE - used - 2;

    if (free > 0) {
        strncat(s->mail_buffer, line, free);
        strcat(s->mail_buffer, "\n");
    }
}

/* ------------------------------------------------------------
   Store mail to file
------------------------------------------------------------ */
void mailbox_store(Session *s) {
    if (!s || !s->mail_buffer) return;

    char hash[SHA256_HEX_LENGTH];
    sha256_hex((uint8_t*)s->username, strlen(s->username), hash);

    char filename[256];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    snprintf(filename, sizeof(filename),
        "data/mail/%04d%02d%02d-%02d%02d-%s.mail",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        tm->tm_hour, tm->tm_min,
        s->username
    );

    FILE *f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "FROM_HASH: %s\n", hash);
    fprintf(f, "TO: admin\n");
    fprintf(f, "DATE: %04d-%02d-%02d %02d:%02d\n",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        tm->tm_hour, tm->tm_min
    );
    fprintf(f, "CONTENT:\n%s\n", s->mail_buffer);

    fclose(f);

    s->mail_buffer[0] = 0;
}
