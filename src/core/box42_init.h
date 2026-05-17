#ifndef BOX42_INIT_H
#define BOX42_INIT_H

#define MAX_COMMANDS 128
#define MAX_STR 256

typedef struct {
    char name[MAX_STR];     // "#help"
    char syntax[MAX_STR];   // "#help <command>"
    char help[MAX_STR];     // "Show all available commands."
    char role[MAX_STR];     // "Users", "Admins", ...
} box42_command_t;

extern box42_command_t BOX42_COMMANDS[MAX_COMMANDS];
extern int BOX42_COMMAND_COUNT;

// INIT
int box42_load_commands_ini(const char *path);

// STRDS
const char* box42_get_help(const char *cmd);
const char* box42_get_syntax(const char *cmd);

// DISPATCHER
int box42_dispatch(const char *input);

#endif
