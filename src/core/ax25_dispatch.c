#include "ax25_dispatch.h"
#include "digipeater.h"
#include "transport_forward.h"

void ax25_dispatch(const AX25_Frame *f) {
    if (!f) return;

    digipeater_process(f);
    transport_forward_process(f);

    // später: Node-/Command-Handling, Routing-Table etc.
}
