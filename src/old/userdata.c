#include "userdata.h"
#include <stdio.h>

int userdata_process(int fd)
{
    printf("[USERDATA] fd=%d\n", fd);
    return 0;
}
