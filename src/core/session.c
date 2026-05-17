#include "session.h"
#include "ax25.h"
#include "ax25_route.h"
#include "config.h"
#include "tnc_layer.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern BOX42_Config g_cfg;

/* ------------------------------------------------------------
   Session Storage
------------------------------------------------------------ */
static Session sessions[MAX_SESSIONS];
static Session *mgr[128];

/* ------------------------------------------------------------
   Init
------------------------------------------------------------ */
static void session_init(void) {
    static int init = 0;
    if (init) return;
    init = 1;

    memset(sessions, 0, sizeof(sessions));
    memset(mgr, 0, sizeof(mgr));

    for (int i = 0; i < MAX_SESSIONS; i++) {
        sessions[i].fd = -1;
        sessions[i].ax25_bind = -1;
        sessions[i].ax25_echo = 0;
        sessions[i].active = 0;
        sessions[i].mode = SESSION_MODE_COMMAND;
        sessions[i].mail_buffer = NULL;
    }
}

/* ------------------------------------------------------------
   Core API
------------------------------------------------------------ */
Session *session_get(int fd) {
    session_init();
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (sessions[i].fd == fd)
            return &sessions[i];
    return NULL;
}

Session *session_create(int fd, const char *username, int level) {
    session_init();

    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == -1) {
            sessions[i].fd = fd;
            strncpy(sessions[i].username, username, sizeof(sessions[i].username)-1);
            sessions[i].username[sizeof(sessions[i].username)-1] = '\0';
            sessions[i].level = level;
            sessions[i].use_up42 = 0;
            sessions[i].ax25_bind = -1;
            sessions[i].ax25_echo = 0;
            sessions[i].active = 0;
            sessions[i].mode = SESSION_MODE_COMMAND;
            sessions[i].mail_buffer = NULL;
            return &sessions[i];
        }
    }
    return NULL;
}

void session_destroy(int fd) {
    session_init();
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].fd == fd) {

            if (sessions[i].mail_buffer) {
                free(sessions[i].mail_buffer);
                sessions[i].mail_buffer = NULL;
            }

            sessions[i].fd = -1;
            sessions[i].username[0] = 0;
            sessions[i].level = 0;
            sessions[i].use_up42 = 0;
            sessions[i].ax25_bind = -1;
            sessions[i].ax25_echo = 0;
            sessions[i].active = 0;
            sessions[i].mode = SESSION_MODE_COMMAND;
            return;
        }
    }
}

Session *session_get_by_index(int idx) {
    session_init();
    if (idx < 0 || idx >= MAX_SESSIONS) return NULL;
    if (sessions[idx].fd == -1) return NULL;
    return &sessions[idx];
}

/* ------------------------------------------------------------
   Manager
------------------------------------------------------------ */
void session_manager_add(Session *s) {
    for (int i = 0; i < 128; i++)
        if (!mgr[i]) { mgr[i] = s; return; }
}

void session_manager_remove(Session *s) {
    for (int i = 0; i < 128; i++)
        if (mgr[i] == s) { mgr[i] = NULL; return; }
}

void session_manager_tick(void) {
    for (int i = 0; i < 128; i++) {
        Session *s = mgr[i];
        if (!s) continue;
        session_router_tick();
    }
}

/* ------------------------------------------------------------
   Low-level send
------------------------------------------------------------ */
void session_send(Session *s, const uint8_t *buf, int len) {
    if (!s || s->fd < 0 || !buf || len <= 0) return;
    write(s->fd, buf, len);
}

void session_write(Session *s, const char *text) {
    if (!s || s->fd < 0 || !text) return;
    write(s->fd, text, strlen(text));
}

/* ------------------------------------------------------------
   Mode setter
------------------------------------------------------------ */
void session_set_mode(Session *s, int mode)
{
    if (!s) return;
    s->mode = mode;
}

/* ------------------------------------------------------------
   UP42 + Router (unchanged)
------------------------------------------------------------ */

#define UP42_MAGIC "UP42"
#define UP42_MAGIC_LEN 4

#define UP42_TYPE_AX25     0x01
#define UP42_TYPE_INFO     0x10
#define UP42_TYPE_PORTS    0x11
#define UP42_TYPE_ROUTE    0x12
#define UP42_TYPE_BIND     0x13
#define UP42_TYPE_UNBIND   0x14
#define UP42_TYPE_ECHO     0x15

/* … dein kompletter UP42‑Code bleibt unverändert … */

/* ------------------------------------------------------------
   Router stubs (required by session_manager_tick)
------------------------------------------------------------ */
void session_router_init(void) {
    /* no initialization needed yet */
}

void session_router_tick(void) {
    /* no periodic router logic yet */
}
