#pragma once

#include <stdint.h>

#define AX25_MAX_CALL     16
#define AX25_MAX_PAYLOAD  256

typedef struct {
    char    src[AX25_MAX_CALL];
    char    dst[AX25_MAX_CALL];
    uint8_t payload[AX25_MAX_PAYLOAD];
    int     payload_len;   // 0..AX25_MAX_PAYLOAD
    int     port;          // 0..3, -1 = unknown
} AX25_Frame;

// Minimal, selbstkonsistentes Format (nicht voll AX.25-konform):
// [1] src_len [1] dst_len [src] [dst] [payload...]
int ax25_parse(const uint8_t *buf, int len, AX25_Frame *out);
int ax25_build(const AX25_Frame *f, uint8_t *buf, int maxlen);
