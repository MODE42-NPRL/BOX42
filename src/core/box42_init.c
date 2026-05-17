#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "box42_init.h"

box42_command_t BOX42_COMMANDS[MAX_COMMANDS];
int BOX42_COMMAND_COUNT = 0;

static void trim(char *s) {
    char *p = s;
    while (*p && isspace(*p)) p++;
    memmove(s, p, strlen(p) + 1);

    int len = strlen(s);
    while (len > 0 && isspace(s[len - 1])) s[--len] = 0;
}

// ------------------------------------------------------------
// INI PARSER
// ------------------------------------------------------------
int box42_load_commands_ini(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char line[512];
    char current_role[MAX_STR] = {0};
    char current_cmd[MAX_STR] = {0};

    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (line[0] == 0) continue;

        // SECTION: [Users], [Admins], [Command #help]
        if (line[0] == '[') {
            if (strncmp(line, "[Command ", 9) == 0) {
                // Extract command name
                char *start = line + 9;
                char *end = strchr(start, ']');
                if (end) {
                    *end = 0;
                    strncpy(current_cmd, start, MAX_STR);
                    trim(current_cmd);

                    // Create new command entry
                    box42_command_t *cmd = &BOX42_COMMANDS[BOX42_COMMAND_COUNT++];
                    memset(cmd, 0, sizeof(*cmd));
                    strncpy(cmd->name, current_cmd, MAX_STR);
                    strncpy(cmd->role, current_role, MAX_STR);
                }
            } else {
                // Role section
                char *start = line + 1;
                char *end = strchr(start, ']');
                if (end) {
                    *end = 0;
                    strncpy(current_role, start, MAX_STR);
                    trim(current_role);
                }
            }
            continue;
        }

        // KEY = VALUE
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = 0;
        char key[MAX_STR], val[MAX_STR];
        strncpy(key, line, MAX_STR);
        strncpy(val, eq + 1, MAX_STR);
        trim(key);
        trim(val);

        box42_command_t *cmd = &BOX42_COMMANDS[BOX42_COMMAND_COUNT - 1];

        if (strcmp(key, "syntax") == 0) {
            strncpy(cmd->syntax, val, MAX_STR);
        } else if (strcmp(key, "help") == 0) {
            strncpy(cmd->help, val, MAX_STR);
        }
    }

    fclose(f);
    return 0;
}


// ------------------------------------------------------------
// STRDS LOOKUP
// ------------------------------------------------------------
const char* box42_get_help(const char *cmd) {
    for (int i = 0; i < BOX42_COMMAND_COUNT; i++) {
        if (strcmp(BOX42_COMMANDS[i].name, cmd) == 0)
            return BOX42_COMMANDS[i].help;
    }
    return NULL;
}

const char* box42_get_syntax(const char *cmd) {
    for (int i = 0; i < BOX42_COMMAND_COUNT; i++) {
        if (strcmp(BOX42_COMMANDS[i].name, cmd) == 0)
            return BOX42_COMMANDS[i].syntax;
    }
    return NULL;
}


// ------------------------------------------------------------
// HELP API
// ------------------------------------------------------------
void box42_help_all(void) {
    printf("Available commands:\n\n");

    for (int i = 0; i < BOX42_COMMAND_COUNT; i++) {
        printf("  %-20s  %s\n",
            BOX42_COMMANDS[i].name,
            BOX42_COMMANDS[i].help
        );
    }

    printf("\nUse #cmd help <command> for details.\n");
}

void box42_help_single(const char *cmd) {
    const char *help = box42_get_help(cmd);
    const char *syntax = box42_get_syntax(cmd);

    if (!help || !syntax) {
        printf("Unknown command: %s\n", cmd);
        return;
    }

    printf("%s\n", cmd);
    printf("Syntax: %s\n", syntax);
    printf("%s\n", help);
}


// ------------------------------------------------------------
// DISPATCHER
// ------------------------------------------------------------
int box42_dispatch(const char *input) {
    for (int i = 0; i < BOX42_COMMAND_COUNT; i++) {
        const char *cmd = BOX42_COMMANDS[i].name;
        size_t len = strlen(cmd);

        // exact match or prefix match
        if (strncmp(input, cmd, len) == 0)
            return i;
    }
    return -1; // unknown command
}

