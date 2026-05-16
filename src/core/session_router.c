#include "session_router.h"
#include "session.h"
#include "telnet.h"
#include "ssh.h"
#include "kiss.h"
#include "ax25.h"
#include "tnc.h"
#include "tunnel.h"
#include "up42_integration.h"

#include <unistd.h>

static int detect_proto(const uint8_t *buf, int len) {
    if (len <= 0) return 0;

    if (buf[0] == 0xFF) return 1;
    if (buf[0] == 0x1B) return 2;
    if (buf[0] == 0xC0) return 3;
    if (buf[0] == 0xAA) return 4;
    if (buf[0] == 0x55) return 5;
    if (buf[0] == 0x42) return 6;

    return 0;
}

void session_router(Session *s) {
    uint8_t buf[2048];
    ssize_t n = read(s->fd, buf, sizeof(buf));
    if (n <= 0) return;

    s->proto = detect_proto(buf, n);

    switch (s->proto) {
        case 1:
            telnet_handle(s);
            break;
        case 2:
            ssh_handle(s);
            break;
        case 3:
            kiss_handle(s);
            break;
        case 4:
            ax25_handle(s);
            break;
        case 5:
            tnc_handle(s);
            break;
        case 6:
            up42_process(s);
            break;
        default:
            tunnel_handle(s);
            break;
    }
}
