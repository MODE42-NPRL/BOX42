/* src/core/main.c */

#include "main.h"
#include "config.h"
#include "box42_strds.h"

int main(void) {
    HSTB42_CTX ctx;
    int port = 2323;

    /* Konfiguration laden (optional, nicht fatal bei Fehler) */
    config_load("BOX42.conf");
    config_apply();

    /* STRDS initialisieren (Prompt etc.) */
    strds_init();

    /* HYSTACK starten (reiner TCP-Stack) */
    if (hystack_init(&ctx, port) != 0)
        return 1;

    hystack_run(&ctx);
    hystack_stop(&ctx);

    return 0;
}
