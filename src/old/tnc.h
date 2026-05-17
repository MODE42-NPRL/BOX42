#pragma once
#include <stdint.h>

int tnc_open(const char *dev, int baud);
int tnc_send(int port, const uint8_t *buf, int len);
void tnc_tick(void);
