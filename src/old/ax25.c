#include "ax25.h"
#include <string.h>

static int safe_str_copy(char *dst, int dstsz, const char *src, int srclen) {
    if (dstsz <= 0 || srclen < 0) return 0;
    if (srclen >= dstsz) srclen = dstsz - 1;
    memcpy(dst, src, srclen);
    dst[srclen] = '\0';
    return srclen;
}

int ax25_parse(const uint8_t *buf, int len, AX25_Frame *out) {
    if (!buf || !out || len < 2) return 0;

    int pos = 0;
    int src_len = buf[pos++];
    int dst_len = buf[pos++];

    if (src_len <= 0 || src_len >= AX25_MAX_CALL) return 0;
    if (dst_len <= 0 || dst_len >= AX25_MAX_CALL) return 0;

    if (pos + src_len + dst_len > len) return 0;

    memset(out, 0, sizeof(*out));
    safe_str_copy(out->src, AX25_MAX_CALL, (const char *)&buf[pos], src_len);
    pos += src_len;
    safe_str_copy(out->dst, AX25_MAX_CALL, (const char *)&buf[pos], dst_len);
    pos += dst_len;

    int payload_len = len - pos;
    if (payload_len < 0) return 0;
    if (payload_len > AX25_MAX_PAYLOAD) payload_len = AX25_MAX_PAYLOAD;

    if (payload_len > 0)
        memcpy(out->payload, &buf[pos], payload_len);

    out->payload_len = payload_len;
    out->port = -1;
    return 1;
}

int ax25_build(const AX25_Frame *f, uint8_t *buf, int maxlen) {
    if (!f || !buf || maxlen < 2) return -1;

    int src_len = (int)strnlen(f->src, AX25_MAX_CALL);
    int dst_len = (int)strnlen(f->dst, AX25_MAX_CALL);
    if (src_len <= 0 || dst_len <= 0) return -1;

    if (f->payload_len < 0 || f->payload_len > AX25_MAX_PAYLOAD)
        return -1;

    int need = 2 + src_len + dst_len + f->payload_len;
    if (need > maxlen) return -1;

    int pos = 0;
    buf[pos++] = (uint8_t)src_len;
    buf[pos++] = (uint8_t)dst_len;

    memcpy(&buf[pos], f->src, src_len);
    pos += src_len;
    memcpy(&buf[pos], f->dst, dst_len);
    pos += dst_len;

    if (f->payload_len > 0) {
        memcpy(&buf[pos], f->payload, f->payload_len);
        pos += f->payload_len;
    }

    return pos;
}
