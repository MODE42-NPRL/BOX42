#pragma once

int tnc_open(const char *dev, int baud);
int tnc_read(int fd, unsigned char *buf, int maxlen);
int tnc_write(int fd, const unsigned char *buf, int len);
void tnc_close(int fd);
