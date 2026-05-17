/* src/core/hystack.c */

#include "hystack.h"
#include "box42_strds.h"
#include "ax25.h"
#include "ax25_drv.h"
#include "box42_strds.h"

#include <string.h>

#ifdef __AMIGA__
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/bsdsocket.h>
struct Library *SocketBase = NULL;
#define close CloseSocket
#define read  recv
#define write send
#else
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <termios.h>
#endif

/* globaler HYSTACK-Kontext für Routing */
HSTB42_CTX *g_hystack_ctx = NULL;

/* ------------------------------------------------------------
   Serial Ports (KISS + HDLC)
------------------------------------------------------------ */
static int serial_fd_kiss[8];
static int serial_fd_hdlc[8];

/* ------------------------------------------------------------
   USB‑TNCX / Modem Serial Open (RAW, non-blocking)
------------------------------------------------------------ */
static int hystack_serial_open_raw(const char *path, int baud)
{
#ifndef __AMIGA__
    int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) return -1;

    struct termios t;
    memset(&t, 0, sizeof(t));
    cfmakeraw(&t);

    speed_t sp = B9600;
    switch (baud) {
        case 1200:   sp = B1200;   break;
        case 2400:   sp = B2400;   break;
        case 4800:   sp = B4800;   break;
        case 9600:   sp = B9600;   break;
        case 19200:  sp = B19200;  break;
        case 38400:  sp = B38400;  break;
        case 57600:  sp = B57600;  break;
        case 115200: sp = B115200; break;
    }

    cfsetispeed(&t, sp);
    cfsetospeed(&t, sp);
    t.c_cflag |= (CLOCAL | CREAD);

    if (tcsetattr(fd, TCSANOW, &t) < 0) {
        close(fd);
        return -1;
    }

    return fd;
#else
    BPTR fh = Open(path, MODE_OLDFILE);
    if (!fh) return -1;
    return fh;
#endif
}

int hystack_serial_autodetect(const char *path)
{
    int fd = hystack_serial_open_raw(path, 9600);
    if (fd >= 0) return fd;

    fd = hystack_serial_open_raw(path, 1200);
    if (fd >= 0) return fd;

    return -1;
}

/* ------------------------------------------------------------
   Serial FD Setter
------------------------------------------------------------ */
void hystack_set_kiss_fd(int port, int fd)
{
    if (port >= 0 && port < 8)
        serial_fd_kiss[port] = fd;
}

void hystack_set_hdlc_fd(int port, int fd)
{
    if (port >= 0 && port < 8)
        serial_fd_hdlc[port] = fd;
}

/* ------------------------------------------------------------
   Serial TX
------------------------------------------------------------ */
void hystack_serial_tx_kiss(const uint8_t *buf, int len, int port)
{
    int fd = serial_fd_kiss[port];
    if (fd > 0)
        write(fd, buf, len);
}

void hystack_serial_tx_hdlc(const uint8_t *buf, int len, int port)
{
    int fd = serial_fd_hdlc[port];
    if (fd > 0)
        write(fd, buf, len);
}

/* ------------------------------------------------------------
   Serial RX Poller
------------------------------------------------------------ */
static void hystack_serial_poll(int fd, int port, int is_kiss)
{
    uint8_t b;
    int r = read(fd, &b, 1);
    if (r == 1) {
        if (is_kiss)
            ax25_drv_kiss_rx_byte(b, port);
        else
            ax25_drv_hdlc_rx_byte(b, port);
    }
}

/* ------------------------------------------------------------
   Session Management
------------------------------------------------------------ */
static HYSTACK_SESSION *hystack_find_session(HSTB42_CTX *ctx,
                                             const AX25_Address *src,
                                             const AX25_Address *dest,
                                             int port)
{
    for (int i = 0; i < HYSTACK_MAX_SESSIONS; i++) {
        HYSTACK_SESSION *s = &ctx->sessions[i];
        if (!s->used) continue;

        if (s->port == port &&
            memcmp(&s->src,  src,  sizeof(AX25_Address)) == 0 &&
            memcmp(&s->dest, dest, sizeof(AX25_Address)) == 0)
        {
            return s;
        }
    }
    return NULL;
}

static HYSTACK_SESSION *hystack_create_session(HSTB42_CTX *ctx,
                                               const AX25_Address *src,
                                               const AX25_Address *dest,
                                               int port)
{
    for (int i = 0; i < HYSTACK_MAX_SESSIONS; i++) {
        HYSTACK_SESSION *s = &ctx->sessions[i];
        if (!s->used) {
            s->used = 1;
            memcpy(&s->src,  src,  sizeof(AX25_Address));
            memcpy(&s->dest, dest, sizeof(AX25_Address));
            s->port = port;
            s->session_id = i;
            return s;
        }
    }
    return NULL;
}

/* ------------------------------------------------------------
   AX.25 → HYSTACK
------------------------------------------------------------ */
void hystack_ax25_in(const AX25_Frame *f, int port)
{
    HSTB42_CTX *ctx = g_hystack_ctx;
    if (!ctx || !f)
        return;

    HYSTACK_SESSION *s =
        hystack_find_session(ctx, &f->src, &f->dst, port);

    if (!s) {
        s = hystack_create_session(ctx, &f->src, &f->dst, port);
        if (!s)
            return;
    }

    /* Hier später: STRDS/BOX42-Session anbinden */
    (void)s;
}

/* ------------------------------------------------------------
   HYSTACK → AX.25
------------------------------------------------------------ */
void hystack_ax25_send(const AX25_Frame *f, int port)
{
    if (port == 0)
        ax25_drv_kiss_send(f, port);
    else
        ax25_drv_hdlc_send(f, port);
}

/* ------------------------------------------------------------
   TCP Listener (IPv4)
------------------------------------------------------------ */
static int hystack_create_v4(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family      = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port        = htons(port);

    if (bind(fd, (struct sockaddr *)&a, sizeof(a)) < 0) { close(fd); return -1; }
    if (listen(fd, 16) < 0) { close(fd); return -1; }

    return fd;
}

/* ------------------------------------------------------------
   TCP Listener (IPv6)
------------------------------------------------------------ */
static int hystack_create_v6(int port)
{
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    a.sin6_addr   = in6addr_any;
    a.sin6_port   = htons(port);

    if (bind(fd, (struct sockaddr *)&a, sizeof(a)) < 0) { close(fd); return -1; }
    if (listen(fd, 16) < 0) { close(fd); return -1; }

    return fd;
}

/* ------------------------------------------------------------
   Accept → STRDS
------------------------------------------------------------ */
static void hystack_accept(int fd)
{
    int cfd = accept(fd, NULL, NULL);
    if (cfd >= 0)
        strds_session_start(cfd);
}

/* ------------------------------------------------------------
   Init
------------------------------------------------------------ */
int hystack_init(HSTB42_CTX *ctx, int port)
{
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));
    g_hystack_ctx = ctx;

#ifdef __AMIGA__
    SocketBase = OpenLibrary("bsdsocket.library", 4);
    if (!SocketBase)
        return -1;
#endif

    ax25_drv_init(
        hystack_ax25_in,
        hystack_serial_tx_kiss,
        hystack_serial_tx_hdlc
    );

    ctx->listen_v4 = hystack_create_v4(port);
    ctx->listen_v6 = hystack_create_v6(port);

    if (ctx->listen_v4 < 0 && ctx->listen_v6 < 0)
        return -1;

    ctx->state = HSTB42_STOPPED;
    return 0;
}

/* ------------------------------------------------------------
   Main Loop
------------------------------------------------------------ */
int hystack_run(HSTB42_CTX *ctx)
{
    if (!ctx) return -1;
    ctx->state = HSTB42_RUNNING;

    fd_set fds;

    while (ctx->state == HSTB42_RUNNING) {
        FD_ZERO(&fds);

        int maxfd = 0;

        if (ctx->listen_v4 >= 0) {
            FD_SET(ctx->listen_v4, &fds);
            if (ctx->listen_v4 > maxfd) maxfd = ctx->listen_v4;
        }
        if (ctx->listen_v6 >= 0) {
            FD_SET(ctx->listen_v6, &fds);
            if (ctx->listen_v6 > maxfd) maxfd = ctx->listen_v6;
        }

        for (int p = 0; p < 8; p++) {
            if (serial_fd_kiss[p] > 0) {
                FD_SET(serial_fd_kiss[p], &fds);
                if (serial_fd_kiss[p] > maxfd) maxfd = serial_fd_kiss[p];
            }
            if (serial_fd_hdlc[p] > 0) {
                FD_SET(serial_fd_hdlc[p], &fds);
                if (serial_fd_hdlc[p] > maxfd) maxfd = serial_fd_hdlc[p];
            }
        }

        int r = select(maxfd + 1, &fds, NULL, NULL, NULL);
        if (r < 0)
            continue;

        if (ctx->listen_v4 >= 0 && FD_ISSET(ctx->listen_v4, &fds))
            hystack_accept(ctx->listen_v4);

        if (ctx->listen_v6 >= 0 && FD_ISSET(ctx->listen_v6, &fds))
            hystack_accept(ctx->listen_v6);

        for (int p = 0; p < 8; p++) {
            if (serial_fd_kiss[p] > 0 && FD_ISSET(serial_fd_kiss[p], &fds))
                hystack_serial_poll(serial_fd_kiss[p], p, 1);

            if (serial_fd_hdlc[p] > 0 && FD_ISSET(serial_fd_hdlc[p], &fds))
                hystack_serial_poll(serial_fd_hdlc[p], p, 0);
        }
    }

    return 0;
}

/* ------------------------------------------------------------
   Stop
------------------------------------------------------------ */
void hystack_stop(HSTB42_CTX *ctx)
{
    if (!ctx) return;

    if (ctx->listen_v4 >= 0) close(ctx->listen_v4);
    if (ctx->listen_v6 >= 0) close(ctx->listen_v6);

#ifdef __AMIGA__
    if (SocketBase)
        CloseLibrary(SocketBase);
    SocketBase = NULL;
#endif

    ctx->state = HSTB42_STOPPED;
}

void hystack_ax25_send_text(const char *callsign, const char *text)
{
    if (!callsign || !text) return;

    AX25_Frame f;
    memset(&f, 0, sizeof(f));

    /* Source Callsign (anpassen, falls du ein anderes Default willst) */
    strncpy(f.src, "BOX42", AX25_MAX_CALL - 1);

    /* Zielrufzeichen */
    strncpy(f.dst, callsign, AX25_MAX_CALL - 1);

    /* Payload */
    strncpy((char *)f.payload, text, AX25_MAX_PAYLOAD - 1);
    f.payload_len = strlen((char *)f.payload);

    /* Senden über Port 0 (oder deinen Standardport) */
    hystack_ax25_send(&f, 0);
}
