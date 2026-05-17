#include "removeme.h"
#include <stdio.h>

int removeme_process(int fd, const char *line)
{
    printf("[REMOVE] fd=%d line=%s\n", fd, line);
    return 0;
}
