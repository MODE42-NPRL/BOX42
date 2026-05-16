#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "sqlite.h"

/* Globale SQLite-DB */
extern sqlite3 *g_db;

/* ---------------------------------------------------------
   sqlite_exec()
   Führt ein SQL-Statement ohne Rückgabe aus
--------------------------------------------------------- */
int sqlite_exec(const char *sql)
{
    char *err = NULL;
    int rc = sqlite3_exec(g_db, sql, NULL, NULL, &err);

    if (rc != SQLITE_OK) {
        if (err) sqlite3_free(err);
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------
   sqlite_news_insert()
   Fügt einen News-Eintrag ein (id, text)
--------------------------------------------------------- */
int sqlite_news_insert(int id, const char *text)
{
    const char *sql =
        "INSERT INTO news (id, created_at, text) VALUES (?, strftime('%s','now'), ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, text, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

/* ---------------------------------------------------------
   sqlite_news_get()
   Holt Text eines Eintrags (id)
--------------------------------------------------------- */
int sqlite_news_get(int id, char *buf, int buflen)
{
    const char *sql = "SELECT text FROM news WHERE id = ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_int(stmt, 1, id);

    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        const unsigned char *txt = sqlite3_column_text(stmt, 0);
        if (txt) {
            strncpy(buf, (const char *)txt, buflen - 1);
            buf[buflen - 1] = 0;
        } else {
            buf[0] = 0;
        }
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return -1;
}

/* ---------------------------------------------------------
   sqlite_news_max_id()
   Liefert höchste ID (für Export)
--------------------------------------------------------- */
int sqlite_news_max_id(void)
{
    const char *sql = "SELECT MAX(id) FROM news;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return 0;

    int rc = sqlite3_step(stmt);
    int max = 0;

    if (rc == SQLITE_ROW)
        max = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return max;
}
