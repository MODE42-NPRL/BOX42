/* src/core/hystack.h */

#ifndef HYSTACK_H
#define HYSTACK_H

#include <stdint.h>

typedef enum {
    HSTB42_STOPPED = 0,
    HSTB42_RUNNING = 1
} HSTB42_STATE;

typedef struct {
    int listen_v4;
    int listen_v6;
    HSTB42_STATE state;
} HSTB42_CTX;

int  hystack_init(HSTB42_CTX *ctx, int port);
int  hystack_run(HSTB42_CTX *ctx);
void hystack_stop(HSTB42_CTX *ctx);

#endif
