#include "listener.h"
#include "session.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int listen_fd = -1;

static void set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int listener_fd(void)
{
    return listen_fd;
}

int listener_init(const char *host, int port)
{
    struct sockaddr_in6 addr6;

    listen_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket(AF_INET6)");
        return -1;
    }

    int no = 0;

    /* Dual-Stack aktivieren: IPv4 + IPv6 */
    if (setsockopt(listen_fd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
        perror("setsockopt(IPV6_V6ONLY)");
        return -1;
    }

    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    memset(&addr6, 0, sizeof(addr6));
    addr6.sin6_family = AF_INET6;
    addr6.sin6_port = htons(port);

    if (inet_pton(AF_INET6, host, &addr6.sin6_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    if (bind(listen_fd, (struct sockaddr *)&addr6, sizeof(addr6)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listen_fd, 16) < 0) {
        perror("listen");
        return -1;
    }

    set_nonblock(listen_fd);

    printf("[LISTENER] Dual-Stack listening on [%s]:%d\n", host, port);
    return 0;
}

void listener_accept(void)
{
    struct sockaddr_in6 addr;
    socklen_t len = sizeof(addr);

    int fd = accept(listen_fd, (struct sockaddr *)&addr, &len);
    if (fd < 0)
        return;

    set_nonblock(fd);

    /* Session erzeugen */
    session_create(fd, "", 0);

    printf("[LISTENER] new connection fd=%d\n", fd);
}
