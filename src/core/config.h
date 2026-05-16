typedef struct {
    char tnc_device[CFG_STRLEN];
    int  tnc_baud;
    int  tnc_enabled;
    int  tnc_kiss_port;
    char tnc_type[CFG_STRLEN];     // tnc | baycom | pccom
    int  tnc_power_rtsdtr;         // 0/1: RS232-Power nutzen
} BOX42_Config;
