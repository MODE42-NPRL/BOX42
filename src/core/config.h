#ifndef BOX42_CONFIG_H
#define BOX42_CONFIG_H

#include <stdint.h>

#define CFG_MAX_LINE 512

/* -------------------------------
   TNC-Konfiguration
-------------------------------- */
typedef struct {
    int enabled;
    char device[128];
    int baud;
    int type;
} TNC_Config;

/* -------------------------------
   Beacon-Konfiguration
-------------------------------- */
typedef struct {
    int enabled;
    int interval;
    char dst[16];
    char text[256];
} Beacon_Config;

/* -------------------------------
   Digi-Konfiguration
-------------------------------- */
typedef struct {
    int enabled[4];
    int cross[4][4];
} DIGI_Config;

/* -------------------------------
   Transport-Forwarding (TF)
-------------------------------- */
typedef struct {
    int used;
    int in_port;
    int out_port;
    char dst[16];
} TF_Entry;

/* -------------------------------
   Haupt-Konfigurationsstruktur
-------------------------------- */
typedef struct {
    char callsign[16];

    TNC_Config    tnc[4];
    Beacon_Config beacon[4];
    DIGI_Config   digi;
    TF_Entry      tf[8];

} BOX42_Config;

/* Globale Konfiguration */
extern BOX42_Config g_cfg;

/* -------------------------------
   Funktionsdeklarationen
-------------------------------- */
int config_load(const char *path);
void config_apply(void);

#endif
