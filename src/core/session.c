#include "session.h"
#include "up42.h"
#include <unistd.h>

void session_init(Session *s, int fd, int proto) {
    s->fd = fd;
    s->proto = proto;
    s->use_up42 = 0;
}

void session_handle(Session *s) {
    uint8_t buf[2048];
    ssize_t n = read(s->fd, buf, sizeof(buf));
    if (n <= 0) return;

    if (s->use_up42) {
        UP42_Frame f;
        if (up42_parse(buf, n, &f) == 0) {
        }
    } else {
    }
}
