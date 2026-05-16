#include "config.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

BOX42_Config g_cfg;

static void cfg_defaults(void) {
    memset(&g_cfg, 0, sizeof(g_cfg));
    strncpy(g_cfg.callsign, "BOX42-1", 15);

    g_cfg.tnc[0].enabled = 1;
    strncpy(g_cfg.tnc[0].device, "/dev/ttyUSB0", 63);
    g_cfg.tnc[0].baud = 1200;
    g_cfg.tnc[0].type = 0;

    g_cfg.beacon[0].enabled = 1;
    g_cfg.beacon[0].interval = 600;
    strncpy(g_cfg.beacon[0].dst, "APRS", 15);
    strncpy(g_cfg.beacon[0].text, "BOX42-1 Node", 255);

    g_cfg.digi.enabled[0] = 1;
    g_cfg.digi.enabled[1] = 1;
    g_cfg.digi.cross[0][1] = 1;
    g_cfg.digi.cross[1][0] = 1;

    g_cfg.tf[0].used = 1;
    g_cfg.tf[0].in_port = 0;
    g_cfg.tf[0].out_port = 1;
    strncpy(g_cfg.tf[0].dst, "DB0XYZ", 15);
}

static void trim(char *s) {
    int n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r'))
        s[--n] = 0;
}

int config_load(const char *path) {
    cfg_defaults();

    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char line[CFG_MAX_LINE];

    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (line[0] == '#' || line[0] == 0)
            continue;

        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = 0;
        char *key = line;
        char *val = eq + 1;

        if (!strcmp(key, "CALLSIGN")) {
            strncpy(g_cfg.callsign, val, 15);
            continue;
        }

        if (!strncmp(key, "TNC", 3)) {
            int idx = key[3] - '0';
            if (idx >= 0 && idx < 4) {
                if (strstr(key, "_ENABLED"))
                    g_cfg.tnc[idx].enabled = atoi(val);
                else if (strstr(key, "_DEVICE"))
                    strncpy(g_cfg.tnc[idx].device, val, 63);
                else if (strstr(key, "_BAUD"))
                    g_cfg.tnc[idx].baud = atoi(val);
                else if (strstr(key, "_TYPE"))
                    g_cfg.tnc[idx].type = atoi(val);
            }
            continue;
        }

        if (!strncmp(key, "BEACON", 6)) {
            int idx = key[6] - '0';
            if (idx >= 0 && idx < 4) {
                if (strstr(key, "_ENABLED"))
                    g_cfg.beacon[idx].enabled = atoi(val);
                else if (strstr(key, "_INTERVAL"))
                    g_cfg.beacon[idx].interval = atoi(val);
                else if (strstr(key, "_DST"))
                    strncpy(g_cfg.beacon[idx].dst, val, 15);
                else if (strstr(key, "_TEXT"))
                    strncpy(g_cfg.beacon[idx].text, val, 255);
            }
            continue;
        }

        if (!strncmp(key, "DIGI", 4)) {
            if (strstr(key, "_ENABLED")) {
                int idx = key[4] - '0';
                if (idx >= 0 && idx < 4)
                    g_cfg.digi.enabled[idx] = atoi(val);
            } else if (strstr(key, "_TO_")) {
                int a = key[4] - '0';
                int b = key[7] - '0';
                if (a >= 0 && a < 4 && b >= 0 && b < 4)
                    g_cfg.digi.cross[a][b] = atoi(val);
            }
            continue;
        }

        if (!strncmp(key, "TF", 2)) {
            int idx = key[2] - '0';
            if (idx >= 0 && idx < 8) {
                g_cfg.tf[idx].used = 1;
                if (strstr(key, "_IN"))
                    g_cfg.tf[idx].in_port = atoi(val);
                else if (strstr(key, "_OUT"))
                    g_cfg.tf[idx].out_port = atoi(val);
                else if (strstr(key, "_DST"))
                    strncpy(g_cfg.tf[idx].dst, val, 15);
            }
            continue;
        }
    }

    fclose(f);
    return 1;
}

void config_apply(void) {
}
