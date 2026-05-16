#include "ax25.h"
#include <string.h>

int ax25_parse(const unsigned char *raw, int len, AX25_Frame *out) {
    if (len < 2) return 0;
    strcpy(out->src, "NOCALL");
    strcpy(out->dst, "NOCALL");
    strcpy(out->payload, (const char*)raw);
    return 1;
}

int ax25_build(const AX25_Frame *in, unsigned char *raw, int maxlen) {
    int l = strlen(in->payload);
    if (l > maxlen) l = maxlen;
    memcpy(raw, in->payload, l);
    return l;
}
