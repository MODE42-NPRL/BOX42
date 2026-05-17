#include "node.h"
#include <stdio.h>

int node_command(int fd, const char *line)
{
    printf("[NODE] fd=%d line=%s\n", fd, line);
    return 0;
}
