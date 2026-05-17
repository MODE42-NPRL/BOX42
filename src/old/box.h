#pragma once

#include "ax25.h"

#define BOX_MAX_MSG     256
#define BOX_MAX_AREA    16
#define BOX_MAX_MSGS    128

typedef enum {
    BOX_ROLE_USER = 0,
    BOX_ROLE_MOD,
    BOX_ROLE_ADMIN,
    BOX_ROLE_OWNER
} BOX_Role;

typedef struct {
    char from[16];
    char area[16];
    char text[BOX_MAX_MSG];
} BOX_Message;

typedef struct {
    BOX_Message msgs[BOX_MAX_MSGS];
    int count;
} BOX_Store;

void box_init();
void box_process_ax25(AX25_Frame *f);

// später: echte Authentifizierung
BOX_Role box_get_role(const char *callsign);
