/* src/core/ax25_drv.h */

#ifndef AX25_DRV_H
#define AX25_DRV_H

#include <stdint.h>
#include "ax25.h"
#include "box42_strds.h"

void ax25_drv_init(
    ax25_rx_callback_t rx_cb,
    ax25_tx_raw_callback_t kiss_tx_cb,
    ax25_tx_raw_callback_t hdlc_tx_cb
);

void ax25_drv_kiss_rx_byte(uint8_t b, int port);
void ax25_drv_hdlc_rx_byte(uint8_t b, int port);

void ax25_drv_kiss_send(const AX25_Frame *f, int port);
void ax25_drv_hdlc_send(const AX25_Frame *f, int port);

#endif
