#include "registration.h"
#include <stdio.h>

int registration_process(int fd, const char *line)
{
    printf("[REG] fd=%d line=%s\n", fd, line);
    return 0;
}
