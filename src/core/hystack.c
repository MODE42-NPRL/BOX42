/* hystack.c */

#include "hystack.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../crypto/tinysha.h"   // SHA256
#include "../crypto/crypto.h"    // MD5 fallback

// ------------------------------------------------------------
// Internal banner
// ------------------------------------------------------------
static void hystack_banner(void) {
    printf("hstb42 - Hybrid Stack BOX/42 (raw TCP, hardened)\n");
}

// ------------------------------------------------------------
// SHA256 (primary)
// ------------------------------------------------------------
void hystack_hash_sha256(const char *input, char *output_hex) {
    if (!input || !output_hex) return;

    uint8_t digest[32];
    sha256((const uint8_t*)input, strlen(input), digest);

    for (int i = 0; i < 32; i++)
        sprintf(output_hex + (i * 2), "%02x", digest[i]);

    output_hex[64] = '\0';
}

// ------------------------------------------------------------
// MD5 (fallback)
// ------------------------------------------------------------
void hystack_hash_md5(const char *input, char *output_hex) {
    if (!input || !output_hex) return;

    uint8_t digest[16];
    md5((const uint8_t*)input, strlen(input), digest);

    for (int i = 0; i < 16; i++)
        sprintf(output_hex + (i * 2), "%02x", digest[i]);

    output_hex[32] = '\0';
}

// ------------------------------------------------------------
// Secure Hash (SHA256 → fallback MD5)
// ------------------------------------------------------------
void hystack_hash_secure(const char *input, char *output_hex) {
    if (!input || !output_hex) return;

    // primary: SHA256
    hystack_hash_sha256(input, output_hex);

    // extremely defensive: if somehow empty, fallback to MD5
    if (output_hex[0] == '\0')
        hystack_hash_md5(input, output_hex);
}

// ------------------------------------------------------------
// Mask sensitive data
// ------------------------------------------------------------
void hystack_mask_sensitive(char *output) {
    if (!output) return;
    strcpy(output, "****");
}

// ------------------------------------------------------------
// Create IPv4 listener (raw TCP, no Telnet negotiation)
// ------------------------------------------------------------
static int hystack_create_v4(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    if (listen(fd, 16) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

// ------------------------------------------------------------
// Create IPv6 listener (raw TCP, no Telnet negotiation)
// ------------------------------------------------------------
static int hystack_create_v6(int port) {
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr   = in6addr_any;
    addr.sin6_port   = htons(port);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    if (listen(fd, 16) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

// ------------------------------------------------------------
// Handle a raw TCP session (no Telnet, hardened entry point)
// ------------------------------------------------------------
static void hystack_handle_session(int fd) {
    // No Telnet negotiation.
    // No Telnet options.
    // No interpretation of IAC commands.
    // Pure raw TCP stream for HSTB42.

    const char *banner = "BOX/42 Hybrid Stack (raw TCP, hardened)\r\n";
    write(fd, banner, strlen(banner));

    // Here later: HSTB42 internal frame engine, session router, etc.

    close(fd);
}

// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------
int hystack_init(HSTB42_CTX *ctx, int port) {
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));

    hystack_banner();

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

        if (ctx->listen_v4 >= 0 && FD_ISSET(ctx->listen_v4, &fds)) {
            int fd = accept(ctx->listen_v4, NULL, NULL);
            if (fd >= 0) hystack_handle_session(fd);
        }

        if (ctx->listen_v6 >= 0 && FD_ISSET(ctx->listen_v6, &fds)) {
            int fd = accept(ctx->listen_v6, NULL, NULL);
            if (fd >= 0) hystack_handle_session(fd);
        }
    }

    return 0;
}

void hystack_stop(HSTB42_CTX *ctx) {
    if (!ctx) return;
    ctx->state = HSTB42_STOPPED;

    if (ctx->listen_v4 >= 0) close(ctx->listen_v4);
    if (ctx->listen_v6 >= 0) close(ctx->listen_v6);
}

