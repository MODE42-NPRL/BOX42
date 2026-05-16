#include "events.h"

#define MAX_EVENTS    32
#define MAX_HANDLERS  4

typedef struct {
    EventHandler handlers[MAX_HANDLERS];
    int count;
} EventSlot;

static EventSlot slots[MAX_EVENTS];

void events_init(void) {
    for (int i = 0; i < MAX_EVENTS; i++) {
        slots[i].count = 0;
        for (int j = 0; j < MAX_HANDLERS; j++)
            slots[i].handlers[j] = 0;
    }
}

int event_register(int id, EventHandler h) {
    if (id < 0 || id >= MAX_EVENTS) 
        return 0;
    if (!h) 
        return 0;

    EventSlot *s = &slots[id];

    if (s->count >= MAX_HANDLERS)
        return 0;

    s->handlers[s->count++] = h;
    return 1;
}

int event_emit(int id, void *data) {
    if (id < 0 || id >= MAX_EVENTS)
        return 0;

    EventSlot *s = &slots[id];
    int called = 0;

    for (int i = 0; i < s->count; i++) {
        EventHandler h = s->handlers[i];
        if (h) {
            h(data);
            called++;
        }
    }

    return called;
}
