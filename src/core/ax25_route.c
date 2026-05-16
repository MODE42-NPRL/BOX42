/* src/core/ax25_route.c */
#include "ax25_route.h"
#include "config.h"
#include <string.h>

extern BOX42_Config g_cfg;

static int match_call(const char *a, const char *b) {
    return strcasecmp(a, b) == 0;
}

static int route_tf(const AX25_Frame *f) {
    for (int i = 0; i < 8; i++) {
        if (!g_cfg.tf[i].used)
            continue;
        if (!match_call(f->dst, g_cfg.tf[i].dst))
            continue;
        if (g_cfg.tf[i].in_port >= 0 && g_cfg.tf[i].in_port != f->port)
            continue;
        return g_cfg.tf[i].out_port;
    }
    return -1;
}

static int route_digi(const AX25_Frame *f) {
    int in = f->port;
    if (in < 0 || in >= 4)
        return -1;
    if (!g_cfg.digi.enabled[in])
        return -1;

    for (int out = 0; out < 4; out++) {
        if (out == in)
            continue;
        if (g_cfg.digi.cross[in][out])
            return out;
    }
    return -1;
}

int ax25_route(const AX25_Frame *f) {
    if (!f)
        return -1;

    int p = route_tf(f);
    if (p >= 0)
        return p;

    p = route_digi(f);
    if (p >= 0)
        return p;

    return -1;
}
