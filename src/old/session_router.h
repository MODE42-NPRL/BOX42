#pragma once
#include <stdint.h>

#define UP42_TYPE_AX25     0x01
#define UP42_TYPE_INFO     0x10
#define UP42_TYPE_PORTS    0x11
#define UP42_TYPE_ROUTE    0x12
#define UP42_TYPE_BIND     0x13
#define UP42_TYPE_UNBIND   0x14
#define UP42_TYPE_ECHO     0x15

struct Session;
struct AX25_Frame;

void session_router_init(void);
void session_router_handle(struct Session *s, const uint8_t *buf, int len);
void session_router_tick(void);
void session_router_ax25_in(const struct AX25_Frame *f, const uint8_t *raw, int len);
