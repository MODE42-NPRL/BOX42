#include "storage.h"
#include "sqlite_common.h"
#include "../os/os.h"

static int remote_fd = -1;

int storage_init(void)
{
    const char *host = config_get("sqlite_remote_host");
    int port = config_get_int("sqlite_remote_port");

    remote_fd = os_remote_connect(host, port);
    return (remote_fd >= 0) ? 0 : -1;
}

static int remote_cmd(const char *cmd, const char *a, const char *b, const char *c)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "%s|%s|%s|%s\n",
             cmd,
             a ? a : "",
             b ? b : "",
             c ? c : "");

    os_write(remote_fd, buf, strlen(buf));

    char reply[256];
    int n = os_read(remote_fd, reply, sizeof(reply)-1);
    if (n <= 0) return -1;

    reply[n] = 0;
    return atoi(reply);
}

int storage_user_exists(const char *user)
{
    return remote_cmd("EXISTS", user, NULL, NULL);
}

int storage_user_auth(const char *user, const char *pass)
{
    return remote_cmd("AUTH", user, pass, NULL);
}

int storage_user_add(const char *user, const char *pass, int level)
{
    char lvl[8];
    snprintf(lvl, sizeof(lvl), "%d", level);
    return remote_cmd("ADDUSER", user, pass, lvl);
}

int storage_user_level(const char *user)
{
    return remote_cmd("LEVEL", user, NULL, NULL);
}
