#pragma once

#define MAX_USERS 64
#define NAME_LEN  16

typedef enum {
    ROLE_USER = 0,
    ROLE_MOD,
    ROLE_ADMIN,
    ROLE_OWNER
} UserRole;

typedef struct {
    char name[NAME_LEN];
    UserRole role;
} UserEntry;

void users_init();
UserRole users_get_role(const char *name);
void users_add(const char *name, UserRole role);
