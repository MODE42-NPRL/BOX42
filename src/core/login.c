#include "login.h"
#include <stdio.h>

int login_process(int fd, const char *line)
{
    printf("[LOGIN] fd=%d line=%s\n", fd, line);
    return 0;
}

