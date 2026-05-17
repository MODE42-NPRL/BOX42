#ifndef MAIL_H
#define MAIL_H

#include "session.h"

int mail_init(const char *db_path);

int mail_send(const char *sender,
              const char *receiver,
              const char *subject,
              const char *body);

int mail_list(Session *s, const char *user);
int mail_read(Session *s, int id);
int mail_delete(Session *s, int id);

/* Session UI */
void mailbox_start(Session *s);
void mailbox_process_line(Session *s, const char *line);
void mailbox_store(Session *s);

#endif
