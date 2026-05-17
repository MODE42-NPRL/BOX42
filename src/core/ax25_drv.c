/* src/core/ax25_drv.c */

#include <stdint.h>
#include <string.h>

#include "ax25.h"
#include "box42_strds.h"

/* ------------------------------------------------------------
   Callback storage
------------------------------------------------------------ */

static ax25_rx_callback_t      g_rx_cb      = 0;
static ax25_tx_raw_callback_t  g_kiss_tx_cb = 0;
static ax25_tx_raw_callback_t  g_hdlc_tx_cb = 0;

void ax25_drv_init(ax25_rx_callback_t rx_cb,
                   ax25_tx_raw_callback_t kiss_tx_cb,
                   ax25_tx_raw_callback_t hdlc_tx_cb)
{
    g_rx_cb      = rx_cb;
    g_kiss_tx_cb = kiss_tx_cb;
    g_hdlc_tx_cb = hdlc_tx_cb;
}

/* ------------------------------------------------------------
   KISS RX (TNC2C)
------------------------------------------------------------ */

#define KISS_FEND  0xC0
#define KISS_FESC  0xDB
#define KISS_TFEND 0xDC
#define KISS_TFESC 0xDD

static uint8_t kiss_buf[2048];
static int     kiss_len = 0;
static int     kiss_esc = 0;

void ax25_drv_kiss_rx_byte(uint8_t b, int port)
{
    if (b == KISS_FEND) {
        if (kiss_len > 1) {
            int kiss_port = kiss_buf[0] & 0x0F;
            const uint8_t *frame = kiss_buf + 1;
            int frame_len = kiss_len - 1;

            AX25_Frame f;
            if (ax25_parse(frame, frame_len, &f) == 0) {
                f.port = kiss_port;
                if (g_rx_cb)
                    g_rx_cb(&f, kiss_port);
            }
        }
        kiss_len = 0;
        kiss_esc = 0;
        return;
    }

    if (b == KISS_FESC) {
        kiss_esc = 1;
        return;
    }

    if (kiss_esc) {
        if (b == KISS_TFEND) b = KISS_FEND;
        else if (b == KISS_TFESC) b = KISS_FESC;
        kiss_esc = 0;
    }

    if (kiss_len < (int)sizeof(kiss_buf))
        kiss_buf[kiss_len++] = b;
}

/* ------------------------------------------------------------
   KISS TX
------------------------------------------------------------ */

void ax25_drv_kiss_send(const AX25_Frame *f, int port)
{
    if (!g_kiss_tx_cb || !f) return;

    uint8_t frame[2048];
    int flen = ax25_build(f, frame, sizeof(frame));
    if (flen <= 0) return;

    uint8_t out[4096];
    int p = 0;

    out[p++] = KISS_FEND;
    out[p++] = (uint8_t)(port & 0x0F);

    for (int i = 0; i < flen; i++) {
        uint8_t b = frame[i];
        if (b == KISS_FEND) {
            out[p++] = KISS_FESC;
            out[p++] = KISS_TFEND;
        } else if (b == KISS_FESC) {
            out[p++] = KISS_FESC;
            out[p++] = KISS_TFESC;
        } else {
            out[p++] = b;
        }
    }

    out[p++] = KISS_FEND;

    g_kiss_tx_cb(out, p, port);
}

/* ------------------------------------------------------------
   HDLC RX (BayCom/PC-COM)
------------------------------------------------------------ */

#define HDLC_FLAG 0x7E

static uint8_t hdlc_buf[2048];
static int     hdlc_len = 0;
static int     hdlc_in_frame = 0;

void ax25_drv_hdlc_rx_byte(uint8_t b, int port)
{
    if (b == HDLC_FLAG) {
        if (hdlc_in_frame && hdlc_len > 0) {
            AX25_Frame f;
            if (ax25_parse(hdlc_buf, hdlc_len, &f) == 0) {
                f.port = port;
                if (g_rx_cb)
                    g_rx_cb(&f, port);
            }
        }
        hdlc_len = 0;
        hdlc_in_frame = 1;
        return;
    }

    if (!hdlc_in_frame) return;

    if (hdlc_len < (int)sizeof(hdlc_buf))
        hdlc_buf[hdlc_len++] = b;
}

/* ------------------------------------------------------------
   HDLC TX
------------------------------------------------------------ */

void ax25_drv_hdlc_send(const AX25_Frame *f, int port)
{
    if (!g_hdlc_tx_cb || !f) return;

    uint8_t frame[2048];
    int flen = ax25_build(f, frame, sizeof(frame));
    if (flen <= 0) return;

    uint8_t out[4096];
    int p = 0;

    out[p++] = HDLC_FLAG;
    memcpy(out + p, frame, flen);
    p += flen;
    out[p++] = HDLC_FLAG;

    g_hdlc_tx_cb(out, p, port);
}
