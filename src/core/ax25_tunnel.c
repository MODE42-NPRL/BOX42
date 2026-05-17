#include <stdint.h>
#include <unistd.h>
#include "ax25_tunnel.h"
#include "session.h"

void ax25_tunnel_handle(Session *s)
{
    uint8_t buf[2048];

    ssize_t n = read(s->fd, buf, sizeof(buf));
    if (n <= 0)
        return;

    if (s->use_up42) {
        /* future: UP42 tunnel mode */
        /* currently no-op */
    } else {
        /* future: normal tunnel mode */
        /* currently no-op */
    }
}
