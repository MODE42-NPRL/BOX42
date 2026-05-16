// src/os/os.h
#ifndef BOX42_OS_H
#define BOX42_OS_H

#include <stdint.h>

struct box42_os_info {
    const char *name;
    const char *arch;
    const char *version;
};

void box42_os_init(void);
void box42_os_info(struct box42_os_info *out);
uint64_t box42_os_time_ms(void);
const char *box42_os_config_path(void);
const char *box42_os_data_path(void);

#endif
