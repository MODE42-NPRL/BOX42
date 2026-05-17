#include "textstore.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sqlite3.h>

static sqlite3 *db = NULL;

static time_t ts_banner = 0;
static time_t ts_news   = 0;

static char banner_cache[2048];
static char news_cache[2048];

static void load_file(const char *path, char *buf, int bufsz, time_t *ts) {
    struct stat st;
    if (stat(path, &st) != 0)
        return;

    if (st.st_mtime == *ts)
        return; // unverändert

    FILE *f = fopen(path, "r");
    if (!f) return;

    size_t n = fread(buf, 1, bufsz - 1, f);
    buf[n] = 0;
    fclose(f);

    *ts = st.st_mtime;
}

static void db_set(const char *name, const char *body) {
    sqlite3_stmt *st = NULL;

    const char *sql =
        "INSERT INTO texts(name, body) VALUES(?, ?) "
        "ON CONFLICT(name) DO UPDATE SET body=excluded.body;";

    if (sqlite3_prepare_v2(db, sql, -1, &st, NULL) != SQLITE_OK)
        return;

    sqlite3_bind_text(st, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(st, 2, body, -1, SQLITE_STATIC);

    sqlite3_step(st);
    sqlite3_finalize(st);
}

static void db_init(void) {
    sqlite3_open("box42.db", &db);

    const char *sql =
        "CREATE TABLE IF NOT EXISTS texts ("
        " name TEXT PRIMARY KEY,"
        " body TEXT NOT NULL"
        ");";

    sqlite3_exec(db, sql, NULL, NULL, NULL);
}

void textstore_init(void) {
    db_init();

    banner_cache[0] = 0;
    news_cache[0]   = 0;

    load_file("banner.txt", banner_cache, sizeof(banner_cache), &ts_banner);
    load_file("news.txt",   news_cache,   sizeof(news_cache),   &ts_news);

    db_set("banner", banner_cache);
    db_set("news",   news_cache);
}

void textstore_tick(void) {
    char tmp_banner[2048];
    char tmp_news[2048];

    strcpy(tmp_banner, banner_cache);
    strcpy(tmp_news,   news_cache);

    load_file("banner.txt", tmp_banner, sizeof(tmp_banner), &ts_banner);
    load_file("news.txt",   tmp_news,   sizeof(tmp_news),   &ts_news);

    if (strcmp(tmp_banner, banner_cache) != 0) {
        strcpy(banner_cache, tmp_banner);
        db_set("banner", banner_cache);
    }

    if (strcmp(tmp_news, news_cache) != 0) {
        strcpy(news_cache, tmp_news);
        db_set("news", news_cache);
    }
}

const char *textstore_get(const char *name) {
    if (!strcmp(name, "banner"))
        return banner_cache;
    if (!strcmp(name, "news"))
        return news_cache;
    return "";
}
