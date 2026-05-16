#pragma once

typedef enum {
    TNC_TYPE_TNC2C,
    TNC_TYPE_BAYCOM,
    TNC_TYPE_PCCOM
} TNC_Type;

static inline TNC_Type tnc_type_from_config(const char *s) {
    if (!strcmp(s, "tnc"))    return TNC_TYPE_TNC2C;
    if (!strcmp(s, "baycom")) return TNC_TYPE_BAYCOM;
    if (!strcmp(s, "pccom"))  return TNC_TYPE_PCCOM;
    return TNC_TYPE_TNC2C;
}
