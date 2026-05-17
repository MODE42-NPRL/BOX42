#include "box42_strds.h"

// ------------------------------------------------------------
// HSTB42 – Hybrid Stack BOX42 (Herz, Motor, Gehirn)
// ------------------------------------------------------------

const HSTB42_SYSTEM BOX42_HSTB42 = {
    .rx_path = {
        .active = 1,
        .from_layer = HSTB42_LAYER_RF,
        .to_layer = HSTB42_LAYER_BOX42,
        .direction = HSTB42_DIR_RX
    },
    .tx_path = {
        .active = 1,
        .from_layer = HSTB42_LAYER_BOX42,
        .to_layer = HSTB42_LAYER_RF,
        .direction = HSTB42_DIR_TX
    }
};

// ------------------------------------------------------------
// BOX42 Standard Definitions (STRDS) v1.2
// ------------------------------------------------------------

const BOX42_STRDS_LIMITS BOX42_LIMITS = {
    .max_users    = BOX42_MAX_USERS,
    .max_channels = BOX42_MAX_CHANNELS,
    .max_nodes    = BOX42_MAX_NODES
};
#include "box42_strds.h"

// ------------------------------------------------------------
// BOX42 Standard Definitions (STRDS) v1.0
// ------------------------------------------------------------

const BOX42_STRDS_LIMITS BOX42_LIMITS = {
    .max_users    = BOX42_MAX_USERS,
    .max_channels = BOX42_MAX_CHANNELS,
    .max_nodes    = BOX42_MAX_NODES
};
