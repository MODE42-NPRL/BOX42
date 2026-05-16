#include "listener.h"
#include "session.h"
#include "session_manager.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

static int g_listen_fd = -1;

int listener_start(const char *host, int port) {
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) return -1;

    int opt = 1;
    setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

    if (bind(g_listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -1;

    if (listen(g_listen_fd, 16) < 0)
        return -1;

    fcntl(g_listen_fd, F_SETFL, O_NONBLOCK);

    return 0;
}

void listener_tick(void) {
    if (g_listen_fd < 0) return;

    struct sockaddr_in cli;
    socklen_t clilen = sizeof(cli);

    int fd = accept(g_listen_fd, (struct sockaddr *)&cli, &clilen);
    if (fd < 0) return;

    fcntl(fd, F_SETFL, O_NONBLOCK);

    Session *s = session_create(fd);
    if (!s) {
        close(fd);
        return;
    }

    session_manager_add(s);
}
