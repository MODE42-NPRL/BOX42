#ifndef BOX42_OS_H
#define BOX42_OS_H

/* OS‑agnostic file descriptor type */
typedef int box_fd;

/* OS init */
int os_init(void);

/* Open serial/modem device */
box_fd os_serial_open(const char *path);

/* Read/write */
int os_read(box_fd fd, void *buf, int len);
int os_write(box_fd fd, const void *buf, int len);

/* Close */
int os_close(box_fd fd);

#endif
