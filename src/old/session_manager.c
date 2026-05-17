#include "session_manager.h"
#include "session_router.h"

static Session *g_sessions[128];

void session_manager_add(Session *s) {
    for (int i = 0; i < 128; i++) {
        if (!g_sessions[i]) {
            g_sessions[i] = s;
            return;
        }
    }
}

void session_manager_remove(Session *s) {
    for (int i = 0; i < 128; i++) {
        if (g_sessions[i] == s) {
            g_sessions[i] = NULL;
            return;
        }
    }
}

void session_manager_tick(void) {
    for (int i = 0; i < 128; i++) {
        Session *s = g_sessions[i];
        if (!s) continue;
        session_router(s);
    }
}
