#include "storage.h"
#include "sqlite_common.h"

int storage_init(void)
{
    return sqlite_local_open("data/box42.db");
}
