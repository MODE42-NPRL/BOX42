#ifndef SESSION_H
#define SESSION_H

typedef struct {
    int fd;
    int proto;
    int use_up42;
} Session;

void session_init(Session *s, int fd, int proto);
void session_handle(Session *s);

#endif
