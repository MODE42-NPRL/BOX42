#pragma once
#include "session.h"

/* Minimal private-chat stubs for the modern BOX42 build */

void chat_private_header(Session *s, const char *username);
void chat_private_start(Session *s, const char *username);
void chat_private_end(Session *s);
void chat_private_message(Session *s, const char *msg);
