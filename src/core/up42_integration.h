#pragma once
#include "session.h"
#include <stdint.h>

int up42_encode(Session *s, const uint8_t *in, int len, uint8_t *out, int max);
int up42_decode(Session *s, const uint8_t *in, int len, uint8_t *out, int max);
void up42_process(Session *s);
