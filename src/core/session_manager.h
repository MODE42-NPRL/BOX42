#pragma once
#include "session.h"

void session_manager_add(Session *s);
void session_manager_remove(Session *s);
void session_manager_tick(void);

