#include "tnc_kiss.h"

int kiss_decode(const unsigned char *in, int len,
                unsigned char *out, int max_out)
{
    if (len < 3) return 0;
    if (in[0] != KISS_FEND) return 0;

    int pos = 1; // nach FEND
    unsigned char cmd = in[pos++];
    (void)cmd; // Port/Command aktuell ignoriert

    int o = 0;
    int esc = 0;

    for (; pos < len; pos++) {
        unsigned char c = in[pos];

        if (c == KISS_FEND)
            break;

        if (esc) {
            if (c == KISS_TFEND)      c = KISS_FEND;
            else if (c == KISS_TFESC) c = KISS_FESC;
            esc = 0;
        } else if (c == KISS_FESC) {
            esc = 1;
            continue;
        }

        if (o >= max_out) return 0;
        out[o++] = c;
    }

    return o;
}

int kiss_encode(const unsigned char *ax25, int len,
                unsigned char *out, int max_out, unsigned char port)
{
    int o = 0;

    if (max_out < 3) return 0;

    out[o++] = KISS_FEND;
    out[o++] = (port & 0x0F); // Data frame on port

    for (int i = 0; i < len; i++) {
        unsigned char c = ax25[i];

        if (c == KISS_FEND) {
            if (o + 2 > max_out) return 0;
            out[o++] = KISS_FESC;
            out[o++] = KISS_TFEND;
        } else if (c == KISS_FESC) {
            if (o + 2 > max_out) return 0;
            out[o++] = KISS_FESC;
            out[o++] = KISS_TFESC;
        } else {
            if (o + 1 > max_out) return 0;
            out[o++] = c;
        }
    }

    if (o + 1 > max_out) return 0;
    out[o++] = KISS_FEND;

    return o;
}
