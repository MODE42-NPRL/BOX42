#include "beacon.h"
#include "ax25.h"
#include "events.h"
#include "events_ids.h"

#include <string.h>

#define BEACON_PORTS 4

typedef struct {
    int      enabled;
    int      port;              // 0..3
    unsigned interval_ticks;    // in CoreLoop-Ticks
    unsigned counter;
    char     dst[AX25_MAX_CALL];
    char     text[AX25_MAX_PAYLOAD];
} BeaconCfg;

static BeaconCfg beacons[BEACON_PORTS];

void beacon_init(void) {
    for (int i = 0; i < BEACON_PORTS; i++) {
        beacons[i].enabled        = 0;
        beacons[i].port           = i;
        beacons[i].interval_ticks = 0;
        beacons[i].counter        = 0;
        beacons[i].dst[0]         = '\0';
        beacons[i].text[0]        = '\0';
    }

    // Beispiel: Port 0 sendet alle 60s einen Beacon
    beacons[0].enabled        = 1;
    beacons[0].port           = 0;
    beacons[0].interval_ticks = 600; // bei 100ms CoreLoop-Tick
    strncpy(beacons[0].dst, "APRS", AX25_MAX_CALL - 1);
    beacons[0].dst[AX25_MAX_CALL - 1] = '\0';
    strncpy(beacons[0].text, "BOX42-1 Node", AX25_MAX_PAYLOAD - 1);
    beacons[0].text[AX25_MAX_PAYLOAD - 1] = '\0';
}

void beacon_tick(void) {
    for (int i = 0; i < BEACON_PORTS; i++) {
        BeaconCfg *b = &beacons[i];
        if (!b->enabled || b->interval_ticks == 0)
            continue;

        b->counter++;
        if (b->counter < b->interval_ticks)
            continue;

        b->counter = 0;

        AX25_Frame f;
        memset(&f, 0, sizeof(f));

        strncpy(f.src, "BOX42-1", AX25_MAX_CALL - 1);
        f.src[AX25_MAX_CALL - 1] = '\0';
        strncpy(f.dst, b->dst, AX25_MAX_CALL - 1);
        f.dst[AX25_MAX_CALL - 1] = '\0';

        int len = (int)strnlen(b->text, AX25_MAX_PAYLOAD);
        if (len > 0) {
            memcpy(f.payload, b->text, len);
            f.payload_len = len;
        } else {
            f.payload_len = 0;
        }

        f.port = b->port;

        event_emit(EVT_AX25_TX, &f);
    }
}
