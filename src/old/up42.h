#ifndef UP42_H
#define UP42_H

#include <stdint.h>
#include <stddef.h>

#define UP42_VERSION 0x01

#define UP42_TYPE_COMMAND   0x01
#define UP42_TYPE_TEXT      0x02
#define UP42_TYPE_AX25      0x03
#define UP42_TYPE_CONTROL   0x04
#define UP42_TYPE_ERROR     0x05

typedef struct {
    uint8_t  version;
    uint8_t  type;
    uint16_t length;
    const uint8_t *payload;
} UP42_Frame;

int up42_parse(const uint8_t *buf, size_t len, UP42_Frame *out);
size_t up42_build(uint8_t *buf, uint8_t type, const uint8_t *payload, uint16_t plen);

#endif
