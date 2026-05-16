#include "session.h"
#include "../core/terminal.h"
#include "../core/banner.h"

int session_create(int fd)
{
    term_apply_colors(fd);
    banner_show(fd);
    term_prompt(fd);
    return 0;
}
