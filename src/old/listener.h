#ifndef LISTENER_H
#define LISTENER_H

/* Initialisiert den Listener (IPv4 + IPv6 Dual-Stack) */
int listener_init(const char *host, int port);

/* Liefert den Listener-FD für select() */
int listener_fd(void);

/* Akzeptiert neue Verbindungen und erzeugt Sessions */
void listener_accept(void);

#endif
