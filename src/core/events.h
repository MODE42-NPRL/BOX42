#pragma once

#define MAX_EVENTS 64
#define MAX_HANDLERS_PER_EVENT 8

typedef void (*EventHandler)(void *data);

typedef struct {
    EventHandler handlers[MAX_HANDLERS_PER_EVENT];
    int count;
} EventSlot;

void event_register(int event_id, EventHandler handler);
void event_emit(int event_id, void *data);
