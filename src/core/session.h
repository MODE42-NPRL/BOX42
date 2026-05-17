#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#define MAX_SESSIONS 128

struct AX25_Frame;

/* Session Modes */
#define SESSION_MODE_COMMAND 0
#define SESSION_MODE_MAIL    1
#define SESSION_MODE_ADMIN   2
#define SESSION_MODE_CHAT    3
#define SESSION_MODE_SILENT  4
#define SESSION_MODE_RAW     5

typedef struct {
    int fd;
    char username[32];
    int level;

    int active;
    int mode;

    int ax25_bind;
    int ax25_echo;

    int use_up42;

    /* Mail-System */
    char *mail_buffer;      /* dynamischer Textpuffer */
    char mail_target[32];   /* Empfänger */

} Session;

/* Core API */
Session *session_create(int fd, const char *username, int level);
void     session_destroy(int fd);
Session *session_get(int fd);
Session *session_get_by_index(int idx);

/* Manager */
void session_manager_add(Session *s);
void session_manager_remove(Session *s);
void session_manager_tick(void);

/* Router */
void session_router_init(void);
void session_router_tick(void);
void session_router_handle(Session *s, const uint8_t *buf, int len);
void session_router_ax25_in(const struct AX25_Frame *f, const uint8_t *raw, int len);

/* Low-level send */
void session_send(Session *s, const uint8_t *buf, int len);
void session_write(Session *s, const char *text);

/* Mode setter */
void session_set_mode(Session *s, int mode);

#endif
