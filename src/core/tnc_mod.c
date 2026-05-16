#include "config.h"
#include "tnc_types.h"
#include "tnc.h"
#include "tnc_baycom.h"
#include "tnc_kiss.h"
#include "ax25.h"
#include "events.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern BOX42_Config g_cfg;

static int fd = -1;
static TNC_Type ttype;
static int kiss_port = 0;

// --------------------------------------------------
// TX: AX.25-Frame -> (KISS oder RAW) -> TNC
// Event-ID: 11
// --------------------------------------------------
static void on_ax25_tx(void *data) {
    if (fd < 0) return;

    AX25_Frame *f = (AX25_Frame*)data;

    unsigned char raw[330];
    int len = ax25_build(f, raw, sizeof(raw));
    if (len <= 0) return;

    if (ttype == TNC_TYPE_TNC2C) {
        unsigned char kiss[512];
        int klen = kiss_encode(raw, len, kiss, sizeof(kiss), kiss_port);
        if (klen > 0)
            write(fd, kiss, klen);
    } else {
        // BayCom / PC-COM: AX.25 direkt
        write(fd, raw, len);
    }
}

// --------------------------------------------------
// INIT
// --------------------------------------------------
void box42_init() {
    if (!g_cfg.tnc_enabled) {
        printf("[TNC] disabled in config\n");
        return;
    }

    ttype = tnc_type_from_config(g_cfg.tnc_type);
    kiss_port = g_cfg.tnc_kiss_port;

    switch (ttype) {
        case TNC_TYPE_TNC2C:
            fd = tnc_open(g_cfg.tnc_device, g_cfg.tnc_baud);
            printf("[TNC] TNC2C KISS on %s @ %d\n",
                   g_cfg.tnc_device, g_cfg.tnc_baud);
            break;

        case TNC_TYPE_BAYCOM:
            fd = baycom_open(g_cfg.tnc_device, g_cfg.tnc_baud, TNC_TYPE_BAYCOM);
            printf("[TNC] BayCom on %s @ %d\n",
                   g_cfg.tnc_device, g_cfg.tnc_baud);
            break;

        case TNC_TYPE_PCCOM:
            fd = baycom_open(g_cfg.tnc_device, g_cfg.tnc_baud, TNC_TYPE_PCCOM);
            printf("[TNC] PC-COM on %s @ %d\n",
                   g_cfg.tnc_device, g_cfg.tnc_baud);
            break;
    }

    if (fd < 0) {
        printf("[TNC] open failed\n");
        return;
    }

    event_register(11, on_ax25_tx);
}

// --------------------------------------------------
// TICK: RX-Pfad
// --------------------------------------------------
void box42_tick() {
    if (fd < 0) return;

    unsigned char buf[512];
    int n = read(fd, buf, sizeof(buf));
    if (n <= 0) return;

    if (ttype == TNC_TYPE_TNC2C) {
        // KISS -> AX.25
        unsigned char ax25_raw[330];
        int axlen = kiss_decode(buf, n, ax25_raw, sizeof(ax25_raw));
        if (axlen <= 0) return;

        AX25_Frame f;
        if (ax25_parse(ax25_raw, axlen, &f))
            event_emit(10, &f);
    } else {
        // BayCom / PC-COM: AX.25 direkt
        AX25_Frame f;
        if (ax25_parse(buf, n, &f))
            event_emit(10, &f);
    }
}

// --------------------------------------------------
// STOP
// --------------------------------------------------
void box42_stop() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}
