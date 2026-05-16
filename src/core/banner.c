#include "banner.h"
#include <unistd.h>
#include <string.h>

static void w(int fd, const char *s)
{
    write(fd, s, strlen(s));
}

void banner_show(int fd)
{
    w(fd, "\r\nBOX42 – UTF‑8 Ready\r\n");
}
