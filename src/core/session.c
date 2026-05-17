#include "session.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

static Session sessions[MAX_SESSIONS];

/* interne Initialisierung */
static void session_init(void)
{
    static int initialized = 0;
    if (initialized) return;
    initialized = 1;

    for (int i = 0; i < MAX_SESSIONS; i++) {
        sessions[i].fd = -1;
        sessions[i].username[0] = '\0';
        sessions[i].level = 0;
        sessions[i].use_up42 = 0;
    }
}

/* Session anhand FD holen */
Session *session_get(int fd)
{
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == fd)
            return &sessions[i];
    }
    return NULL;
}

/* Neue Session anlegen */
Session *session_create(int fd, const char *username, int level)
{
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == -1) {
            sessions[i].fd = fd;
            strncpy(sessions[i].username, username, sizeof(sessions[i].username)-1);
            sessions[i].username[sizeof(sessions[i].username)-1] = '\0';
            sessions[i].level = level;
            sessions[i].use_up42 = 0;
            return &sessions[i];
        }
    }

    return NULL;
}

/* Session löschen */
void session_destroy(int fd)
{
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == fd) {
            sessions[i].fd = -1;
            sessions[i].username[0] = '\0';
            sessions[i].level = 0;
            sessions[i].use_up42 = 0;
            return;
        }
    }
}

/* Iterator für coreloop */
Session *session_get_by_index(int idx)
{
    session_init();

    if (idx < 0 || idx >= MAX_SESSIONS)
        return NULL;

    if (sessions[idx].fd == -1)
        return NULL;

    return &sessions[idx];
}
