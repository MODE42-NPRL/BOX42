#include "sysinfo.h"
#include <stdio.h>

int sysinfo_dispatch(int fd, const char *line)
{
    printf("[SYSINFO] fd=%d line=%s\n", fd, line);
    return 0;
}
