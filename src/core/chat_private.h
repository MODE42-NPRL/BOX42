#pragma once
#include "session.h"

void chat_private_start(Session *s, const char *username);
void chat_private_end(Session *s);
void chat_private_message(Session *s, const char *msg, int len);
void chat_private_header(Session *s, const char *username);
