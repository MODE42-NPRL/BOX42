#include "ax25_sniffer.h"
#include "events.h"
#include "events_ids.h"

#include <stdio.h>

static void ax25_sniffer_on_rx(void *data) {
    AX25_Frame *f = (AX25_Frame *)data;
    if (!f) return;

    printf("[HEX] p%d len=%d\n", f->port, f->payload_len);

    for (int i = 0; i < f->payload_len; i++) {
        if (i % 16 == 0)
            printf("  %03d: ", i);
        printf("%02X ", f->payload[i]);
        if (i % 16 == 15)
            printf("\n");
    }
    if (f->payload_len % 16 != 0)
        printf("\n");
}

void ax25_sniffer_init(void) {
    event_register(EVT_AX25_RX, ax25_sniffer_on_rx);
}
