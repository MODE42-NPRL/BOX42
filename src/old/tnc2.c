#include <stdio.h>
#include <string.h>

#include "../../core/tnc.h"
#include "../../core/tnc_kiss.h"
#include "../../core/ax25.h"
#include "../../core/events.h"

static int fd = -1;
static int kiss_port = 0;

void box42_init() {
    // später: Config laden
    fd = tnc_open("/dev/ttyUSB0", 1200);
    kiss_port = 0;

    if (fd < 0)
        printf("[TNC2C] open failed\n");
    else
        printf("[TNC2C] opened\n");
}

void box42_tick() {
    if (fd < 0) return;

    unsigned char buf[512];
    int n = tnc_read(fd, buf, sizeof(buf));
    if (n <= 0) return;

    unsigned char ax25_raw[330];
    int axlen = kiss_decode(buf, n, ax25_raw, sizeof(ax25_raw));
    if (axlen <= 0) return;

    AX25_Frame f;
    if (ax25_parse(ax25_raw, axlen, &f)) {
        event_emit(10, &f);
    }
}

void box42_stop() {
    if (fd >= 0) tnc_close(fd);
}
