#include "session.h"
#include <stdio.h>
#include <string.h>

static Session g_sessions[512];

Session *session_get(int id) {
    if (id < 0 || id >= 512) return NULL;
    return &g_sessions[id];
}

int session_find_by_username(const char *name) {
    for (int i = 0; i < 512; i++) {
        if (strcmp(g_sessions[i].username, name) == 0)
            return i;
    }
    return -1;
}

void session_send_text(int sid, const char *text) {
    printf("[SEND to %d] %s\n", sid, text);
}

void session_close(Session *s) {
    s->mode = MODE_NORMAL;
    s->private_peer = -1;
}

Session *session_create(int fd) {
    Session *s = malloc(sizeof(Session));
    if (!s) return NULL;
    memset(s, 0, sizeof(Session));
    s->fd = fd;
    return s;
}

void session_destroy(Session *s) {
    if (!s) return;
    if (s->fd >= 0) close(s->fd);
    free(s);
}

void session_reset(Session *s) {
    if (!s) return;
    s->proto = 0;
    s->use_up42 = 0;
    s->port = -1;
    s->buflen = 0;
}
