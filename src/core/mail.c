#include "mail.h"
#include "session.h"
#include "config.h"

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* global config */
extern BOX42_Config g_cfg;

/* ------------------------------------------------------------
   SQLite handle
------------------------------------------------------------ */
static sqlite3 *mail_db = NULL;

/* ------------------------------------------------------------
   Timestamp formatting (always 24h, no AM/PM)
------------------------------------------------------------ */
static void format_timestamp(int ts, char *out, size_t outsz)
{
    time_t t = ts;
    struct tm *tm = localtime(&t);

    if (!tm) {
        snprintf(out, outsz, "1970-01-01 00:00");
        return;
    }

    snprintf(out, outsz, "%04d-%02d-%02d %02d:%02d",
        tm->tm_year + 1900,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min
    );
}

/* ------------------------------------------------------------
   Initialize SQLite mail database
------------------------------------------------------------ */
int mail_init(const char *db_path)
{
    if (sqlite3_open(db_path, &mail_db) != SQLITE_OK)
        return -1;

    const char *sql =
        "CREATE TABLE IF NOT EXISTS mail ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " sender TEXT NOT NULL,"
        " receiver TEXT NOT NULL,"
        " timestamp INTEGER NOT NULL,"
        " subject TEXT,"
        " body TEXT NOT NULL,"
        " flags INTEGER DEFAULT 0"
        ");";

    char *err = NULL;
    if (sqlite3_exec(mail_db, sql, NULL, NULL, &err) != SQLITE_OK) {
        if (err) {
            fprintf(stderr, "mail_init SQL error: %s\n", err);
            sqlite3_free(err);
        }
        sqlite3_close(mail_db);
        mail_db = NULL;
        return -2;
    }

    return 0;
}

/* ------------------------------------------------------------
   Insert mail into SQLite
------------------------------------------------------------ */
int mail_send(const char *sender,
              const char *receiver,
              const char *subject,
              const char *body)
{
    if (!mail_db)
        return -3;

    const char *sql =
        "INSERT INTO mail (sender, receiver, timestamp, subject, body)"
        " VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(mail_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(st, 1, sender,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, receiver, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st,  3, (int)time(NULL));
    sqlite3_bind_text(st, 4, subject ? subject : "", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, body, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);

    return (rc == SQLITE_DONE) ? 0 : -2;
}

/* ------------------------------------------------------------
   List all mails for a user
------------------------------------------------------------ */
int mail_list(Session *s, const char *user)
{
    if (!mail_db)
        return -3;

    const char *sql =
        "SELECT id, sender, timestamp, flags FROM mail "
        "WHERE receiver = ? "
        "ORDER BY timestamp DESC;";

    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(mail_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(st, 1, user, -1, SQLITE_TRANSIENT);

    char line[512];
    char tsbuf[32];

    session_write(s, "Your messages:\r\n");

    while (sqlite3_step(st) == SQLITE_ROW) {
        int id = sqlite3_column_int(st, 0);
        const char *sender = (const char*)sqlite3_column_text(st, 1);
        int ts = sqlite3_column_int(st, 2);
        int flags = sqlite3_column_int(st, 3);

        if (!sender)
            sender = "(unknown)";

        format_timestamp(ts, tsbuf, sizeof(tsbuf));

        snprintf(line, sizeof(line),
            "[%d] %s from %s (%s)\r\n",
            id, tsbuf, sender, flags == 0 ? "unread" : "read"
        );
        session_write(s, line);
    }

    sqlite3_finalize(st);
    return 0;
}

/* ------------------------------------------------------------
   Read a mail
------------------------------------------------------------ */
int mail_read(Session *s, int id)
{
    if (!mail_db)
        return -3;

    const char *sql =
        "SELECT sender, timestamp, subject, body FROM mail WHERE id = ?;";

    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(mail_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_int(st, 1, id);

    if (sqlite3_step(st) != SQLITE_ROW) {
        session_write(s, "Message not found.\r\n");
        sqlite3_finalize(st);
        return -2;
    }

    const char *sender  = (const char*)sqlite3_column_text(st, 0);
    int ts              = sqlite3_column_int(st, 1);
    const char *subject = (const char*)sqlite3_column_text(st, 2);
    const char *body    = (const char*)sqlite3_column_text(st, 3);

    if (!sender)  sender  = "(unknown)";
    if (!subject) subject = "";
    if (!body)    body    = "";

    char header[512];
    char tsbuf[32];

    format_timestamp(ts, tsbuf, sizeof(tsbuf));

    snprintf(header, sizeof(header),
        "From: %s\r\nDate: %s\r\nSubject: %s\r\n\r\n",
        sender, tsbuf, subject
    );

    session_write(s, header);
    session_write(s, body);
    session_write(s, "\r\n");

    sqlite3_finalize(st);

    /* mark as read */
    const char *usql = "UPDATE mail SET flags = 1 WHERE id = ?;";
    sqlite3_stmt *ust;

    if (sqlite3_prepare_v2(mail_db, usql, -1, &ust, NULL) == SQLITE_OK) {
        sqlite3_bind_int(ust, 1, id);
        sqlite3_step(ust);
        sqlite3_finalize(ust);
    }

    return 0;
}

/* ------------------------------------------------------------
   Delete mail
------------------------------------------------------------ */
int mail_delete(Session *s, int id)
{
    if (!mail_db)
        return -3;

    const char *sql = "DELETE FROM mail WHERE id = ?;";
    sqlite3_stmt *st;

    if (sqlite3_prepare_v2(mail_db, sql, -1, &st, NULL) != SQLITE_OK) {
        session_write(s, "Delete failed.\r\n");
        return -1;
    }

    sqlite3_bind_int(st, 1, id);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);

    if (rc != SQLITE_DONE) {
        session_write(s, "Delete failed.\r\n");
        return -2;
    }

    session_write(s, "Message deleted.\r\n");
    return 0;
}

/* ------------------------------------------------------------
   Session UI: mail buffer
------------------------------------------------------------ */
#define MAIL_BUFFER_SIZE 4096

static void ensure_mail_buffer(Session *s)
{
    if (!s->mail_buffer) {
        s->mail_buffer = malloc(MAIL_BUFFER_SIZE);
        if (s->mail_buffer)
            s->mail_buffer[0] = 0;
    }
}

/* ------------------------------------------------------------
   Session UI: start mail input
------------------------------------------------------------ */
void mailbox_start(Session *s)
{
    ensure_mail_buffer(s);
    if (!s->mail_buffer)
        return;

    s->mode = SESSION_MODE_MAIL;
    s->mail_buffer[0] = 0;

    session_write(s,
        "Enter message. End with a single dot on a new/empty line.\r\n\r\n"
    );
}

/* ------------------------------------------------------------
   Session UI: process each line
------------------------------------------------------------ */
void mailbox_process_line(Session *s, const char *line)
{
    if (strcmp(line, ".") == 0) {
        mailbox_store(s);
        session_write(s, "Message sent.\r\n\r\n");
        s->mode = SESSION_MODE_COMMAND;
        return;
    }

    ensure_mail_buffer(s);
    if (!s->mail_buffer)
        return;

    size_t used = strlen(s->mail_buffer);
    size_t free = MAIL_BUFFER_SIZE - used - 2;

    if (free < 2)
        return;

    strncat(s->mail_buffer, line, free - 1);
    strcat(s->mail_buffer, "\n");
}

/* ------------------------------------------------------------
   Session UI: store mail via SQLite
------------------------------------------------------------ */
void mailbox_store(Session *s)
{
    if (!s || !s->mail_buffer)
        return;

    const char *receiver =
        (s->mail_target[0] ? s->mail_target : "admin");

    mail_send(
        s->username,
        receiver,
        "",
        s->mail_buffer
    );

    s->mail_buffer[0] = 0;
}
