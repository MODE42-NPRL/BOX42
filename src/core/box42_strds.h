/* src/core/box42_strds.h */

#ifndef BOX42_STRDS_H
#define BOX42_STRDS_H

// ------------------------------------------------------------
// HSTB42 – Hybrid Stack BOX42 (Herz, Motor, Gehirn) v1.0 (frozen)
// ------------------------------------------------------------

// Hybrid-Stack Layer Identifiers
typedef enum {
    HSTB42_LAYER_RF = 0,        // AX.25 RF Layer
    HSTB42_LAYER_SERIAL = 1,    // KISS/TNC2C Serial Layer
    HSTB42_LAYER_HYBRID = 2,    // Hybrid Conversion Layer
    HSTB42_LAYER_TCPIP = 3,     // TCP/IP Session Layer
    HSTB42_LAYER_BOX42 = 4      // BOX42 Node/BBS/User Layer
} HSTB42_LAYER;

// Hybrid-Stack Direction (100% dual)
typedef enum {
    HSTB42_DIR_RX = 0,          // Empfangspfad
    HSTB42_DIR_TX = 1           // Sendepfad
} HSTB42_DIRECTION;

// Hybrid-Stack Container (readonly, no logic)
typedef struct {
    int active;                 // 1 = aktiv, 0 = inaktiv
    HSTB42_LAYER from_layer;    // Eingangsschicht
    HSTB42_LAYER to_layer;      // Ausgangsschicht
    HSTB42_DIRECTION direction; // RX oder TX
} HSTB42_UNIT;

// Hybrid-Stack System (HSTB42)
typedef struct {
    HSTB42_UNIT rx_path;        // Empfangspfad
    HSTB42_UNIT tx_path;        // Sendepfad
} HSTB42_SYSTEM;

// Globale Hybrid-Stack-Konstante
extern const HSTB42_SYSTEM BOX42_HSTB42;


// ------------------------------------------------------------
// BOX42 Standard Definitions (STRDS) v1.2 (frozen)
// ------------------------------------------------------------

// Systemweite Limits (Hardcode-Defaults)
#define BOX42_MAX_USERS         32
#define BOX42_MAX_CHANNELS      20
#define BOX42_MAX_NODES         32
#define BOX42_MAX_CALLSIGN_LEN  12
#define BOX42_MAX_NAME_LEN      24
#define BOX42_MAX_PASS_LEN      24
#define BOX42_MAX_MSG_LEN       512
#define BOX42_MAX_PATH_LEN      128

// Rollen
typedef enum {
    ROLE_OWNER   = 0,
    ROLE_ADMINS  = 1,
    ROLE_MODS    = 2,
    ROLE_USERS   = 4,
    ROLE_GUESTS  = 5
} BOX42_ROLE;

// Node-Typen
typedef enum {
    NODE_TERM       = 0,
    NODE_TELNET4    = 1,
    NODE_TELNET6    = 2,
    NODE_TELNETAX25 = 3
} BOX42_NODE_TYPE;

// User-Datensatz
typedef struct {
    char name[BOX42_MAX_NAME_LEN];
    char callsign[BOX42_MAX_CALLSIGN_LEN];
    char password[BOX42_MAX_PASS_LEN];
    BOX42_ROLE role;
    int active;
} BOX42_USER;

// Node-Datensatz
typedef struct {
    BOX42_NODE_TYPE type;
    char path[BOX42_MAX_PATH_LEN];
    int active;
} BOX42_NODE;

// Channel-Datensatz
typedef struct {
    char name[BOX42_MAX_NAME_LEN];
    int in_use;
} BOX42_CHANNEL;

// System-Standardwerte (readonly)
typedef struct {
    int max_users;
    int max_channels;
    int max_nodes;
} BOX42_STRDS_LIMITS;

// Globale Limits
extern const BOX42_STRDS_LIMITS BOX42_LIMITS;


// ------------------------------------------------------------
// STRDS Prompt / Command System v0.1.2 (hardened, non-violent)
// ------------------------------------------------------------

struct HSTB42_SESSION;

// Initialisiert STRDS (Prompt etc.)
void strds_init(void);

// Zeigt Prompt nur im COMMAND-Mode
void strds_prompt(struct HSTB42_SESSION *s);

// Verarbeitet eine einzelne Eingabezeile
void strds_process(struct HSTB42_SESSION *s, const char *line);

// Startet eine STRDS-Session auf einem TCP-FD (von HYSTACK aufgerufen)
void strds_session_start(int fd);

#endif
