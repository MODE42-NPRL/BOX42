#ifndef CHAT_H
#define CHAT_H

#include "session.h"
#include "chat_parser.h"
#include "chat_private.h"
#include "chat_action.h"

/*
 * High-level chat interface used by command.c and other modules.
 * This unifies all chat-related subsystems.
 */

/* Handle a full chat line (command or message) */
void chat_handle_line(Session *s, const char *line);

/* Send a message only to this session */
void chat_send(Session *s, const char *msg);

/* Broadcast a message to all sessions (placeholder) */
void chat_broadcast(Session *s, const char *msg);

#endif
