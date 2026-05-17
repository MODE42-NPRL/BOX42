#include "chat_private.h"
#include "session.h"
#include <stdio.h>

/*
 * Private chat is not implemented in this minimal BOX42 build.
 * These functions exist only so that command.c and chat_parser.c can link.
 */

void chat_private_header(Session *s, const char *username)
{
    printf("[PRIVATE] header: %s\n", username);
}

void chat_private_start(Session *s, const char *username)
{
    printf("[PRIVATE] start with %s (not implemented)\n", username);
}

void chat_private_end(Session *s)
{
    printf("[PRIVATE] end (not implemented)\n");
}

void chat_private_message(Session *s, const char *msg)
{
    printf("[PRIVATE] %s\n", msg);
}
