#include "chat.h"
#include <string.h>
#include <stdio.h>

void chat_handle_line(Session *s, const char *line)
{
    if (!line || !*line)
        return;

    /* Commands start with '#' */
    if (line[0] == '#') {
        chat_handle_command(s, line, strlen(line));
        return;
    }

    /* Normal message */
    chat_broadcast(s, line);
}

void chat_send(Session *s, const char *msg)
{
    /* Placeholder: replace with BOX42 session output */
    printf("(to %s) %s\n", s->username, msg);
}

void chat_broadcast(Session *s, const char *msg)
{
    /* Placeholder: replace with global chat broadcast */
    printf("[%s] %s\n", s->username, msg);
}
