#include "bootstrap.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

/* Create directory if it does not exist */
static int mk(const char *path)
{
    struct stat st;

    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            printf("  = exists:  %s\n", path);
            return 0;
        }
        printf("  ! error: %s exists but is not a directory\n", path);
        return -1;
    }

    if (mkdir(path, 0755) == 0) {
        printf("  + created: %s\n", path);
        return 0;
    }

    printf("  ! error: cannot create %s\n", path);
    return -1;
}

int box42_bootstrap(void)
{
    printf("Initializing BOX42 directory structure...\n");

    if (mk("bin")) return -1;

    if (mk("build")) return -1;
    if (mk("build/obj")) return -1;
    if (mk("build/dep")) return -1;
    if (mk("build/logs")) return -1;

    if (mk("src")) return -1;
    if (mk("src/core")) return -1;

    if (mk("src/net")) return -1;
    if (mk("src/net/telnet")) return -1;
    if (mk("src/net/ssh")) return -1;
    if (mk("src/net/ssh/ssh_keys")) return -1;
    if (mk("src/net/serial")) return -1;
    if (mk("src/net/usb")) return -1;
    if (mk("src/net/usb/drivers")) return -1;
    if (mk("src/net/packet")) return -1;

    if (mk("src/session")) return -1;
    if (mk("src/cmd")) return -1;
    if (mk("src/subsystems")) return -1;
    if (mk("src/storage")) return -1;
    if (mk("src/util")) return -1;
    if (mk("src/include")) return -1;

    if (mk("etc")) return -1;

    if (mk("data")) return -1;
    if (mk("data/logs")) return -1;
    if (mk("data/mail")) return -1;

    if (mk("tools")) return -1;

    printf("Done.\n");
    return 0;
}
