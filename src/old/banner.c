#include "banner.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BANNER_FILE "banner.txt"
#define BANNER_BUF  512

void banner_send(int fd) {
    if (fd < 0) return;

    FILE *f = fopen(BANNER_FILE, "r");
    if (!f) return;

    char line[BANNER_BUF];

    while (fgets(line, sizeof(line), f)) {
        write(fd, line, strlen(line));
    }

    fclose(f);
}
