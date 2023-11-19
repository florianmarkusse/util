#ifndef HASH_MSI_UINT16_SET_H
#define HASH_MSI_UINT16_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "common.h"

typedef FLO_MSI_SET(uint16_t) flo_msi_uint16_t;

bool flo_msi_insertUint16(uint16_t value, size_t hash, flo_msi_uint16_t *index);

#ifdef __cplusplus
}
#endif

#endif
