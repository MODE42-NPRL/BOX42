#include "system_command.h"
#include <stdio.h>

int system_command(int fd, const char *line)
{
    printf("[SYSTEM] fd=%d line=%s\n", fd, line);
    return 0;
}
