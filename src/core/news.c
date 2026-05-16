#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "news.h"
#include "term.h"
#include "sqlite.h"
#include "session.h"

#define NEWS_TXT_PATH "./bin/etc/news.txt"
#define NEWS_TXT_DEV  "./src/etc/news.txt"

/* ---------------------------------------------------------
   Datei öffnen (Runtime → Dev-Fallback)
--------------------------------------------------------- */
static FILE *news_open_read(void)
{
    FILE *f = fopen(NEWS_TXT_PATH, "r");
    if (!f)
        f = fopen(NEWS_TXT_DEV, "r");
    return f;
}

/* ---------------------------------------------------------
   news_import_from_txt()
   news.txt → SQLite
--------------------------------------------------------- */
int news_import_from_txt(void)
{
    FILE *f = news_open_read();
    if (!f)
        return -1;

    sqlite_exec("DELETE FROM news;");

    char line[512];
    char text[4096];
    int id = -1;
    text[0] = 0;

    while (fgets(line, sizeof(line), f)) {

        if (line[0] == '[') {
            if (id >= 0)
                sqlite_news_insert(id, text);

            sscanf(line, "[%d]", &id);
            text[0] = 0;
            continue;
        }

        strncat(text, line, sizeof(text) - strlen(text) - 1);
    }

    if (id >= 0)
        sqlite_news_insert(id, text);

    fclose(f);
    return 0;
}

/* ---------------------------------------------------------
   news_export_to_txt()
   SQLite → news.txt
   MOTD (0) wird am Schluss geschrieben
--------------------------------------------------------- */
int news_export_to_txt(void)
{
    FILE *f = fopen(NEWS_TXT_PATH, "w");
    if (!f)
        return -1;

    int max = sqlite_news_max_id();
    char buf[4096];

    /* Erst alle News außer MOTD */
    for (int i = 1; i <= max; i++) {
        if (sqlite_news_get(i, buf, sizeof(buf)) == 0)
            fprintf(f, "[%d]\n%s\n", i, buf);
    }

    /* MOTD am Schluss */
    if (sqlite_news_get(0, buf, sizeof(buf)) == 0)
        fprintf(f, "[0]\n%s\n", buf);

    fclose(f);
    return 0;
}

/* ---------------------------------------------------------
   news_show_motd()
--------------------------------------------------------- */
int news_show_motd(int fd)
{
    char buf[4096];

    if (sqlite_news_get(0, buf, sizeof(buf)) != 0) {
        term_write(fd, "No MOTD.\n");
        return 0;
    }

    term_write(fd, buf);
    term_write(fd, "\n");
    return 0;
}

/* ---------------------------------------------------------
   news_show_all()
   SQLite sortiert automatisch → MOTD oben
--------------------------------------------------------- */
int news_show_all(int fd)
{
    int max = sqlite_news_max_id();
    char buf[4096];

    for (int i = 0; i <= max; i++) {
        if (sqlite_news_get(i, buf, sizeof(buf)) == 0) {
            char hdr[32];
            snprintf(hdr, sizeof(hdr), "[%d]\n", i);
            term_write(fd, hdr);
            term_write(fd, buf);
            term_write(fd, "\n");
        }
    }

    return 0;
}

/* ---------------------------------------------------------
   news_dispatch()
   #motd ist Alias für #news
--------------------------------------------------------- */
int news_dispatch(int fd, const char *line)
{
    if (strncmp(line, "#motd", 5) == 0)
        return news_show_all(fd);

    return news_show_all(fd);
}

/* ---------------------------------------------------------
   news_init()
   Wird beim Start von BOX42 aufgerufen
--------------------------------------------------------- */
int news_init(void)
{
    return news_import_from_txt();
}
