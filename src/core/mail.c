#include "mail.h"
#include "session.h"
#include "config.h"

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ------------------------------------------------------------
   SQLite handle
------------------------------------------------------------ */
static sqlite3 *mail_db = NULL;

/* ------------------------------------------------------------
   Timestamp formatting (24h or AM/PM)
------------------------------------------------------------ */
static void format_timestamp(int ts, char *out, size_t outsz, int ampm)
{
    time_t t = ts;
    struct tm *tm = localtime(&t);

    if (!ampm) {
        /* 24h format */
        snprintf(out, outsz, "%04d-%02d-%02d %02d:%02d",
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min
        );
    } else {
        /* AM/PM format */
        int hour = tm->tm_hour;
        const char *suffix = "AM";

        if (hour == 0) {
            hour = 12;
            suffix = "AM";
        } else if (hour == 12) {
            suffix = "PM";
        } else if (hour > 12) {
            hour -= 12;
            suffix = "PM";
        }

        snprintf(out, outsz, "%04d-%02d-%02d %02d:%02d %s",
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            hour,
            tm->tm_min,
            suffix
        );
    }
}

/* ------------------------------------------------------------
   Initialize SQLite mail database
------------------------------------------------------------ */
int mail_init(const char *db_path) {
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
        sqlite3_free(err);
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
    const char *sql =
        "INSERT INTO mail (sender, receiver, timestamp, subject, body)"
        " VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(mail_db, sql, - -1, &st, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(st, 1, sender, -1, SQLITE_STATIC);
    sqlite3_bind_text(st, 2, receiver, -1, SQLITE_STATIC);
    sqlite3_bind_int(st, 3, (int)time(NULL));
    sqlite3_bind_text(st, 4, subject ? subject : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(st, 5, body, -1, SQLITE_STATIC);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);

    return (rc == SQLITE_DONE) ? 0 : -2;
}

/* ------------------------------------------------------------
   List all mails for a user
------------------------------------------------------------ */
int mail_list(Session *s, const char *user) {
    const char *sql =
        "SELECT id, sender, timestamp, flags FROM mail "
        "WHERE receiver = ? "
        "ORDER BY timestamp DESC;";

    sqlite3_stmt *st;
    if (sqlite3_prepare_v2(mail_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(st, 1, user, -1, SQLITE_STATIC);

    char line[256];
    char tsbuf[32];

    session_write(s, "Your messages:\r\n");

    while (sqlite3_step(st) == SQLITE_ROW) {
        int id = sqlite3_column_int(st, 0);
        const char *sender = (const char*)sqlite3_column_text(st, 1);
        int ts = sqlite3_column_int(st, 2);
        int flags = sqlite3_column_int(st, 3);

        format_timestamp(ts, tsbuf, sizeof(tsbuf), config.mail_ampm);

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
int mail_read(Session *s, int id) {
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

    const char *sender = (const char*)sqlite3_column_text(st, 0);
    int ts = sqlite3_column_int(st, 1);
    const char *subject = (const char*)sqlite3_column_text(st, 2);
    const char *body = (const char*)sqlite3_column_text(st, 3);

    char header[256];
    char tsbuf[32];

    format_timestamp(ts, tsbuf, sizeof(tsbuf), config.mail_ampm);

    snprintf(header, sizeof(header),
        "From: %s\r\nDate: %s\r\nSubject: %s\r\n\r\n",
        sender, tsbuf, subject
    );

    session_write(s, header);
    session_write(s, body);
    session_write(s, "\r\n");

    sqlite3_finalize(st);

    sqlite3_exec(mail_db,
        "UPDATE mail SET flags = 1 WHERE id = ?;",
        NULL, NULL, NULL);

    return 0;
}

/* ------------------------------------------------------------
   Delete mail
------------------------------------------------------------ */
int mail_delete(Session *s, int id) {
    char sql[64];
    snprintf(sql, sizeof(sql),
        "DELETE FROM mail WHERE id = %d;", id);

    sqlite3_exec(mail_db, sql, NULL, NULL, NULL);
    session_write(s, "Message deleted.\r\n");
    return 0;
}

/* ------------------------------------------------------------
   Session UI: start mail input
------------------------------------------------------------ */
#define MAIL_BUFFER_SIZE 4096

static void ensure_mail_buffer(Session *s) {
    if (!s->mail_buffer) {
        s->mail_buffer = malloc(MAIL_BUFFER_SIZE);
        if (s->mail_buffer)
            s->mail_buffer[0] = 0;
    }
}

void mailbox_start(Session *s) {
    ensure_mail_buffer(s);
    s->mode = SESSION_MODE_MAIL;
    s->mail_buffer[0] = 0;

    session_write(s,
        "Enter message. End with a single dot on a new/empty line.\r\n\r\n"
    );
}

/* ------------------------------------------------------------
   Session UI: process each line
------------------------------------------------------------ */
void mailbox_process_line(Session *s, const char *line) {
    if (strcmp(line, ".") == 0) {
        mailbox_store(s);
        session_write(s, "Message sent.\r\n\r\n");
        s->mode = SESSION_MODE_COMMAND;
        return;
    }

    ensure_mail_buffer(s);
    size_t used = strlen(s->mail_buffer);
    size_t free = MAIL_BUFFER_SIZE - used - 2;

    if (free > 0) {
        strncat(s->mail_buffer, line, free);
        strcat(s->mail_buffer, "\n");
    }
}

/* ------------------------------------------------------------
   Session UI: store mail via SQLite
------------------------------------------------------------ */
void mailbox_store(Session *s) {
    if (!s || !s->mail_buffer) return;

    const char *receiver =
        (s->mail_target[0] ? s->mail_target : "admin");

    mail_send(
        s->username,
        receiver,
        "",
        s->mail_buffer
    );

    s->mail_buffer[0] = 0;
    s->mail_target[0] = 0;
}
