#include "encoding.h"
#include <locale.h>

void encoding_init(void)
{
    /* enforce UTF‑8 locale */
    setlocale(LC_ALL, "C.UTF-8");
}
