#include "focus.h"
#include <stdio.h>

int focus_process(int fd, const char *line)
{
    printf("[FOCUS] process fd=%d line=%s\n", fd, line);
    return 0;
}

int focus_input(int fd, const char *line)
{
    printf("[FOCUS] input fd=%d line=%s\n", fd, line);
    return 0;
}
