#include "chat_action.h"
#include <stdio.h>

/*
 * Parst eine Chat-Action, z.B. "#me waves"
 */
chat_action chat_action_parse(const char *text)
{
    chat_action act;
    act.type = CHAT_ACTION_EMOTE;
    act.text = text;
    return act;
}

/*
 * Führt eine Chat-Action aus.
 * Aktuell nur Emotes: "* text"
 */
void chat_action_execute(const chat_action *act)
{
    if (!act || act->type == CHAT_ACTION_NONE || !act->text)
        return;

    printf("* %s\n", act->text);
}
