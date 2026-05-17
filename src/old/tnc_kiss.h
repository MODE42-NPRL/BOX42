#pragma once

#define KISS_FEND  0xC0
#define KISS_FESC  0xDB
#define KISS_TFEND 0xDC
#define KISS_TFESC 0xDD

// Rückgabewert: Länge des AX.25‑Frames in out, 0 = ungültig
int kiss_decode(const unsigned char *in, int len,
                unsigned char *out, int max_out);

// Rückgabewert: Länge des KISS‑Frames in out, 0 = Fehler
int kiss_encode(const unsigned char *ax25, int len,
                unsigned char *out, int max_out, unsigned char port);
