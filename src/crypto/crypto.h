#ifndef BOX42_CRYPTO_H
#define BOX42_CRYPTO_H

int crypto_tinysha_available(void);
int crypto_sha256_available(void);
int crypto_md5_available(void);

void crypto_hash_password(const char *pw, char *out);

#endif
