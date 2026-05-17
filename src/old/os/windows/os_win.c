#include "../../os.h"
#include <windows.h>

int os_init(void)
{
    return 0;
}

box_fd os_serial_open(const char *path)
{
    (void)path;
    return -1;
}

int os_read(box_fd fd, void *buf, int len)
{
    (void)fd; (void)buf; (void)len;
    return 0;
}

int os_write(box_fd fd, const void *buf, int len)
{
    (void)fd; (void)buf;
    return len;
}

int os_close(box_fd fd)
{
    (void)fd;
    return 0;
}
