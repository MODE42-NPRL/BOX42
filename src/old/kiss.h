#pragma once
#include <stdint.h>

void kiss_init(void);
int kiss_feed(int port, const uint8_t *buf, int len);
int kiss_get_frame(int port, uint8_t *out, int max);
