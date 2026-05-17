#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ax25.h"

/* ------------------------------------------------------------
   CRC16-CCITT (AX.25 FCS)
------------------------------------------------------------ */
uint16_t ax25_crc(const uint8_t *data, int len)
{
    uint16_t crc = 0xFFFF;

    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408; /* reversed 0x1021 */
            else
                crc >>= 1;
        }
    }
    return ~crc;
}

/* ------------------------------------------------------------
   Helper: parse "CALL-SSID"
------------------------------------------------------------ */
static void parse_call(const uint8_t *in, char *out)
{
    char call[7];
    for (int i = 0; i < 6; i++)
        call[i] = (char)(in[i] >> 1);
    call[6] = 0;

    int ssid = (in[6] >> 1) & 0x0F;

    snprintf(out, AX25_MAX_CALL, "%s-%d", call, ssid);
}

/* ------------------------------------------------------------
   Helper: encode "CALL-SSID"
------------------------------------------------------------ */
static void encode_call(const char *in, uint8_t *out, int last)
{
    char call[7] = {0};
    int ssid = 0;

    /* split CALL-SSID */
    const char *dash = strchr(in, '-');
    if (dash) {
        int len = dash - in;
        if (len > 6) len = 6;
        memcpy(call, in, len);
        ssid = atoi(dash + 1);
    } else {
        strncpy(call, in, 6);
    }

    /* pad callsign */
    for (int i = 0; i < 6; i++) {
        char c = (i < (int)strlen(call)) ? call[i] : ' ';
        out[i] = ((uint8_t)c) << 1;
    }

    out[6] = ((ssid & 0x0F) << 1) | 0x60; /* reserved bits */

    if (last)
        out[6] |= 0x01; /* H-bit */
}

/* ------------------------------------------------------------
   AX.25 Parse (protocol-only)
------------------------------------------------------------ */
int ax25_parse(const uint8_t *buf, int len, AX25_Frame *out)
{
    if (!buf || !out || len < 17)
        return -1;

    /* FCS check */
    uint16_t rx_fcs = buf[len-2] | (buf[len-1] << 8);
    uint16_t calc = ax25_crc(buf, len - 2);

    if (rx_fcs != calc)
        return -2;

    int p = 0;

    /* Destination */
    parse_call(buf + p, out->dst);
    p += 7;

    /* Source */
    parse_call(buf + p, out->src);
    p += 7;

    /* Skip digipeaters (if any) */
    while (!(buf[p-1] & 0x01)) {
        p += 7;
        if (p + 7 >= len) return -3;
    }

    /* Control */
    uint8_t control = buf[p++];

    /* PID */
    uint8_t pid = buf[p++];

    /* Payload */
    int info_len = len - 2 - p;
    if (info_len < 0 || info_len > AX25_MAX_PAYLOAD)
        return -4;

    memcpy(out->payload, buf + p, info_len);
    out->payload_len = info_len;

    return 0;
}

/* ------------------------------------------------------------
   AX.25 Build (protocol-only)
------------------------------------------------------------ */
int ax25_build(const AX25_Frame *f, uint8_t *buf, int maxlen)
{
    if (!f || !buf) return -1;

    int p = 0;

    /* Destination */
    encode_call(f->dst, buf + p, 0);
    p += 7;

    /* Source */
    encode_call(f->src, buf + p, 1); /* last address */
    p += 7;

    /* Control: UI frame */
    buf[p++] = 0x03;

    /* PID: no layer 3 */
    buf[p++] = 0xF0;

    /* Payload */
    if (f->payload_len > AX25_MAX_PAYLOAD)
        return -2;

    memcpy(buf + p, f->payload, f->payload_len);
    p += f->payload_len;

    /* FCS */
    uint16_t fcs = ax25_crc(buf, p);
    if (p + 2 > maxlen)
        return -3;

    buf[p++] = fcs & 0xFF;
    buf[p++] = (fcs >> 8) & 0xFF;

    return p;
}
