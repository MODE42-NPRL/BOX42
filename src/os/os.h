#ifndef BOX42_OS_H
#define BOX42_OS_H

typedef int box_fd;

/* Initialize OS backend */
int os_init(void);

/* Serial / TTY open */
box_fd os_serial_open(const char *path);

/* Read / Write */
int os_read(box_fd fd, void *buf, int len);
int os_write(box_fd fd, const void *buf, int len);

/* Close */
int os_close(box_fd fd);

#endif
