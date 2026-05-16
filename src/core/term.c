#include "term.h"
#include <stdio.h>

int term_write(int fd, const char *msg)
{
    /* Platzhalter – später Session‑Output */
    printf("[TERM fd=%d] %s", fd, msg);
    return 0;
}
