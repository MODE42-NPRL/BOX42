#include "coreloop.h"
#include <stdio.h>

void coreloop_init(CoreLoop *cl) {
    cl->running = 1;
    cl->module_count = 0;
}

void coreloop_add_module(CoreLoop *cl, BOX42_Module m) {
    if (cl->module_count < 16) {
        cl->modules[cl->module_count++] = m;
    } else {
        printf("[BOX42] Too many modules\n");
    }
}

void coreloop_run(CoreLoop *cl) {
    while (cl->running) {
        for (int i = 0; i < cl->module_count; i++) {
            if (cl->modules[i].tick)
                cl->modules[i].tick();
        }
        // später: event_emit(EVENT_TICK, NULL);
    }
}

void coreloop_stop(CoreLoop *cl) {
    cl->running = 0;
}
