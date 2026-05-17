#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_DIGEST_LENGTH 32
#define SHA256_HEX_LENGTH (SHA256_DIGEST_LENGTH * 2 + 1)

void sha256(const uint8_t *data, size_t len, uint8_t out[SHA256_DIGEST_LENGTH]);
void sha256_hex(const uint8_t *data, size_t len, char out[SHA256_HEX_LENGTH]);

#endif
