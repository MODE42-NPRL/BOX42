#include "chat_private.h"
#include "session.h"
#include <string.h>

void chat_handle_command(Session *s, const char *cmd, int len) {

    if (strncmp(cmd, "#private ", 9) == 0) {
        chat_private_start(s, cmd + 9);
        return;
    }

    if (strcmp(cmd, "#private-end") == 0) {
        chat_private_end(s);
        return;
    }

    if (strncmp(cmd, "#me ", 4) == 0) {
        chat_action(s, cmd + 4);
        return;
    }
}
