#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void trim(char *s) {
    char *p = s;
    while (*p == ' ' || *p == '\t')
        p++;
    memmove(s, p, strlen(p) + 1);

    int l = strlen(s);
    while (l > 0 && (
        s[l-1] == ' ' ||
        s[l-1] == '\t' ||
        s[l-1] == '\n' ||
        s[l-1] == '\r'))
    {
        s[--l] = 0;
    }
}

void config_load(BOX42_Config *cfg, const char *path) {
    strcpy(cfg->tnc_device, "/dev/ttyUSB0");
    cfg->tnc_baud = 1200;
    cfg->tnc_enabled = 1;
    cfg->tnc_kiss_port = 0;
    strcpy(cfg->tnc_type, "tnc");
    cfg->tnc_power_rtsdtr = 0;

    FILE *f = fopen(path, "r");
    if (!f)
        return;

    char line[256];
    char section[64] = "";

    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (!line[0] || line[0] == '#')
            continue;

        if (line[0] == '[') {
            char *end = strchr(line, ']');
            if (end) {
                *end = 0;
                strcpy(section, line + 1);
            }
            continue;
        }

        char *eq = strchr(line, '=');
        if (!eq)
            continue;

        *eq = 0;

        char key[128], val[128];
        strcpy(key, line);
        strcpy(val, eq + 1);

        trim(key);
        trim(val);

        if (!strcmp(section, "tnc")) {
            if (!strcmp(key, "device"))
                strncpy(cfg->tnc_device, val, CFG_STRLEN);
            else if (!strcmp(key, "baud"))
                cfg->tnc_baud = atoi(val);
            else if (!strcmp(key, "enabled"))
                cfg->tnc_enabled = (!strcmp(val, "yes") || !strcmp(val, "1"));
            else if (!strcmp(key, "kiss_port"))
                cfg->tnc_kiss_port = atoi(val);
            else if (!strcmp(key, "type"))
                strncpy(cfg->tnc_type, val, CFG_STRLEN);
            else if (!strcmp(key, "power_rtsdtr"))
                cfg->tnc_power_rtsdtr = (!strcmp(val, "yes") || !strcmp(val, "1"));
        }
    }

    fclose(f);
}
