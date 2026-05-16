#ifndef CHAT_ACTION_H
#define CHAT_ACTION_H

#include <stddef.h>

/* Typ der Chat-Action */
typedef enum {
    CHAT_ACTION_NONE = 0,
    CHAT_ACTION_EMOTE,      /* z.B. "#me ..." */
} chat_action_type;

/* Repräsentiert eine geparste Chat-Action */
typedef struct {
    chat_action_type type;
    const char *text;       /* Text der Aktion, z.B. Emote-Text */
} chat_action;

/* Parst eine Aktion aus einem String, z.B. nach "#me " */
chat_action chat_action_parse(const char *text);

/* Führt eine Aktion aus (Ausgabe, Broadcast, etc.) */
void chat_action_execute(const chat_action *act);

#endif
