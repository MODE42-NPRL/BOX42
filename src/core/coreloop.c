#include "coreloop.h"
#include "events.h"
#include "tnc_mod.h"
#include "beacon.h"
#include "digipeater.h"
#include "transport_forward.h"
#include "events_ids.h"
#include "ax25_dispatch.h"
#include "ax25_monitor.h"

#include <unistd.h>
#include "port_status.h"
#include "ax25_sniffer.h"
#include "textstore.h"

static int running = 0;

static void on_ax25_rx(void *data) {
    AX25_Frame *f = (AX25_Frame *)data;
    ax25_dispatch(f);
}

void coreloop_init(void) {
    events_init();

    event_register(EVT_AX25_RX, on_ax25_rx);
    // Monitor optional
    // ax25_monitor_init();
    ax25_sniffer_init();

    tnc_mod_init();
    beacon_init();
    digipeater_init();
    transport_forward_init();

    running = 1;
}

void coreloop_run(void) {
    while (running) {
        tnc_mod_tick();
        beacon_tick();
        usleep(100000); // 100 ms
    }
}

void coreloop_stop(void) {
    running = 0;
    tnc_mod_stop();
}

void coreloop_run(void) {
    while (running) {
        tnc_mod_tick();
        beacon_tick();
        port_status_tick();
        usleep(100000); // 100 ms
    }
}

tatic int tick_counter = 0;

void coreloop_run(void) {
    while (running) {
        tnc_mod_tick();
        beacon_tick();
        port_status_tick();

        tick_counter++;
        if (tick_counter >= 50) { // alle 5 Sekunden
            textstore_tick();
            tick_counter = 0;
        }

        usleep(100000);
    }
}
