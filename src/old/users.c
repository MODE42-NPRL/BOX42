#include "users.h"
#include <string.h>

static UserEntry users[MAX_USERS];
static int count = 0;

void users_init() {
    count = 0;
}

void users_add(const char *name, UserRole role) {
    if (count >= MAX_USERS) return;
    strncpy(users[count].name, name, NAME_LEN);
    users[count].role = role;
    count++;
}

UserRole users_get_role(const char *name) {
    for (int i = 0; i < count; i++) {
        if (!strcmp(users[i].name, name))
            return users[i].role;
    }
    return ROLE_USER;
}
