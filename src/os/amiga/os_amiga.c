#include "os.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/bsdsocket.h>

int os_init(void)
{
    /* Load bsdsocket.library */
    return 0;
}

box_fd os_serial_open(const char *path)
{
    /* AmigaOS: OpenDevice("serial.device", ...) */
    return -1;
}

int os_read(box_fd fd, void *buf, int len)
{
    return 0;
}

int os_write(box_fd fd, const void *buf, int len)
{
    return len;
}

int os_close(box_fd fd)
{
    return 0;
}
