#include "chat_commands.h"
#include <stdio.h>

int chat_join(int fd)
{
    printf("[CHAT] join fd=%d\n", fd);
    return 0;
}

int chat_leave(int fd)
{
    printf("[CHAT] leave fd=%d\n", fd);
    return 0;
}

int chat_say(int fd, const char *line)
{
    printf("[CHAT] say fd=%d line=%s\n", fd, line);
    return 0;
}

int chat_who(int fd)
{
    printf("[CHAT] who fd=%d\n", fd);
    return 0;
}
