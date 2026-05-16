#include "up42.h"
#include <string.h>

int up42_parse(const uint8_t *buf, size_t len, UP42_Frame *out) {
    if (len < 4) return -1;

    out->version = buf[0];
    out->type    = buf[1];
    out->length  = (buf[2] << 8) | buf[3];

    if (out->version != UP42_VERSION) return -2;
    if (out->length > (len - 4)) return -3;

    out->payload = buf + 4;
    return 0;
}

size_t up42_build(uint8_t *buf, uint8_t type, const uint8_t *payload, uint16_t plen) {
    buf[0] = UP42_VERSION;
    buf[1] = type;
    buf[2] = (plen >> 8) & 0xFF;
    buf[3] = plen & 0xFF;

    memcpy(buf + 4, payload, plen);
    return plen + 4;
}
