#pragma once

#define CFG_MAX_LINE 256

typedef struct {
    char callsign[16];

    struct {
        int enabled;
        char device[64];
        int baud;
        int type;
    } tnc[4];

    struct {
        int enabled;
        int interval;
        char dst[16];
        char text[256];
    } beacon[4];

    struct {
        int enabled[4];
        int cross[4][4];
    } digi;

    struct {
        int used;
        int in_port;
        int out_port;
        char dst[16];
    } tf[8];

} BOX42_Config;

extern BOX42_Config g_cfg;

int config_load(const char *path);
void config_apply(void);
