#include "terminal.h"
#include <unistd.h>
#include <string.h>

static void w(int fd, const char *s)
{
    write(fd, s, strlen(s));
}

void term_apply_colors(int fd)
{
    w(fd, "\033[44m\033[37m"); /* dark blue + silver */
}

void term_reset(int fd)
{
    w(fd, "\033[0m");
}

void term_prompt(int fd)
{
    w(fd, "\r\n> "); /* UTF‑8 safe */
}
