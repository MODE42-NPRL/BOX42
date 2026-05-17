#include "crypto.h"
#include "tinysha.h"
#include "sha256_fallback.h"
#include "md5_legacy.h"
#include <string.h>
#include <stdio.h>

void crypto_hash_password(const char *pw, char *out)
{
    unsigned char hash[32];

    if (crypto_tinysha_available()) {
        tinysha256((const unsigned char*)pw, strlen(pw), hash);
        sprintf(out, "tinysha:%02x%02x%02x%02x%02x%02x%02x%02x"
                     "%02x%02x%02x%02x%02x%02x%02x%02x"
                     "%02x%02x%02x%02x%02x%02x%02x%02x"
                     "%02x%02x%02x%02x%02x%02x%02x%02x",
                     hash[0], hash[1], hash[2], hash[3],
                     hash[4], hash[5], hash[6], hash[7],
                     hash[8], hash[9], hash[10], hash[11],
                     hash[12], hash[13], hash[14], hash[15],
                     hash[16], hash[17], hash[18], hash[19],
                     hash[20], hash[21], hash[22], hash[23],
                     hash[24], hash[25], hash[26], hash[27],
                     hash[28], hash[29], hash[30], hash[31]);
        return;
    }

    if (crypto_sha256_available()) {
        sha256_fallback(pw, out);
        return;
    }

    md5_legacy(pw, out);
}
