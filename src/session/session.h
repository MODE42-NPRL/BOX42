#ifndef BOX42_SESSION_H
#define BOX42_SESSION_H

#include <stdint.h>

#define MODE_NORMAL   0
#define MODE_CHAT     1
#define MODE_PRIVATE  2

typedef struct Session {

    // --- Basis ---
    int id;

    // --- Chat-System ---
    int mode;
    int private_peer;
    int term_width;
    char username[64];

    // --- Netzwerk / Telnet / SSH / AX.25 ---
    int fd;                 // File descriptor für Telnet/SSH/AX25
    int port;               // Eingangsport (AX25, TNC, Tunnel)
    char call[16];          // Rufzeichen
    char addr[64];          // Remote-Adresse (Telnet/SSH)

    // --- UP42 Integration ---
    int use_up42;           // 1 = UP42 aktiv

    // --- Session-Manager ---
    uint64_t last_activity; // Timestamp

} Session;

Session *session_get(int id);
int session_find_by_username(const char *name);
void session_send_text(int sid, const char *text);
void session_close(Session *s);

#endif

