#pragma once

typedef void (*EventHandler)(void *data);

void events_init(void);
int  event_register(int id, EventHandler h);
int  event_emit(int id, void *data);
