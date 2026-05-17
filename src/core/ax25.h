#pragma once
#include <stdint.h>

#define AX25_MAX_CALL     16
typedef char AX25_Address[AX25_MAX_CALL];
#define AX25_MAX_PAYLOAD 1024

/* AX.25 Frame (simplified, protocol-only) */
typedef struct {
    char src[AX25_MAX_CALL];   /* "CALL-SSID" */
    char dst[AX25_MAX_CALL];   /* "CALL-SSID" */

    uint8_t payload[AX25_MAX_PAYLOAD];
    int payload_len;

    int port;                  /* optional: KISS port */
} AX25_Frame;

/* Parse raw AX.25 frame (no KISS, no HDLC flags) */
int ax25_parse(const uint8_t *buf, int len, AX25_Frame *out);

/* Build raw AX.25 frame (no KISS, no HDLC flags) */
int ax25_build(const AX25_Frame *f, uint8_t *buf, int maxlen);

/* CRC16-CCITT (AX.25 FCS) */
uint16_t ax25_crc(const uint8_t *data, int len);
