#include "session.h"
#include "chat_private.h"
#include <string.h>
#include <stdio.h>

void chat_action(Session *s, const char *text) {
    char out[512];
    snprintf(out, sizeof(out), "* %s %s", s->username, text);

    if (s->mode == MODE_PRIVATE) {
        int peer = s->private_peer;
        session_send_text(peer, out);
        session_send_text(s->id, out);
        return;
    }

    // In Channels handled elsewhere
}
