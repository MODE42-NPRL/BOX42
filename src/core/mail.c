#include "mail.h"
#include <stdio.h>

void mail_send(Session *from, const char *to, const char *msg)
{
    /* Platzhalter – später SQLite oder Filesystem */
    printf("[MAIL] %s -> %s: %s\n", from->username, to, msg);
}

void mail_list(Session *s)
{
    /* Platzhalter */
    printf("[MAIL] Listing mails for %s (not implemented)\n", s->username);
}

void mail_read(Session *s, int id)
{
    /* Platzhalter */
    printf("[MAIL] Reading mail %d for %s (not implemented)\n", id, s->username);
}
