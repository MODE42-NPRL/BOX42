#include "coreloop.h"
#include "listener.h"
#include "session.h"
#include "commands.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdatomic.h>

static atomic_int running = 0;

void coreloop_init(void)
{
    printf("[CORE] init\n");
    running = 1;
}

void coreloop_run(void)
{
    printf("[CORE] run loop\n");

    while (running) {

        fd_set rfds;
        FD_ZERO(&rfds);

        int lfd = listener_fd();
        int maxfd = lfd;

        /* Listener-FD überwachen */
        FD_SET(lfd, &rfds);

        /* Sessions eintragen */
        for (int i = 0; i < MAX_SESSIONS; i++) {
            Session *s = session_get_by_index(i);
            if (!s) continue;

            FD_SET(s->fd, &rfds);
            if (s->fd > maxfd)
                maxfd = s->fd;
        }

        /* Blockierendes Warten */
        int r = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (r <= 0)
            continue;

        /* Neue Verbindung? */
        if (FD_ISSET(lfd, &rfds)) {
            listener_accept();
        }

        /* Sessions verarbeiten */
        for (int i = 0; i < MAX_SESSIONS; i++) {
            Session *s = session_get_by_index(i);
            if (!s) continue;

            if (FD_ISSET(s->fd, &rfds)) {
                commands_process(s);
            }
        }
    }
}

void coreloop_stop(void)
{
    printf("[CORE] stop\n");
    running = 0;
}
