#include "session_router.h"
#include "session.h"
#include "ax25.h"
#include "ax25_route.h"
#include "tnc_layer.h"
#include "config.h"

#include <string.h>
#include <stdio.h>

#define UP42_MAGIC "UP42"
#define UP42_MAGIC_LEN 4

#define UP42_TYPE_AX25     0x01
#define UP42_TYPE_INFO     0x10
#define UP42_TYPE_PORTS    0x11
#define UP42_TYPE_ROUTE    0x12
#define UP42_TYPE_BIND     0x13
#define UP42_TYPE_UNBIND   0x14
#define UP42_TYPE_ECHO     0x15

extern BOX42_Config g_cfg;

void session_router_init(void) {
}

void session_router_tick(void) {
}

static int up42_valid(const uint8_t *buf, int len) {
    if (len < UP42_MAGIC_LEN + 1)
        return 0;
    if (memcmp(buf, UP42_MAGIC, UP42_MAGIC_LEN) != 0)
        return 0;
    return 1;
}

static void up42_send(struct Session *s, uint8_t type, const uint8_t *data, int len) {
    uint8_t out[2048];
    int p = 0;

    memcpy(&out[p], UP42_MAGIC, UP42_MAGIC_LEN);
    p += UP42_MAGIC_LEN;

    out[p++] = type;

    memcpy(&out[p], data, len);
    p += len;

    session_send(s, out, p);
}

static void cmd_info(struct Session *s) {
    char buf[256];
    int n = snprintf(buf, sizeof(buf),
        "BOX42 Node\n"
        "Version: %s\n"
        "AX25-Light Router\n",
        g_cfg.version
    );
    up42_send(s, UP42_TYPE_INFO, (uint8_t*)buf, n);
}

static void cmd_ports(struct Session *s) {
    char buf[256];
    int n = snprintf(buf, sizeof(buf),
        "Ports:\n"
        "0: %s\n"
        "1: %s\n"
        "2: %s\n"
        "3: %s\n",
        g_cfg.tnc[0].enabled ? g_cfg.tnc[0].device : "off",
        g_cfg.tnc[1].enabled ? g_cfg.tnc[1].device : "off",
        g_cfg.tnc[2].enabled ? g_cfg.tnc[2].device : "off",
        g_cfg.tnc[3].enabled ? g_cfg.tnc[3].device : "off"
    );
    up42_send(s, UP42_TYPE_PORTS, (uint8_t*)buf, n);
}

static void cmd_route(struct Session *s) {
    char buf[512];
    int p = 0;

    p += snprintf(&buf[p], sizeof(buf)-p, "Routing:\n");

    for (int i = 0; i < 4; i++) {
        p += snprintf(&buf[p], sizeof(buf)-p, "Port %d: ", i);
        for (int j = 0; j < 4; j++) {
            p += snprintf(&buf[p], sizeof(buf)-p, "%d ", g_cfg.digi.cross[i][j]);
        }
        p += snprintf(&buf[p], sizeof(buf)-p, "\n");
    }

    up42_send(s, UP42_TYPE_ROUTE, (uint8_t*)buf, p);
}

static void cmd_bind(struct Session *s, int port) {
    if (port < 0 || port > 3) return;
    s->ax25_bind = port;

    char msg[64];
    int n = snprintf(msg, sizeof(msg), "BOUND %d\n", port);
    up42_send(s, UP42_TYPE_BIND, (uint8_t*)msg, n);
}

static void cmd_unbind(struct Session *s) {
    s->ax25_bind = -1;

    char msg[32];
    int n = snprintf(msg, sizeof(msg), "UNBOUND\n");
    up42_send(s, UP42_TYPE_UNBIND, (uint8_t*)msg, n);
}

static void cmd_echo(struct Session *s, int mode) {
    s->ax25_echo = mode;

    char msg[32];
    int n = snprintf(msg, sizeof(msg), "ECHO %s\n", mode ? "ON" : "OFF");
    up42_send(s, UP42_TYPE_ECHO, (uint8_t*)msg, n);
}

static void handle_up42_ax25_out(struct Session *s, const uint8_t *buf, int len) {
    const uint8_t *p = buf + UP42_MAGIC_LEN + 1;
    int plen = len - (UP42_MAGIC_LEN + 1);

    AX25_Frame f;
    if (!ax25_parse(p, plen, &f))
        return;

    int out = ax25_route(&f);
    if (out < 0)
        return;

    uint8_t axbuf[2048];
    int axlen = ax25_build(&f, axbuf, sizeof(axbuf));
    if (axlen <= 0)
        return;

    tnc_layer_send(out, axbuf, axlen);

    if (s->ax25_echo)
        up42_send(s, UP42_TYPE_AX25, axbuf, axlen);
}

void session_router_handle(struct Session *s, const uint8_t *buf, int len) {
    if (!s || !buf || len <= 0)
        return;

    if (!up42_valid(buf, len)) {
        session_send(s, buf, len);
        return;
    }

    uint8_t type = buf[UP42_MAGIC_LEN];

    switch (type) {
        case UP42_TYPE_AX25:
            handle_up42_ax25_out(s, buf, len);
            break;

        case UP42_TYPE_INFO:
            cmd_info(s);
            break;

        case UP42_TYPE_PORTS:
            cmd_ports(s);
            break;

        case UP42_TYPE_ROUTE:
            cmd_route(s);
            break;

        case UP42_TYPE_BIND:
            if (len >= UP42_MAGIC_LEN + 2)
                cmd_bind(s, buf[UP42_MAGIC_LEN+1]);
            break;

        case UP42_TYPE_UNBIND:
            cmd_unbind(s);
            break;

        case UP42_TYPE_ECHO:
            if (len >= UP42_MAGIC_LEN + 2)
                cmd_echo(s, buf[UP42_MAGIC_LEN+1]);
            break;

        default:
            break;
    }
}

void session_router_ax25_in(const AX25_Frame *f, const uint8_t *raw, int len) {
    struct Session *s = session_first();
    while (s) {
        if (s->ax25_bind < 0 || s->ax25_bind == f->port)
            up42_send(s, UP42_TYPE_AX25, raw, len);
        s = session_next(s);
    }
}
