#include "config.h"
#include "config_apply.h"
#include "listener.h"
#include "session_manager.h"
#include "session_router.h"
#include "tnc_layer.h"

#include <unistd.h>

int main(void) {
    if (config_load("BOX42.config") < 0)
        return 1;

    config_apply();
    tnc_layer_init();

    for (;;) {
        listener_tick();
        session_manager_tick();
        tnc_layer_tick();
        usleep(1000);
    }

    return 0;
}
