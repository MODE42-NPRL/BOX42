#include "session_api.h"
#include <stdio.h>

int session_logoff(int fd)
{
    printf("[SESSION] logoff fd=%d\n", fd);
    return 0;
}

int session_exit(int fd)
{
    printf("[SESSION] exit fd=%d\n", fd);
    return 0;
}
