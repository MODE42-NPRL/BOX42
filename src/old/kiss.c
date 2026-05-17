#include "kiss.h"
#include <string.h>

#define MAXBUF 2048

static uint8_t buf[4][MAXBUF];
static int pos[4];
static int esc[4];
static int ready[4];

void kiss_init(void) {
    memset(buf, 0, sizeof(buf));
    memset(pos, 0, sizeof(pos));
    memset(esc, 0, sizeof(esc));
    memset(ready, 0, sizeof(ready));
}

int kiss_feed(int port, const uint8_t *in, int len) {
    for (int i = 0; i < len; i++) {
        uint8_t c = in[i];

        if (c == 0xC0) {
            if (pos[port] > 0) {
                ready[port] = pos[port];
                pos[port] = 0;
                esc[port] = 0;
                return 1;
            }
            pos[port] = 0;
            esc[port] = 0;
            continue;
        }

        if (c == 0xDB) {
            esc[port] = 1;
            continue;
        }

        if (esc[port]) {
            if (c == 0xDC) c = 0xC0;
            else if (c == 0xDD) c = 0xDB;
            esc[port] = 0;
        }

        if (pos[port] < MAXBUF)
            buf[port][pos[port]++] = c;
    }

    return 0;
}

int kiss_get_frame(int port, uint8_t *out, int max) {
    if (!ready[port]) return 0;
    int n = ready[port];
    if (n > max) n = max;
    memcpy(out, buf[port], n);
    ready[port] = 0;
    return n;
}
