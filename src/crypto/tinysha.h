#ifndef TINY_SHA_H
#define TINY_SHA_H

#include <stddef.h>

void tinysha256(const unsigned char *data, size_t len, unsigned char out[32]);

#endif
