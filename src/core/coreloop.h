#pragma once

#include "modules.h"

typedef struct {
    int running;
    BOX42_Module modules[16];
    int module_count;
} CoreLoop;

void coreloop_init(CoreLoop *cl);
void coreloop_add_module(CoreLoop *cl, BOX42_Module m);
void coreloop_run(CoreLoop *cl);
void coreloop_stop(CoreLoop *cl);
