-- =========================================================
-- BOX42 SQLite Schema
-- =========================================================

PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA foreign_keys = ON;

-- =========================================================
-- USERS
-- =========================================================
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    created_at INTEGER NOT NULL,
    status TEXT NOT NULL CHECK(status IN ('pending','active','disabled')),
    last_login INTEGER,
    flags INTEGER DEFAULT 0,

    firstname TEXT,
    lastname TEXT,
    country TEXT,
    city TEXT,
    nickname TEXT,
    extra_info TEXT
);

CREATE INDEX IF NOT EXISTS idx_users_status ON users(status);


-- =========================================================
-- MAIL QUEUE
-- =========================================================
CREATE TABLE IF NOT EXISTS mail_queue (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    from_user TEXT NOT NULL,
    to_admin TEXT NOT NULL,
    created_at INTEGER NOT NULL,
    message TEXT NOT NULL,
    processed INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_mail_processed ON mail_queue(processed);


-- =========================================================
-- NEWS (inkl. MOTD)
-- id = 0 → MOTD
-- id >= 1 → News
-- =========================================================
CREATE TABLE IF NOT EXISTS news (
    id INTEGER PRIMARY KEY,
    created_at INTEGER NOT NULL,
    text TEXT NOT NULL
);


-- =========================================================
-- STATS (Counter)
-- =========================================================
CREATE TABLE IF NOT EXISTS stats (
    key TEXT PRIMARY KEY,
    value INTEGER NOT NULL
);

-- Default-Werte
INSERT OR IGNORE INTO stats (key, value) VALUES ('logins', 0);
INSERT OR IGNORE INTO stats (key, value) VALUES ('messages', 0);
INSERT OR IGNORE INTO stats (key, value) VALUES ('errors', 0);
INSERT OR IGNORE INTO stats (key, value) VALUES ('sessions', 0);
