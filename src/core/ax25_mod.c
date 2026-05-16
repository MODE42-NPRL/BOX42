#include <stdio.h>
#include "../../core/events.h"
#include "../../core/ax25.h"

static int counter = 0;

void box42_init() {
    printf("[AX25] init\n");
}

void box42_tick() {
    counter++;

    if (counter % 50 == 0) {
        AX25_Frame f;
        strcpy(f.src, "DB0BOX");
        strcpy(f.dst, "DB0MGH");
        strcpy(f.payload, "PING");

        event_emit(10, &f);
    }
}

void box42_stop() {
    printf("[AX25] stop\n");
}
