#include "mail_start.h"
#include <stdio.h>

int mail_start(int fd, const char *line)
{
    printf("[MAIL] start fd=%d line=%s\n", fd, line);
    return 0;
}
