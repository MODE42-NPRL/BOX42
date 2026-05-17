/* src/core/main.c */

#include "main.h"
#include "hystack.h"
#include "box42_strds.h"
#include "config.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

static HSTB42_CTX g_ctx;

/* ------------------------------------------------------------
   TEXTCACHE
------------------------------------------------------------ */
B42_TEXTCACHE b42_textcache = {0};

/* Datei lesen */
static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return strdup("");

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = 0;

    fclose(f);
    return buf;
}

/* ------------------------------------------------------------
   SQLite Sync
------------------------------------------------------------ */
void b42_text_sync_sqlite(void)
{
    sqlite3 *db = NULL;
    if (sqlite3_open("BOX42.db", &db) != SQLITE_OK)
        return;

    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS texts ("
        " key TEXT PRIMARY KEY,"
        " value TEXT"
        ");",
        NULL, NULL, NULL);

    sqlite3_exec(db, "DELETE FROM texts;", NULL, NULL, NULL);

    sqlite3_stmt *st = NULL;
    sqlite3_prepare_v2(db,
        "INSERT INTO texts (key,value) VALUES (?,?);",
        -1, &st, NULL);

    const char *keys[3] = {"banner", "motd", "news"};
    const char *vals[3] = {b42_textcache.banner,
                           b42_textcache.motd,
                           b42_textcache.news};

    for (int i = 0; i < 3; i++) {
        sqlite3_reset(st);
        sqlite3_bind_text(st, 1, keys[i], -1, SQLITE_STATIC);
        sqlite3_bind_text(st, 2, vals[i], -1, SQLITE_STATIC);
        sqlite3_step(st);
    }

    sqlite3_finalize(st);
    sqlite3_close(db);
}

/* ------------------------------------------------------------
   Reload alle 60 Sekunden
------------------------------------------------------------ */
void b42_text_reload_if_needed(void)
{
    uint64_t now = time(NULL);
    if (now - b42_textcache.last_reload < 60)
        return;

    free(b42_textcache.banner);
    free(b42_textcache.motd);
    free(b42_textcache.news);

    /* Du willst: etc/... */
    b42_textcache.banner = read_file("etc/banner.txt");
    b42_textcache.motd   = read_file("etc/motd.txt");
    b42_textcache.news   = read_file("etc/news.txt");

    b42_textcache.last_reload = now;

    b42_text_sync_sqlite();
}

/* ------------------------------------------------------------
   Init
------------------------------------------------------------ */
void b42_text_init(void)
{
    b42_textcache.banner = strdup("");
    b42_textcache.motd   = strdup("");
    b42_textcache.news   = strdup("");
    b42_textcache.last_reload = 0;

    b42_text_reload_if_needed();
}

/* ------------------------------------------------------------
   STRDS Hooks
------------------------------------------------------------ */
void b42_text_show_banner(int fd)
{
    b42_text_reload_if_needed();
    strds_write(fd, b42_textcache.banner);
    strds_write(fd, "\n");
}

void b42_text_show_motd(int fd)
{
    b42_text_reload_if_needed();
    strds_write(fd, b42_textcache.motd);
    strds_write(fd, "\n");
}

void b42_text_show_news(int fd)
{
    b42_text_reload_if_needed();
    strds_write(fd, b42_textcache.news);
    strds_write(fd, "\n");
}

/* ------------------------------------------------------------
   CTRL+C Handler
------------------------------------------------------------ */
static void sig_handler(int sig)
{
    (void)sig;
    hystack_stop(&g_ctx);
}

/* ------------------------------------------------------------
   MAIN
------------------------------------------------------------ */
int main(void)
{
    int port = 2323;

    printf("BOX/42 HYSTACK starting on port %d...\n", port);

    /* Konfiguration laden */
    config_load("BOX42.conf");
    config_apply();

    /* STRDS initialisieren */
    strds_init();

    /* Banner/MOTD/News initialisieren */
    b42_text_init();

    /* HYSTACK starten */
    if (hystack_init(&g_ctx, port) != 0) {
        fprintf(stderr, "ERROR: hystack_init() failed\n");
        return 1;
    }

    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    printf("HYSTACK running. Connect via: telnet 127.0.0.1:%d\n", port);

    hystack_run(&g_ctx);
    hystack_stop(&g_ctx);

    printf("HYSTACK stopped.\n");
    return 0;
}

