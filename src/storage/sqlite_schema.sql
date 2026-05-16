CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT,
    level INTEGER NOT NULL DEFAULT 1,
    created_at INTEGER NOT NULL,
    last_login INTEGER,
    status TEXT NOT NULL DEFAULT 'active'
);

CREATE TABLE settings (
    key TEXT PRIMARY KEY,
    value TEXT
);

CREATE TABLE sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT,
    level INTEGER,
    login_time INTEGER,
    last_active INTEGER
);

CREATE TABLE userdata (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL,
    key TEXT NOT NULL,
    value TEXT,
    UNIQUE(username, key)
);
