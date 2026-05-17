#ifndef MAIN_H
#define MAIN_H

#include "hystack.h"
#include <stdint.h>

/* Banner/MOTD/News Cache */
typedef struct {
    char *banner;
    char *motd;
    char *news;
    uint64_t last_reload;
} B42_TEXTCACHE;

extern B42_TEXTCACHE b42_textcache;

/* Textsystem */
void b42_text_init(void);
void b42_text_reload_if_needed(void);
void b42_text_sync_sqlite(void);

/* STRDS Hooks */
void b42_text_show_banner(int fd);
void b42_text_show_motd(int fd);
void b42_text_show_news(int fd);

#endif
