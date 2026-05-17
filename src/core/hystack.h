/* src/core/hystack.h */
#pragma once
#include <stdint.h>
#include "ax25.h"

#ifdef __AMIGA__
#include <proto/bsdsocket.h>
extern struct Library *SocketBase;
#endif

/* High-level AX.25 text sender (plattformunabhängig!) */
void hystack_ax25_send_text(const char *callsign, const char *text);

/* AX.25-Session */

/* AX.25-Session */
typedef struct {
    int used;
    AX25_Address src;
    AX25_Address dest;
    int port;
    int session_id;
} HYSTACK_SESSION;

#define HYSTACK_MAX_SESSIONS 32

typedef struct {
    int listen_v4;
    int listen_v6;
    int state;

    HYSTACK_SESSION sessions[HYSTACK_MAX_SESSIONS];
} HSTB42_CTX;

#define HSTB42_STOPPED 0
#define HSTB42_RUNNING 1

/* Init / Run / Stop */
int hystack_init(HSTB42_CTX *ctx, int port);
int hystack_run(HSTB42_CTX *ctx);
void hystack_stop(HSTB42_CTX *ctx);

/* AX.25 Routing */
void hystack_ax25_in(const AX25_Frame *f, int port);
void hystack_ax25_send(const AX25_Frame *f, int port);

/* Serial FD Setter */
void hystack_set_kiss_fd(int port, int fd);
void hystack_set_hdlc_fd(int port, int fd);

/* USB‑TNCX / Modem Auto‑Detect */
int hystack_serial_autodetect(const char *path);
