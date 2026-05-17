#include "box.h"
#include "events.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static BOX_Store store;

void box_init() {
    store.count = 0;
}

// Platzhalter: später echte Owner/Admin/Mod-Logik
BOX_Role box_get_role(const char *callsign) {
    if (!strcmp(callsign, "ANDREAS")) return BOX_ROLE_OWNER;
    return BOX_ROLE_USER;
}

static void box_store(const char *from, const char *area, const char *text) {
    if (store.count >= BOX_MAX_MSGS) return;

    BOX_Message *m = &store.msgs[store.count++];
    strncpy(m->from, from, sizeof(m->from));
    strncpy(m->area, area, sizeof(m->area));
    strncpy(m->text, text, sizeof(m->text));
}

static int box_list(char *out, int max) {
    int pos = 0;
    for (int i = 0; i < store.count; i++) {
        int n = snprintf(out + pos, max - pos,
                         "%d: [%s] %s\n",
                         i,
                         store.msgs[i].area,
                         store.msgs[i].from);
        if (n <= 0) break;
        pos += n;
        if (pos >= max) break;
    }
    return pos;
}

static int box_read(int id, char *out, int max) {
    if (id < 0 || id >= store.count) return 0;
    return snprintf(out, max, "%s\n", store.msgs[id].text);
}

void box_process_ax25(AX25_Frame *f) {
    BOX_Role role = box_get_role(f->src);

    char *cmd = strtok(f->payload, " ");
    char *arg = strtok(NULL, "");

    if (!cmd) return;

    if (!strcmp(cmd, "POST")) {
        if (!arg) return;
        box_store(f->src, "MAIN", arg);
    }
    else if (!strcmp(cmd, "LIST")) {
        char out[512];
        box_list(out, sizeof(out));

        AX25_Frame reply;
        strcpy(reply.src, "BOX42");
        strcpy(reply.dst, f->src);
        strncpy(reply.payload, out, sizeof(reply.payload));

        event_emit(11, &reply);
    }
    else if (!strcmp(cmd, "READ")) {
        if (!arg) return;
        int id = atoi(arg);

        char out[512];
        box_read(id, out, sizeof(out));

        AX25_Frame reply;
        strcpy(reply.src, "BOX42");
        strcpy(reply.dst, f->src);
        strncpy(reply.payload, out, sizeof(reply.payload));

        event_emit(11, &reply);
    }
    else if (!strcmp(cmd, "DEL")) {
        if (role < BOX_ROLE_MOD) return; // nur Mods/Admins/Owner

        if (!arg) return;
        int id = atoi(arg);

        if (id >= 0 && id < store.count) {
            for (int i = id; i < store.count - 1; i++)
                store.msgs[i] = store.msgs[i+1];
            store.count--;
        }
    }
    else if (!strcmp(cmd, "HELP")) {
        AX25_Frame reply;
        strcpy(reply.src, "BOX42");
        strcpy(reply.dst, f->src);

        if (role >= BOX_ROLE_MOD) {
            strcpy(reply.payload,
                   "Commands:\n"
                   "POST <text>\n"
                   "LIST\n"
                   "READ <id>\n"
                   "DEL <id>\n"
                   "HELP\n");
        } else {
            strcpy(reply.payload,
                   "Commands:\n"
                   "POST <text>\n"
                   "LIST\n"
                   "READ <id>\n"
                   "HELP\n");
        }

        event_emit(11, &reply);
    }
}
