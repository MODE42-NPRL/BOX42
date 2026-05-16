#include "command.h"
#include "commands.h"
#include "session.h"
#include "focus.h"
#include "chat.h"
#include "box.h"
#include "mail.h"
#include "system.h"

void command_dispatch(int fd, const char *line)
{
    struct session *s = session_get(fd);

    /* Focus‑abhängige Eingaben ohne # */
    if (line[0] != '#') {
        switch (s->focus) {
            case FOCUS_CHAT:   chat_say(fd, line); return;
            case FOCUS_MAIL:   mail_input(fd, line); return;
            case FOCUS_BOX:    box_input(fd, line); return;
            case FOCUS_SYSTEM: system_input(fd, line); return;
            default:
                term_write(fd, "Not in a focus area. Use #focus <area>.\n");
                return;
        }
    }

    /* Commands */
    if (cmd_is("#focus", line)) return cmd_focus(fd, line);
    if (cmd_is("#login", line)) return cmd_login(fd, line);
    if (cmd_is("#registration", line)) return cmd_registration(fd, line);
    if (cmd_is("#userdata", line)) return cmd_userdata(fd, line);
    if (cmd_is("#join", line)) return cmd_join(fd, line);
    if (cmd_is("#leave", line)) return cmd_leave(fd, line);
    if (cmd_is("#say", line)) return cmd_say(fd, line);
    if (cmd_is("#who", line)) return cmd_who(fd, line);
    if (cmd_is("#logoff", line)) return cmd_logoff(fd, line);
    if (cmd_is("#exit", line)) return cmd_logoff(fd, line);
    if (cmd_is("#removeme", line)) return cmd_removeme(fd, line);

    term_write(fd, "Unknown command.\n");
}
