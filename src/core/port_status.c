#include <stdlib.h>
#include "port_status.h"
#include "events.h"
#include "events_ids.h"
#include "config.h"

#include <stdio.h>
#include <string.h>

#define PS_PORTS 4
#define PS_INTERVAL 100   // alle 100 Ticks (~10s bei 100ms CoreLoop)

static unsigned counter = 0;

static const char *state_str(int enabled, int fd_ok) {
    if (!enabled) return "OFF";
    if (!fd_ok)   return "ERR";
    return "ON";
}

void port_status_init(void) {
    counter = 0;
}

void port_status_tick(void) {
    counter++;
    if (counter < PS_INTERVAL)
        return;

    counter = 0;

    // stdout-clean: nur technische Info, keine sensiblen Daten
    printf("[STATUS] Ports: ");

    for (int i = 0; i < PS_PORTS; i++) {
        int enabled = g_cfg.tnc[i].enabled;
        int fd_ok   = (enabled && g_cfg.tnc[i].device[0] != 0);

        printf("p%d=%s ", i, state_str(enabled, fd_ok));
    }

    printf("\n");
}
