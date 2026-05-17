#include "commands.h"
#include "session.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

static void handle_command(Session *s, const char *cmd)
{
    /* Minimaler Testbefehl */
    if (strcmp(cmd, "ping") == 0) {
        write(s->fd, "pong\r\n", 6);
        return;
    }

    write(s->fd, "Unknown command\r\n", 17);
}

void commands_process(Session *s)
{
    char buf[256];
    int n = read(s->fd, buf, sizeof(buf)-1);

    if (n <= 0) {
        /* Verbindung geschlossen */
        session_destroy(s->fd);
        close(s->fd);
        return;
    }

    buf[n] = '\0';

    /* Zeilenweise verarbeiten */
    char *p = strtok(buf, "\r\n");
    while (p) {
        handle_command(s, p);
        p = strtok(NULL, "\r\n");
    }
}
