#include "storage.h"
#include "sqlite.h"

int storage_init(void)
{
    return sqlite_open("data/box42.db");
}
