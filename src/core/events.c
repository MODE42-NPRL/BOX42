#include "events.h"
#include <stdio.h>

static EventSlot event_slots[MAX_EVENTS];

void event_register(int event_id, EventHandler handler) {
    if (event_id < 0 || event_id >= MAX_EVENTS) return;

    EventSlot *slot = &event_slots[event_id];

    if (slot->count >= MAX_HANDLERS_PER_EVENT) {
        printf("[BOX42] Event %d handler overflow\n", event_id);
        return;
    }

    slot->handlers[slot->count++] = handler;
}

void event_emit(int event_id, void *data) {
    if (event_id < 0 || event_id >= MAX_EVENTS) return;

    EventSlot *slot = &event_slots[event_id];

    for (int i = 0; i < slot->count; i++) {
        slot->handlers[i](data);
    }
}
