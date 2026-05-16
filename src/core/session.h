#ifndef SESSION_H
#define SESSION_H

/* Basis-Session-Typ für BOX42 */

typedef struct Session {
    int  fd;
    char username[64];
    int  level;

    int use_up42;   /* 0 = normal, 1 = UP4.2 tunneling enabled */
} Session;

/* Liefert die Session zu einem fd (oder NULL, wenn keine existiert) */
Session *session_get(int fd);

#endif
