#include "tnc_layer.h"
#include "tnc.h"
#include "config.h"
#include "ax25.h"
#include "ax25_route.h"
#include "kiss.h"

#include <string.h>

static int tnc_fd[4] = { -1, -1, -1, -1 };

static int kiss_encode(uint8_t *out, int max, const uint8_t *in, int len) {
    if (max < len + 2) return -1;
    int p = 0;
    out[p++] = 0xC0;
    out[p++] = 0x00;
    memcpy(&out[p], in, len);
    p += len;
    out[p++] = 0xC0;
    return p;
}

void tnc_layer_init(void) {
    kiss_init();

    for (int i = 0; i < 4; i++) {
        tnc_fd[i] = -1;
        if (g_cfg.tnc[i].enabled && g_cfg.tnc[i].device[0]) {
            tnc_fd[i] = tnc_open(g_cfg.tnc[i].device, g_cfg.tnc[i].baud);
        }
    }
}

int tnc_layer_send(int port, const uint8_t *buf, int len) {
    if (port < 0 || port >= 4) return -1;
    if (tnc_fd[port] < 0) return -1;

    uint8_t kiss[2048];
    int n = kiss_encode(kiss, sizeof(kiss), buf, len);
    if (n <= 0) return -1;

    return tnc_write(tnc_fd[port], kiss, n);
}

void tnc_layer_tick(void) {
    uint8_t inbuf[2048];
    uint8_t axbuf[2048];

    for (int p = 0; p < 4; p++) {
        if (tnc_fd[p] < 0) continue;

        int n = tnc_read(tnc_fd[p], inbuf, sizeof(inbuf));
        if (n <= 0) continue;

        kiss_feed(p, inbuf, n);

        int axlen;
        while ((axlen = kiss_get_frame(p, axbuf, sizeof(axbuf))) > 0) {
            AX25_Frame f;
            if (!ax25_parse(axbuf, axlen, &f)) continue;

            f.port = p;

            int out = ax25_route(&f);
            if (out >= 0)
                tnc_layer_send(out, axbuf, axlen);
        }
    }
}
