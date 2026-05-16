#include "session.h"
#include "prompt.h"

const char *prompt_for_session(int fd)
{
    struct session *s = session_get(fd);

    switch (s->focus) {
        case FOCUS_CHAT:   return "[chat]>";
        case FOCUS_MAIL:   return "[mail]>";
        case FOCUS_BOX:    return "[box]>";
        case FOCUS_SYSTEM: return "[system]>";
        default:           return "[idle]>";
    }
}
