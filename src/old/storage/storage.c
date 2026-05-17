#include "storage.h"
#include "sqlite_common.h"
#include "../etc/config.h"

#if defined(BOX42_SQLITE_REMOTE)
#include "sqlite_remote.c"
#else
#include "sqlite_local.c"
#endif
