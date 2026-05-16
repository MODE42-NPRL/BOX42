#include "session.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
