#include "chat_private.h"
#include <string.h>
#include <stdio.h>

void chat_private_header(Session *s, const char *username) {
    int width = s->term_width;
    if (width < 20) width = 20;

    char line[512];
    int name_len = strlen(username);

    int core_len = name_len + 6; // ### + name + ###

    if (core_len > width)
        width = core_len;

    int pad_left  = (width - core_len) / 2;
    int pad_right = width - core_len - pad_left;

    int p = 0;

    for (int i = 0; i < pad_left; i++)
        line[p++] = '#';

    line[p++] = '#';
    line[p++] = '#';
    line[p++] = '#';

    memcpy(&line[p], username, name_len);
    p += name_len;

    line[p++] = '#';
    line[p++] = '#';
    line[p++] = '#';

    for (int i = 0; i < pad_right; i++)
        line[p++] = '#';

    line[p] = 0;

    session_send_text(s->id, line);
}

void chat_private_start(Session *s, const char *username) {
    int peer = session_find_by_username(username);
    if (peer < 0) {
        session_send_text(s->id, "User not found");
        return;
    }

    if (peer == s->id) {
        session_send_text(s->id, "Cannot private-chat with yourself");
        return;
    }

    Session *p = session_get(peer);

    if (s->mode == MODE_PRIVATE || p->mode == MODE_PRIVATE) {
        session_send_text(s->id, "Private chat already active");
        return;
    }

    s->mode = MODE_PRIVATE;
    p->mode = MODE_PRIVATE;

    s->private_peer = peer;
    p->private_peer = s->id;

    chat_private_header(s, s->username);
    session_send_text(s->id, "");
    chat_private_header(s, p->username);

    chat_private_header(p, p->username);
    session_send_text(peer, "");
    chat_private_header(p, s->username);

    char msg[128];
    snprintf(msg, sizeof(msg), "[PRIVATE] You are now chatting with %s", p->username);
    session_send_text(s->id, msg);

    snprintf(msg, sizeof(msg), "[PRIVATE] You are now chatting with %s", s->username);
    session_send_text(peer, msg);
}

void chat_private_end(Session *s) {
    if (s->mode != MODE_PRIVATE)
        return;

    int peer = s->private_peer;
    Session *p = session_get(peer);

    session_send_text(s->id, "[PRIVATE] Chat ended");
    session_send_text(peer, "[PRIVATE] Chat ended");

    s->mode = MODE_CHAT;
    p->mode = MODE_CHAT;

    s->private_peer = -1;
    p->private_peer = -1;
}

void chat_private_message(Session *s, const char *msg, int len) {
    int peer = s->private_peer;
    if (peer < 0) return;

    char out[512];
    snprintf(out, sizeof(out), "<%s> %s", s->username, msg);

    session_send_text(peer, out);
    session_send_text(s->id, out);
}
