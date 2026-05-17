#ifndef BOX42_NEWS_H
#define BOX42_NEWS_H

/* Importiert news.txt → SQLite */
int news_import_from_txt(void);

/* Exportiert SQLite → news.txt (MOTD am Schluss) */
int news_export_to_txt(void);

/* Zeigt MOTD (Eintrag 0) */
int news_show_motd(int fd);

/* Zeigt alle News (0..n) */
int news_show_all(int fd);

/* Dispatcher für #news und #motd */
int news_dispatch(int fd, const char *line);

/* Initialisierung beim Start */
int news_init(void);

#endif
