#ifndef CHAT_PARSER_H
#define CHAT_PARSER_H

#include "session.h"

void chat_handle_command(Session *s, const char *cmd, int len);

#endif
