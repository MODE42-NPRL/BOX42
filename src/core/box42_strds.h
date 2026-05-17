/* src/core/box42_strds.h */

#ifndef BOX42_STRDS_H
#define BOX42_STRDS_H

#include <stdint.h>
#include <string.h>

#include "ax25.h"
#include "box42_strds.h"
#include "session.h"
#include <sqlite3.h>

/* ------------------------------------------------------------
   AX.25 Driver callback types
------------------------------------------------------------ */
typedef void (*ax25_rx_callback_t)(const AX25_Frame *f, int port);
typedef void (*ax25_tx_raw_callback_t)(const uint8_t *buf, int len, int port);

/* ------------------------------------------------------------
   HSTB42 – Hybrid Stack BOX42
------------------------------------------------------------ */

typedef enum {
    HSTB42_LAYER_RF = 0,
    HSTB42_LAYER_SERIAL = 1,
    HSTB42_LAYER_HYBRID = 2,
    HSTB42_LAYER_TCPIP = 3,
    HSTB42_LAYER_BOX42 = 4
} HSTB42_LAYER;

typedef enum {
    HSTB42_DIR_RX = 0,
    HSTB42_DIR_TX = 1
} HSTB42_DIRECTION;

typedef struct {
    int active;
    HSTB42_LAYER from_layer;
    HSTB42_LAYER to_layer;
    HSTB42_DIRECTION direction;
} HSTB42_UNIT;

typedef struct {
    HSTB42_UNIT rx_path;
    HSTB42_UNIT tx_path;
} HSTB42_SYSTEM;

extern const HSTB42_SYSTEM BOX42_HSTB42;

/* ------------------------------------------------------------
   STRDS Limits
------------------------------------------------------------ */

#define BOX42_MAX_USERS         32
#define BOX42_MAX_CHANNELS      32
#define BOX42_MAX_NODES         32
#define BOX42_MAX_CALLSIGN_LEN  12
#define BOX42_MAX_NAME_LEN      24
#define BOX42_MAX_PASS_LEN      24
#define BOX42_MAX_MSG_LEN       512
#define BOX42_MAX_PATH_LEN      128

typedef enum {
    ROLE_OWNER   = 0,
    ROLE_ADMINS  = 1,
    ROLE_MODS    = 2,
    ROLE_USERS   = 4,
    ROLE_GUESTS  = 5
} BOX42_ROLE;

typedef enum {
    NODE_TERM       = 0,
    NODE_TELNET4    = 1,
    NODE_TELNET6    = 2,
    NODE_TELNETAX25 = 3
} BOX42_NODE_TYPE;

typedef struct {
    char name[BOX42_MAX_NAME_LEN];
    char callsign[BOX42_MAX_CALLSIGN_LEN];
    char password[BOX42_MAX_PASS_LEN];
    BOX42_ROLE role;
    int active;
} BOX42_USER;

typedef struct {
    BOX42_NODE_TYPE type;
    char path[BOX42_MAX_PATH_LEN];
    int active;
} BOX42_NODE;

typedef struct {
    char name[BOX42_MAX_NAME_LEN];
    int in_use;
} BOX42_CHANNEL;

typedef struct {
    int max_users;
    int max_channels;
    int max_nodes;
} BOX42_STRDS_LIMITS;

extern const BOX42_STRDS_LIMITS BOX42_LIMITS;

/* ------------------------------------------------------------
   STRDS Core
------------------------------------------------------------ */

void strds_init(void);
void strds_session_start(int fd);

/* ------------------------------------------------------------
   Terminal Environment Detection
------------------------------------------------------------ */

int box42_strds_is_tmux(void);
int box42_strds_is_screen(void);

/* ------------------------------------------------------------
   UNIVERSAL TELNET / TERMINAL INPUT NORMALIZER
------------------------------------------------------------ */

int strds_normalize_input(uint8_t *buf, int len, char *out, int outsz);

/* ------------------------------------------------------------
   AX.25 ↔ STRDS Session Bridge
------------------------------------------------------------ */

void strds_ax25_input(const char *callsign, const uint8_t *data, int len);
void strds_ax25_output(Session *s, const char *text);
Session *strds_ax25_get_or_create(const char *callsign);

/* ------------------------------------------------------------
   Low-level write helper
------------------------------------------------------------ */

void strds_write(int fd, const char *text);

/* ------------------------------------------------------------
   AX.25 Driver API (merged into STRDS)
------------------------------------------------------------ */

void ax25_drv_init(
    ax25_rx_callback_t rx_cb,
    ax25_tx_raw_callback_t kiss_tx_cb,
    ax25_tx_raw_callback_t hdlc_tx_cb
);

void ax25_drv_kiss_rx_byte(uint8_t b, int port);
void ax25_drv_hdlc_rx_byte(uint8_t b, int port);

void ax25_drv_kiss_send(const AX25_Frame *f, int port);
void ax25_drv_hdlc_send(const AX25_Frame *f, int port);

void box42_sqlite_init(const char *path);
sqlite3 *box42_db(void);

#endif /* BOX42_STRDS_H */
