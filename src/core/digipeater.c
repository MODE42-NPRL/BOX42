#include "digipeater.h"
#include "events.h"
#include "events_ids.h"
#include <string.h>

#define DIGI_PORTS 4

static int digi_enabled[DIGI_PORTS];
static int digi_cross[DIGI_PORTS][DIGI_PORTS];

static int is_valid_call(const char *c) {
    int len = (int)strnlen(c, AX25_MAX_CALL);
    return (len > 0 && len < AX25_MAX_CALL);
}

void digipeater_init(void) {
    for (int i = 0; i < DIGI_PORTS; i++) {
        digi_enabled[i] = 0;
        for (int j = 0; j < DIGI_PORTS; j++)
            digi_cross[i][j] = 0;
    }

    // Beispiel: Port 0 <-> Port 1
    digi_enabled[0] = 1;
    digi_enabled[1] = 1;

    digi_cross[0][1] = 1;
    digi_cross[1][0] = 1;
}

void digipeater_process(const AX25_Frame *f) {
    if (!f) return;
    if (f->port < 0 || f->port >= DIGI_PORTS) return;
    if (!digi_enabled[f->port]) return;

    if (!is_valid_call(f->src) || !is_valid_call(f->dst))
        return;

    for (int out = 0; out < DIGI_PORTS; out++) {
        if (!digi_enabled[out]) continue;
        if (!digi_cross[f->port][out]) continue;
        if (out == f->port) continue;

        AX25_Frame out_f = *f;
        out_f.port = out;

        event_emit(EVT_AX25_TX, &out_f);
    }
}
