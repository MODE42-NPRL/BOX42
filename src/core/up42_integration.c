#include "up42_integration.h"
#include "session.h"
#include <unistd.h>
#include <string.h>

int up42_encode(Session *s, const uint8_t *in, int len, uint8_t *out, int max) {
    if (!s || !in || !out || len <= 0 || max <= 0) return 0;
    if (len > max) len = max;
    memcpy(out, in, len);
    return len;
}

int up42_decode(Session *s, const uint8_t *in, int len, uint8_t *out, int max) {
    if (!s || !in || !out || len <= 0 || max <= 0) return 0;
    if (len > max) len = max;
    memcpy(out, in, len);
    return len;
}

void up42_process(Session *s) {
    uint8_t buf[2048];
    ssize_t n = read(s->fd, buf, sizeof(buf));
    if (n <= 0) return;

    uint8_t out[2048];
    int m = up42_decode(s, buf, n, out, sizeof(out));
    if (m <= 0) return;

    write(s->fd, out, m);
}
