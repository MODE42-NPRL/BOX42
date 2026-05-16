#include "config.h"
#include "listener.h"
#include "tnc.h"

int g_tnc_fd[4];

void config_apply(void) {
    for(int i=0;i<4;i++) g_tnc_fd[i]=-1;

    for(int i=0;i<4;i++) {
        if(g_cfg.tnc[i].enabled && g_cfg.tnc[i].device[0]) {
            g_tnc_fd[i]=tnc_open(g_cfg.tnc[i].device,g_cfg.tnc[i].baud);
        }
    }
}
