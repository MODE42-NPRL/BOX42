#pragma once
#include "ax25.h"

void digipeater_init(void);
void digipeater_process(const AX25_Frame *f);
