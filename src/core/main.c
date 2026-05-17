#include "hystack.h"

int main(void) {
    HSTB42_CTX ctx;

    int port_v4 = 2323;
    int port_v6 = 2323;

    hystack_init(&ctx, port_v4, port_v6);
    hystack_run(&ctx);
    hystack_stop(&ctx);

    return 0;
}
