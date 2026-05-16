#pragma once
#include <stdint.h>

#define MODE_NORMAL   0
#define MODE_CHAT     1
#define MODE_PRIVATE  2

typedef struct Session {
    int id;
    int mode;
    int private_peer;
    int term_width;
    char username[64];
} Session;

Session *session_get(int id);
int session_find_by_username(const char *name);
void session_send_text(int sid, const char *text);
void session_close(Session *s);
