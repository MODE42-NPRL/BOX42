#include "listener.h"
#include "session.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void start_listener(int port, int proto) {
    int fd = socket(AF_INET6, SOCK_STREAM, 0);

    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in6 addr = {0};
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(port);

    bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(fd, 16);

    while (1) {
        int cfd = accept(fd, 0, 0);
        Session s;
        session_init(&s, cfd, proto);
        session_handle(&s);
        close(cfd);
    }
}
