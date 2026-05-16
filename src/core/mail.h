#ifndef MAIL_H
#define MAIL_H

#include "session.h"

/*
 * Minimal Mail-API für BOX42.
 * command.c erwartet diese Funktionen.
 */

/* Schreibe eine Mail an einen Benutzer */
void mail_send(Session *from, const char *to, const char *msg);

/* Liste Mails für einen Benutzer */
void mail_list(Session *s);

/* Zeige eine einzelne Mail */
void mail_read(Session *s, int id);

#endif
