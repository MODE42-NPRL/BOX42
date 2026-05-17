#ifndef SESSION_H
#define SESSION_H

#include <stddef.h>

#define MAX_SESSIONS 128

typedef struct Session {
    int fd;
    char username[32];
    int level;
    int use_up42;
} Session;

/* API aus session.c */
Session *session_get(int fd);
Session *session_create(int fd, const char *username, int level);
void session_destroy(int fd);

/* Iterator für coreloop */
Session *session_get_by_index(int idx);

#endif
