// main.c
#include "coreloop.h"

int main(int argc, char **argv) {
    coreloop_init();
    coreloop_run();
    coreloop_stop();
    return 0;
}
