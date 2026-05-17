#pragma once
#include "ax25.h"

void transport_forward_init(void);
void transport_forward_process(const AX25_Frame *f);
