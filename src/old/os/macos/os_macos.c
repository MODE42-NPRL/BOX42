#include "../../os.h"
#include <unistd.h>
#include <fcntl.h>

int os_init(void)
{
    return 0;
}

box_fd os_serial_open(const char *path)
{
    return open(path, O_RDWR | O_NOCTTY);
}

int os_read(box_fd fd, void *buf, int len)
{
    return read(fd, buf, len);
}

int os_write(box_fd fd, const void *buf, int len)
{
    return write(fd, buf, len);
}

int os_close(box_fd fd)
{
    return close(fd);
}
