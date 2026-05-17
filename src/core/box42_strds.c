/* src/core/box42_strds.c
 *
 * STRDS: Stream-Driven Session Layer
 * - Terminal/Environment detection
 * - Universal input normalizer
 * - AX.25 <-> Session mapping
 * - Telnet session loop
 * - STRICT MODE: only "#cmd ..." is accepted
 */

#include "box42_os.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "session.h"
#include "box42_strds.h"
#include "hystack.h"   /* für AX.25 TX */

/* ------------------------------------------------------------
   SQLite Integration
------------------------------------------------------------ */

static sqlite3 *g_db = NULL;

sqlite3 *box42_db(void)
{
    return g_db;
}

static void box42_sql_exec(const char *sql)
{
    char *err = NULL;
    if (sqlite3_exec(g_db, sql, 0, 0, &err) != SQLITE_OK) {
        fprintf(stderr, "[SQLITE] ERROR: %s\n", err);
        sqlite3_free(err);
    }
}

static void box42_sql_create_tables(void)
{
    box42_sql_exec(
        "CREATE TABLE IF NOT EXISTS users ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " username TEXT UNIQUE NOT NULL,"
        " password_hash TEXT NOT NULL,"
        " created_at INTEGER NOT NULL,"
        " status TEXT NOT NULL,"
        " last_login INTEGER,"
        " flags INTEGER DEFAULT 0"
        ");"
    );

    box42_sql_exec(
        "CREATE TABLE IF NOT EXISTS mail_queue ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " from_user TEXT NOT NULL,"
        " to_admin TEXT NOT NULL,"
        " created_at INTEGER NOT NULL,"
        " message TEXT NOT NULL,"
        " processed INTEGER DEFAULT 0"
        ");"
    );
}

void box42_sqlite_init(const char *path)
{
    if (sqlite3_open(path, &g_db) != SQLITE_OK) {
        fprintf(stderr, "[SQLITE] Cannot open DB at %s\n", path);
        exit(1);
    }

    box42_sql_create_tables();

    printf("[SQLITE] DB ready: %s\n", path);
}

/* extern Command-Dispatcher */
extern void box42_cmd_dispatch(Session *s, const char *cmd);

/* ------------------------------------------------------------
   STRDS Terminal Environment Detection (tmux/screen)
------------------------------------------------------------ */

int box42_strds_is_tmux(void)
{
    const char *tmux = getenv("TMUX");
    return (tmux && tmux[0] != '\0');
}

int box42_strds_is_screen(void)
{
    const char *term   = getenv("TERM");
    const char *window = getenv("WINDOW");

    if (window && window[0] != '\0')
        return 1;

    if (term && strstr(term, "screen"))
        return 1;

    return 0;
}


/* ------------------------------------------------------------
   UNIVERSAL INPUT NORMALIZER
   Fix für:
   - Windows Telnet
   - DOS/Win95 Telnet.exe
   - PuTTY RAW/Telnet
   - Amiga-Terminals
   - CR, LF, CRLF, LFCR, CRNUL
   - Backspace 0x08, 0x7F
   - Telnet IAC
------------------------------------------------------------ */
int strds_normalize_input(uint8_t *buf, int len, char *out, int outsz)
{
    int o = 0;

    for (int i = 0; i < len; i++) {
        uint8_t c = buf[i];

        /* Telnet IAC (Interpret As Command) */
        if (c == 255) {
            if (i + 2 < len)
                i += 2;
            continue;
        }

        /* CRLF, CRNUL, LFCR → '\n' */
        if (c == '\r') {
            if (i + 1 < len && (buf[i+1] == '\n' || buf[i+1] == 0x00))
                i++;
            if (o < outsz - 1)
                out[o++] = '\n';
            continue;
        }

        if (c == '\n') {
            if (o < outsz - 1)
                out[o++] = '\n';
            continue;
        }

        /* Backspace / Delete */
        if (c == 0x08 || c == 0x7F) {
            if (o > 0) o--;
            continue;
        }

        /* Printable ASCII */
        if (c >= 32 && c <= 126) {
            if (o < outsz - 1)
                out[o++] = (char)c;
            continue;
        }
    }

    out[o] = 0;
    return o;
}


/* ------------------------------------------------------------
   Init
------------------------------------------------------------ */
void strds_init(void)
{
    box42_sqlite_init("./data/users.db");

    /* Rest deiner STRDS-Initialisierung */
}

/* ------------------------------------------------------------
   Prompt (optional, nur im COMMAND-Mode)
------------------------------------------------------------ */
static const char *prompt_symbol = "> ";

static void strds_prompt(Session *s)
{
    if (!s) return;
    if (s->mode == SESSION_MODE_COMMAND)
        session_write(s, prompt_symbol);
}


/* ------------------------------------------------------------
   STRICT MODE Line processing
   - Nur Zeilen, die mit '#' beginnen, werden betrachtet
   - Nur "#cmd ..." ist gültig
   - Alles andere wird verworfen (kein Echo, keine Antwort)
------------------------------------------------------------ */
static void strds_process(Session *s, const char *line)
{
    if (!s || !line) return;

    /* STRICT MODE: nur '#' akzeptieren */
    if (line[0] != '#')
        return;

    /* Nur '#cmd ' ist gültig */
    if (strncmp(line, "#cmd ", 5) != 0)
        return;

    /* Command extrahieren */
    const char *cmd = line + 5;
    while (*cmd == ' ')
        cmd++;

    if (*cmd == 0)
        return;

    /* Dispatcher */
    box42_cmd_dispatch(s, cmd);
}


/* ------------------------------------------------------------
   AX.25 Session Mapping
------------------------------------------------------------ */

typedef struct {
    char     callsign[16];
    Session *s;
} AX25_MAP;

static AX25_MAP ax25_sessions[32];

static Session *ax25_find(const char *cs)
{
    for (int i = 0; i < 32; i++) {
        if (ax25_sessions[i].s &&
            strcasecmp(ax25_sessions[i].callsign, cs) == 0)
            return ax25_sessions[i].s;
    }
    return NULL;
}

static Session *ax25_create(const char *cs)
{
    for (int i = 0; i < 32; i++) {
        if (!ax25_sessions[i].s) {
            Session *s = session_create(-1, cs, 0);
            if (!s) return NULL;

            strncpy(ax25_sessions[i].callsign, cs, 15);
            ax25_sessions[i].callsign[15] = 0;
            ax25_sessions[i].s = s;

            s->mode   = SESSION_MODE_COMMAND;
            s->active = 1;

            /* Banner/MOTD für AX.25 (optional) */
            extern void b42_text_show_banner(int fd);
            extern void b42_text_show_motd(int fd);

            b42_text_show_banner(s->fd);
            b42_text_show_motd(s->fd);

            return s;
        }
    }
    return NULL;
}


/* ------------------------------------------------------------
   AX.25 Session Helper
------------------------------------------------------------ */
Session *strds_ax25_get_or_create(const char *cs)
{
    Session *s = ax25_find(cs);
    if (s)
        return s;
    return ax25_create(cs);
}


/* ------------------------------------------------------------
   AX.25 → STRDS Input
------------------------------------------------------------ */
void strds_ax25_input(const char *callsign, const uint8_t *data, int len)
{
    Session *s = strds_ax25_get_or_create(callsign);
    if (!s) return;

    char norm[512];
    int  n = strds_normalize_input((uint8_t *)data, len, norm, sizeof(norm));
    if (n <= 0) return;

    char *line = strtok(norm, "\n");
    while (line) {
        strds_process(s, line);
        line = strtok(NULL, "\n");
    }
}


/* ------------------------------------------------------------
   STRDS → AX.25 Output
------------------------------------------------------------ */
void strds_ax25_output(Session *s, const char *text)
{
    if (!s || !text) return;

    hystack_ax25_send_text(s->username, text);
}


/* ------------------------------------------------------------
   Session start (Telnet)
------------------------------------------------------------ */
void strds_session_start(int fd)
{
    Session *s = session_create(fd, "guest", 0);
    if (!s) return;

    s->mode   = SESSION_MODE_COMMAND;
    s->active = 1;

    strds_prompt(s);

    uint8_t raw[512];
    char    norm[512];

    while (s->active) {
        int n = read(fd, raw, sizeof(raw));
        if (n <= 0) {
            s->active = 0;
            break;
        }

        /* UNIVERSAL TELNET / TERMINAL FIX */
        int m = strds_normalize_input(raw, n, norm, sizeof(norm));
        if (m > 0) {
            char *line = strtok(norm, "\n");
            while (line) {
                strds_process(s, line);
                line = strtok(NULL, "\n");
            }
        }

        if (s->active)
            strds_prompt(s);
    }

    session_destroy(fd);
}


/* ------------------------------------------------------------
   Low-level write helper
------------------------------------------------------------ */
void strds_write(int fd, const char *text)
{
    if (!text) return;
    write(fd, text, strlen(text));
}
