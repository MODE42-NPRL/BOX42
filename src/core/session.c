#include "session.h"
#include <string.h>
#include <stdio.h>

#define MAX_SESSIONS 128

static Session sessions[MAX_SESSIONS];

/* Initialisiert alle Sessions beim Start */
static void session_init(void)
{
    static int initialized = 0;
    if (initialized) return;
    initialized = 1;

    for (int i = 0; i < MAX_SESSIONS; i++) {
        sessions[i].fd = -1;
        sessions[i].username[0] = '\0';
        sessions[i].level = 0; /* guest */
    }
}

/* Liefert die Session zu einem fd */
Session *session_get(int fd)
{
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == fd)
            return &sessions[i];
    }
    return NULL;
}

/* Erstellt eine neue Session */
Session *session_create(int fd, const char *username, int level)
{
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == -1) {
            sessions[i].fd = fd;
            strncpy(sessions[i].username, username, sizeof(sessions[i].username)-1);
            sessions[i].username[sizeof(sessions[i].username)-1] = '\0';
            sessions[i].level = level;
            return &sessions[i];
        }
    }

    return NULL; /* kein Platz */
}

/* Entfernt eine Session */
void session_destroy(int fd)
{
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == fd) {
            sessions[i].fd = -1;
            sessions[i].username[0] = '\0';
            sessions[i].level = 0;
            return;
        }
    }
}
