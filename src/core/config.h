#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

BOX42_Config g_cfg;

static void trim(char *s) {
    char *p = s;
    while (*p==' '||*p=='\t'||*p=='\n'||*p=='\r') p++;
    memmove(s,p,strlen(p)+1);
    int n=strlen(s);
    while(n>0&&(s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\n'||s[n-1]=='\r')) {
        s[n-1]=0;
        n--;
    }
}

int config_load(const char *path) {
    memset(&g_cfg,0,sizeof(g_cfg));

    FILE *f=fopen(path,"r");
    if(!f) return -1;

    char line[CFG_MAX_LINE];
    while(fgets(line,sizeof(line),f)) {
        trim(line);
        if(!line[0]||line[0]=='#') continue;

        char *eq=strchr(line,'=');
        if(!eq) continue;

        *eq=0;
        char *key=line;
        char *val=eq+1;
        trim(key);
        trim(val);

        if(!strcmp(key,"callsign")) strncpy(g_cfg.callsign,val,sizeof(g_cfg.callsign)-1);

        for(int i=0;i<4;i++) {
            char k1[64];
            snprintf(k1,sizeof(k1),"tnc%d_enabled",i);
            if(!strcmp(key,k1)) g_cfg.tnc[i].enabled=atoi(val);

            snprintf(k1,sizeof(k1),"tnc%d_device",i);
            if(!strcmp(key,k1)) strncpy(g_cfg.tnc[i].device,val,sizeof(g_cfg.tnc[i].device)-1);

            snprintf(k1,sizeof(k1),"tnc%d_baud",i);
            if(!strcmp(key,k1)) g_cfg.tnc[i].baud=atoi(val);

            snprintf(k1,sizeof(k1),"tnc%d_type",i);
            if(!strcmp(key,k1)) g_cfg.tnc[i].type=atoi(val);
        }

        for(int i=0;i<4;i++) {
            char k1[64];
            snprintf(k1,sizeof(k1),"beacon%d_enabled",i);
            if(!strcmp(key,k1)) g_cfg.beacon[i].enabled=atoi(val);

            snprintf(k1,sizeof(k1),"beacon%d_interval",i);
            if(!strcmp(key,k1)) g_cfg.beacon[i].interval=atoi(val);

            snprintf(k1,sizeof(k1),"beacon%d_dst",i);
            if(!strcmp(key,k1)) strncpy(g_cfg.beacon[i].dst,val,sizeof(g_cfg.beacon[i].dst)-1);

            snprintf(k1,sizeof(k1),"beacon%d_text",i);
            if(!strcmp(key,k1)) strncpy(g_cfg.beacon[i].text,val,sizeof(g_cfg.beacon[i].text)-1);
        }

        for(int i=0;i<4;i++) {
            char k1[64];
            snprintf(k1,sizeof(k1),"digi_enabled%d",i);
            if(!strcmp(key,k1)) g_cfg.digi.enabled[i]=atoi(val);

            for(int j=0;j<4;j++) {
                snprintf(k1,sizeof(k1),"digi_cross%d_%d",i,j);
                if(!strcmp(key,k1)) g_cfg.digi.cross[i][j]=atoi(val);
            }
        }

        for(int i=0;i<8;i++) {
            char k1[64];
            snprintf(k1,sizeof(k1),"tf%d_used",i);
            if(!strcmp(key,k1)) g_cfg.tf[i].used=atoi(val);

            snprintf(k1,sizeof(k1),"tf%d_in_port",i);
            if(!strcmp(key,k1)) g_cfg.tf[i].in_port=atoi(val);

            snprintf(k1,sizeof(k1),"tf%d_out_port",i);
            if(!strcmp(key,k1)) g_cfg.tf[i].out_port=atoi(val);

            snprintf(k1,sizeof(k1),"tf%d_dst",i);
            if(!strcmp(key,k1)) strncpy(g_cfg.tf[i].dst,val,sizeof(g_cfg.tf[i].dst)-1);
        }
    }

    fclose(f);
    return 0;
}
