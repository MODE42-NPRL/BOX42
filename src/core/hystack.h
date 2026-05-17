/* hystack.h */

#ifndef HYSTACK_H
#define HYSTACK_H

// ------------------------------------------------------------
// hstb42 - Hybrid Stack
// Full Hybrid Traffic Server for BOX/42
// ------------------------------------------------------------
// The HSTB42 operates full‑duplex, parallel and asynchronous.
// All internal processing inside BOX/42 is performed strictly
// through TCP/IP (IPv4/IPv6, TCP/UDP). Every external signal,
// protocol or modem (AX.25, KISS, TNC, Internet, Packet Radio)
// is internally converted, masked and transported as TCP/IP.
// After internal processing, the traffic is restored back into
// its original protocol and returned to the respective hardware
// or software endpoint. 100% compatibility is guaranteed.
//
// NOTE:
// Only one unified port is required for both IPv4 and IPv6.
// [Ports] telnet-ports46 = 2323
// This listener is NOT a Telnet server. It is a hardened,
// generic, raw TCP stream endpoint for the HSTB42 engine.
// ------------------------------------------------------------
//
// Security Rules (hardcoded)
// ------------------------------------------------------------
// - SHA256 is the primary hashing algorithm.
// - MD5 is used only as a fallback if SHA256 is unavailable.
// - Sensitive user data is NEVER stored or displayed in plaintext.
// - Sensitive fields are masked as "****" or hashed.
// - Non-sensitive data MAY be plaintext.
// - All hashing is performed inside hystack using tinysha.
// ------------------------------------------------------------

#include <stdint.h>

// hash buffer sizes
#define HSTB42_SHA256_HEX 65
#define HSTB42_MD5_HEX    33

typedef enum {
    HSTB42_STOPPED = 0,
    HSTB42_RUNNING = 1
} HSTB42_STATE;

typedef struct {
    int listen_v4;
    int listen_v6;
    HSTB42_STATE state;
} HSTB42_CTX;

// core API
int  hystack_init(HSTB42_CTX *ctx, int port);
int  hystack_run(HSTB42_CTX *ctx);
void hystack_stop(HSTB42_CTX *ctx);

// security / hashing API
void hystack_hash_sha256(const char *input, char *output_hex);
void hystack_hash_md5(const char *input, char *output_hex);
void hystack_hash_secure(const char *input, char *output_hex);
void hystack_mask_sensitive(char *output);

#endif
