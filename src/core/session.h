#pragma once
#include <stdint.h>

typedef struct Session {
    int fd;
    int proto;
    int use_up42;
    int port;
    uint8_t buf[2048];
    int buflen;
} Session;

Session *session_create(int fd);
void session_destroy(Session *s);
void session_reset(Session *s);
