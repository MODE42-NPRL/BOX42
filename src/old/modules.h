#pragma once

typedef struct {
    void *handle;
    void (*init)(void);
    void (*tick)(void);
    void (*stop)(void);
} BOX42_Module;

BOX42_Module module_load(const char *path);
void module_unload(BOX42_Module *m);
