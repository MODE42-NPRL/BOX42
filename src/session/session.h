#ifndef BOX42_SESSION_H
#define BOX42_SESSION_H

/* Create a new session for a connected file descriptor.
 * fd = socket, tty, USB-serial, modem, AX.25, etc.
 * Returns 0 on success, negative on error.
 */
int session_create(int fd);

/* Route incoming data for this session.
 * Called by the event loop when fd becomes readable.
 */
int session_route(int fd);

#endif /* BOX42_SESSION_H */
