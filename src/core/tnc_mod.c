#include "tnc_mod.h"
#include "tnc_types.h"
#include "tnc.h"
#include "tnc_baycom.h"
#include "tnc_kiss.h"
#include "ax25.h"
#include "events.h"
#include "events_ids.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define TNC_PORTS 4

static int       tnc_fd[TNC_PORTS]        = { -1, -1, -1, -1 };
static TNC_Type  tnc_type_arr[TNC_PORTS]  = { 0 };
static int       tnc_kiss_port[TNC_PORTS] = { 0 };
static int       tnc_enabled[TNC_PORTS]   = { 0 };

static void on_ax25_tx(void *data);

static void tnc_port_close(int i) {
    if (i < 0 || i >= TNC_PORTS) return;
    if (tnc_fd[i] >= 0) {
        close(tnc_fd[i]);
        tnc_fd[i] = -1;
    }
}

void tnc_mod_init(void) {
    for (int i = 0; i < TNC_PORTS; i++) {
        tnc_fd[i]        = -1;
        tnc_enabled[i]   = 0;
        tnc_type_arr[i]  = TNC_TYPE_TNC2C;
        tnc_kiss_port[i] = 0;
    }

    // Beispiel: nur Port 0 aktiv, TNC2C KISS
    tnc_enabled[0]   = 1;
    tnc_type_arr[0]  = TNC_TYPE_TNC2C;
    tnc_kiss_port[0] = 0;

    for (int i = 0; i < TNC_PORTS; i++) {
        if (!tnc_enabled[i]) continue;

        int fd = -1;

        switch (tnc_type_arr[i]) {
            case TNC_TYPE_TNC2C:
                fd = tnc_open("/dev/ttyUSB0", 1200);
                break;
            case TNC_TYPE_BAYCOM:
                fd = baycom_open("/dev/ttyUSB1", 1200, TNC_TYPE_BAYCOM);
                break;
            case TNC_TYPE_PCCOM:
                fd = baycom_open("/dev/ttyUSB2", 1200, TNC_TYPE_PCCOM);
                break;
            default:
                fd = -1;
                break;
        }

        if (fd < 0) {
            printf("[TNC] port %d open failed\n", i);
            tnc_enabled[i] = 0;
        } else {
            tnc_fd[i] = fd;
            printf("[TNC] port %d up (fd=%d)\n", i, fd);
        }
    }

    event_register(EVT_AX25_TX, on_ax25_tx);
}

static void on_tnc_rx(int port, const unsigned char *buf, int n) {
    if (port < 0 || port >= TNC_PORTS) return;
    if (!buf || n <= 0) return;

    uint8_t tmp[330];
    AX25_Frame f;

    if (tnc_type_arr[port] == TNC_TYPE_TNC2C) {
        int axlen = kiss_decode(buf, n, tmp, (int)sizeof(tmp));
        if (axlen <= 0) return;
        if (!ax25_parse(tmp, axlen, &f)) return;
    } else {
        if (!ax25_parse(buf, n, &f)) return;
    }

    f.port = port;
    event_emit(EVT_AX25_RX, &f);
}

static void on_ax25_tx(void *data) {
    AX25_Frame *f = (AX25_Frame *)data;
    if (!f) return;

    int port = f->port;
    if (port < 0 || port >= TNC_PORTS) return;
    if (!tnc_enabled[port]) return;
    if (tnc_fd[port] < 0) return;

    uint8_t raw[330];
    int len = ax25_build(f, raw, (int)sizeof(raw));
    if (len <= 0) return;

    if (tnc_type_arr[port] == TNC_TYPE_TNC2C) {
        uint8_t kiss[512];
        int klen = kiss_encode(raw, len, kiss, (int)sizeof(kiss), tnc_kiss_port[port]);
        if (klen <= 0) return;
        (void)write(tnc_fd[port], kiss, (size_t)klen);
    } else {
        (void)write(tnc_fd[port], raw, (size_t)len);
    }
}

void tnc_mod_tick(void) {
    uint8_t buf[512];

    for (int i = 0; i < TNC_PORTS; i++) {
        if (!tnc_enabled[i]) continue;
        if (tnc_fd[i] < 0) continue;

        ssize_t n = read(tnc_fd[i], buf, sizeof(buf));
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            printf("[TNC] port %d read error, closing\n", i);
            tnc_port_close(i);
            tnc_enabled[i] = 0;
            continue;
        }
        if (n == 0)
            continue;

        on_tnc_rx(i, buf, (int)n);
    }
}

void tnc_mod_stop(void) {
    for (int i = 0; i < TNC_PORTS; i++)
        tnc_port_close(i);
}
