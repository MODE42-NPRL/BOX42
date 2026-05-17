#include "transport_forward.h"
#include "events.h"
#include "events_ids.h"
#include <string.h>

#define TF_PORTS 4
#define TF_RULES 8

typedef struct {
    int  used;
    int  in_port;              // -1 = any
    int  out_port;             // 0..3
    char dst[AX25_MAX_CALL];   // "" = any
} TF_Rule;

static TF_Rule rules[TF_RULES];

static int match_call(const char *rule, const char *dst) {
    if (!rule || !dst) return 0;
    if (rule[0] == '\0') return 1;
    return strncmp(rule, dst, AX25_MAX_CALL) == 0;
}

void transport_forward_init(void) {
    for (int i = 0; i < TF_RULES; i++) {
        rules[i].used    = 0;
        rules[i].in_port = -1;
        rules[i].out_port= -1;
        rules[i].dst[0]  = '\0';
    }

    // Beispiel: alles an DB0XYZ von Port 0 nach Port 1
    rules[0].used     = 1;
    rules[0].in_port  = 0;
    rules[0].out_port = 1;
    strncpy(rules[0].dst, "DB0XYZ", AX25_MAX_CALL - 1);
    rules[0].dst[AX25_MAX_CALL - 1] = '\0';
}

void transport_forward_process(const AX25_Frame *f) {
    if (!f) return;
    if (f->port < 0 || f->port >= TF_PORTS) return;

    for (int i = 0; i < TF_RULES; i++) {
        TF_Rule *r = &rules[i];
        if (!r->used) continue;

        if (r->in_port != -1 && r->in_port != f->port)
            continue;

        if (!match_call(r->dst, f->dst))
            continue;

        if (r->out_port < 0 || r->out_port >= TF_PORTS)
            continue;
        if (r->out_port == f->port)
            continue;

        AX25_Frame out = *f;
        out.port = r->out_port;

        event_emit(EVT_AX25_TX, &out);
    }
}
