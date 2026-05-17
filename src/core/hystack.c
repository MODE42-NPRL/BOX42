/* src/core/hystack.c */

#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#include "hystack.h"
#include "box42_strds.h"

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

// ------------------------------------------------------------
// Create IPv4 listener
// ------------------------------------------------------------
static int hystack_create_v4(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&a, sizeof(a)) < 0) { close(fd); return -1; }
    if (listen(fd, 16) < 0) { close(fd); return -1; }

    return fd;
}

// ------------------------------------------------------------
// Create IPv6 listener
// ------------------------------------------------------------
static int hystack_create_v6(int port) {
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in6 a;
    memset(&a, 0, sizeof(a));
    a.sin6_family = AF_INET6;
    a.sin6_addr = in6addr_any;
    a.sin6_port = htons(port);

    if (bind(fd, (struct sockaddr*)&a, sizeof(a)) < 0) { close(fd); return -1; }
    if (listen(fd, 16) < 0) { close(fd); return -1; }

    return fd;
}

// ------------------------------------------------------------
// Accept → STRDS Session Start
// ------------------------------------------------------------
static void hystack_accept(int fd) {
    int cfd = accept(fd, NULL, NULL);
    if (cfd >= 0)
        strds_session_start(cfd);
}

// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------
int hystack_init(HSTB42_CTX *ctx, int port) {
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));

    ctx->listen_v4 = hystack_create_v4(port);
    ctx->listen_v6 = hystack_create_v6(port);

    if (ctx->listen_v4 < 0 && ctx->listen_v6 < 0)
        return -1;

    ctx->state = HSTB42_STOPPED;
    return 0;
}

int hystack_run(HSTB42_CTX *ctx) {
    if (!ctx) return -1;
    ctx->state = HSTB42_RUNNING;

    fd_set fds;
    int maxfd = (ctx->listen_v4 > ctx->listen_v6 ? ctx->listen_v4 : ctx->listen_v6);

    while (ctx->state == HSTB42_RUNNING) {
        FD_ZERO(&fds);

        if (ctx->listen_v4 >= 0) FD_SET(ctx->listen_v4, &fds);
        if (ctx->listen_v6 >= 0) FD_SET(ctx->listen_v6, &fds);

        int r = select(maxfd + 1, &fds, NULL, NULL, NULL);
        if (r < 0) continue;

        if (ctx->listen_v4 >= 0 && FD_ISSET(ctx->listen_v4, &fds))
            hystack_accept(ctx->listen_v4);

        if (ctx->listen_v6 >= 0 && FD_ISSET(ctx->listen_v6, &fds))
            hystack_accept(ctx->listen_v6);
    }

    return 0;
}

void hystack_stop(HSTB42_CTX *ctx) {
    if (!ctx) return;

    if (ctx->listen_v4 >= 0) close(ctx->listen_v4);
    if (ctx->listen_v6 >= 0) close(ctx->listen_v6);

    ctx->state = HSTB42_STOPPED;
}
