CREATE TABLE mail (
    const char *sql =
    "SELECT id, sender, timestamp, flags FROM mail "
    "WHERE receiver = ? "
    "ORDER BY timestamp DESC;";
    subject TEXT,
    body TEXT NOT NULL,
    flags INTEGER DEFAULT 0
);
