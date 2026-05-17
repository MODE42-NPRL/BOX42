// core/modules.c
#include "modules.h"
#include <dlfcn.h>
#include <stdio.h>

BOX42_Module box42_module_load(const char *path) {
    BOX42_Module m = {0};

    m.handle = dlopen(path, RTLD_NOW);
    if (!m.handle) {
        printf("[BOX42] module load error: %s\n", dlerror());
        return m;
    }

    m.init = dlsym(m.handle, "box42_init");
    m.tick = dlsym(m.handle, "box42_tick");
    m.stop = dlsym(m.handle, "box42_stop");

    if (m.init) m.init();
    return m;
}

void box42_module_unload(BOX42_Module *m) {
    if (!m || !m->handle) return;

    if (m->stop) m->stop();
    dlclose(m->handle);

    m->handle = NULL;
    m->init = NULL;
    m->tick = NULL;
    m->stop = NULL;
}
