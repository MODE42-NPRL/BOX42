#ifndef MAIL_H
#define MAIL_H

#include "session.h"

/* Simple mailbox interface bound to a Session */

void mailbox_start(Session *s);
void mailbox_process_line(Session *s, const char *line);
void mailbox_store(Session *s);

#endif

