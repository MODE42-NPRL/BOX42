// src/os/amiga/os_amiga.c
#include "../os.h"
#include <stdio.h>
#include <time.h>

/*
   Dies ist eine portable Stub-Version.
   Später wird sie durch echte AmigaOS 3.9 / 3.2 / AROS / MorphOS
   Implementierungen ersetzt.
*/

void box42_os_init(void)
{
    // später: AmigaOS-spezifische Initialisierung
}

void box42_os_info(struct box42_os_info *out)
{
    if (!out) return;

    out->name = "AmigaOS";
    out->arch = "m68k";
    out->version = "3.x stub";
}

uint64_t box42_os_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

const char *box42_os_config_path(void)
{
    // später: PROGDIR:BOX42.config oder ENV:BOX42/
    return "etc/";
}

const char *box42_os_data_path(void)
{
    // später: T: oder PROGDIR:data/
    return "data/";
}
