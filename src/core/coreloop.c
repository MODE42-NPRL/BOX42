#include "coreloop.h"
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>

/*
 * Minimal core loop for the modern BOX42 build.
 * All legacy subsystems (events, digipeater, tnc_mod, monitor, sniffer,
 * port_status, textstore, listener, config) are intentionally removed.
 */

static atomic_int running = 0;

void coreloop_init(void)
{
    printf("[CORE] init\n");
    running = 1;
}

void coreloop_run(void)
{
    printf("[CORE] run loop (minimal)\n");

    while (running) {
        /* Placeholder for future polling */
        usleep(100000); /* 100 ms */
        break;          /* prevent infinite loop in minimal build */
    }
}

void coreloop_stop(void)
{
    printf("[CORE] stop\n");
    running = 0;
}
