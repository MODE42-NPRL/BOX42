#include "box42_strds.h"
#include "box42.h"
#include "box42_init.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// ------------------------------------------------------------
// HSTB42 System Definition
// ------------------------------------------------------------

const HSTB42_SYSTEM BOX42_HSTB42 = {
    .rx_path = { 1, HSTB42_LAYER_RF,    HSTB42_LAYER_BOX42, HSTB42_DIR_RX },
    .tx_path = { 1, HSTB42_LAYER_BOX42, HSTB42_LAYER_RF,    HSTB42_DIR_TX }
};

const BOX42_STRDS_LIMITS BOX42_LIMITS = {
    BOX42_MAX_USERS,
    BOX42_MAX_CHANNELS,
    BOX42_MAX_NODES
};


// ------------------------------------------------------------
// Internal Config Storage
// ------------------------------------------------------------

static char cfg_prompt_symbol[16] = ">";
static char cfg_welcome[64]       = "Welcome to BOX/42";


// ------------------------------------------------------------
// Config Loader
// ------------------------------------------------------------

static void ensure_config(void) {
    struct stat st;
    if (stat("bin/etc/BOX42.config", &st) == 0)
        return;

    mkdir("bin", 0755);
    mkdir("bin/etc", 0755);

    FILE *f = fopen("bin/etc/BOX42.config", "w");
    if (!f) return;

    fprintf(f,
        "[Prompt]\n"
        "symbol = >\n\n"
        "[System]\n"
        "welcome = Welcome to BOX/42\n"
    );

    fclose(f);
}

int box42_config_load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = 0;
        char *key = line;
        char *val = eq + 1;

        while (*key == ' ' || *key == '\t') key++;
        while (*val == ' ' || *val == '\t') val++;

        if (strncmp(key, "symbol", 6) == 0)
            strncpy(cfg_prompt_symbol, val, sizeof(cfg_prompt_symbol)-1);

        if (strncmp(key, "welcome", 7) == 0)
            strncpy(cfg_welcome, val, sizeof(cfg_welcome)-1);
    }

    fclose(f);
    return 1;
}

const char *box42_config_get(const char *section, const char *key) {
    if (!strcmp(section, "Prompt") && !strcmp(key, "symbol"))
        return cfg_prompt_symbol;

    if (!strcmp(section, "System") && !strcmp(key, "welcome"))
        return cfg_welcome;

    return NULL;
}


// ------------------------------------------------------------
// COMMAND DISPATCH
// ------------------------------------------------------------

int box42_handle_input(const char *input) {

    // Only commands starting with "#cmd "
    if (strncmp(input, "#cmd ", 5) == 0) {

        const char *cmd = input + 5;

        // #cmd help
        if (strcmp(cmd, "help") == 0) {
            box42_help_all();
            return 0;
        }

        // #cmd help <command>
        if (strncmp(cmd, "help ", 5) == 0) {
            const char *target = cmd + 5;
            box42_help_single(target);
            return 0;
        }

        // generic dispatcher
        int idx = box42_dispatch(cmd);
        if (idx >= 0) {
            printf("Command recognized: %s\n", BOX42_COMMANDS[idx].name);
            return 0;
        }

        printf("Unknown command.\n");
        return 0;
    }

    // Not a command → normal text
    return 1;
}


// ------------------------------------------------------------
// BOX42 Init
// ------------------------------------------------------------

int box42_init(void) {
    ensure_config();
    box42_config_load("bin/etc/BOX42.config");

    // Load commands.ini
    box42_load_commands_ini("src/etc/commands.txt");

    return 1;
}
