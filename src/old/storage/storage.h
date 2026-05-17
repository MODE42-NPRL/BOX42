#ifndef BOX42_STORAGE_H
#define BOX42_STORAGE_H

int storage_init(void);

/* user management */
int storage_user_add(const char *user, const char *pass, int level);
int storage_user_exists(const char *user);
int storage_user_auth(const char *user, const char *pass);
int storage_user_level(const char *user);

/* userdata */
int storage_userdata_set(const char *user, const char *key, const char *value);
const char *storage_userdata_get(const char *user, const char *key);

#endif
