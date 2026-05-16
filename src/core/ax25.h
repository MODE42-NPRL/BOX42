#pragma once

typedef struct {
    char src[10];
    char dst[10];
    char payload[256];
} AX25_Frame;

int ax25_parse(const unsigned char *raw, int len, AX25_Frame *out);
int ax25_build(const AX25_Frame *in, unsigned char *raw, int maxlen);
