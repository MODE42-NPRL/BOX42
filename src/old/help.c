#include "help.h"
#include <stdio.h>

int cmd_help(int fd, const char *line)
{
    printf("[HELP] fd=%d line=%s\n", fd, line);
    return 0;
}
