#pragma once
#include <stdint.h>

void tnc_layer_init(void);
void tnc_layer_tick(void);
int tnc_layer_send(int port, const uint8_t *buf, int len);
