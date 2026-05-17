#include "tunnel.h"
#include "session.h"
#include <unistd.h>

void tunnel_handle(Session *s) {
    uint8_t buf[2048];
    ssize_t n = read(s->fd, buf, sizeof(buf));
    if (n <= 0) return;

    if (s->use_up42) {
    } else {
    }
}
