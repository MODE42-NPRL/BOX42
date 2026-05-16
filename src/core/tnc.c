#include "tnc.h"
#include "config.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

extern BOX42_Config g_cfg;

static speed_t baud_to_flag(int baud) {
    switch (baud) {
        case 300:    return B300;
        case 600:    return B600;
        case 1200:   return B1200;
        case 2400:   return B2400;
        case 4800:   return B4800;
        case 9600:   return B9600;
#ifdef B19200
        case 19200:  return B19200;
#endif
#ifdef B38400
        case 38400:  return B38400;
#endif
        default:     return B1200;
    }
}

int tnc_open(const char *dev, int baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) return -1;

    struct termios t;
    tcgetattr(fd, &t);

    cfmakeraw(&t);
    cfsetispeed(&t, baud_to_flag(baud));
    cfsetospeed(&t, baud_to_flag(baud));

    t.c_cflag |= CLOCAL | CREAD;

    tcsetattr(fd, TCSANOW, &t);

    // optional RS232-Power (TNC2C kann über V.24 versorgt werden)
    if (g_cfg.tnc_power_rtsdtr) {
        int flags;
        if (ioctl(fd, TIOCMGET, &flags) == 0) {
            flags |= TIOCM_DTR | TIOCM_RTS;
            ioctl(fd, TIOCMSET, &flags);
        }
    }

    return fd;
}

int tnc_read(int fd, unsigned char *buf, int maxlen) {
    return read(fd, buf, maxlen);
}

int tnc_write(int fd, const unsigned char *buf, int len) {
    return write(fd, len ? fd : fd, len); // <- korrigieren wir gleich unten
}

void tnc_close(int fd) {
    close(fd);
}
