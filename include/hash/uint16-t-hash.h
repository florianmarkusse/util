#ifndef HASH_UINT16_T_HASH_H
#define HASH_UINT16_T_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "array.h"
#include "memory/arena.h"

typedef FLO_ARRAY(uint16_t) flo_uint16_a;

typedef struct {
    uint16_t value;
    uint16_t hash;
} flo_Uint16Entry;

/**
 * Hashing with linear probing for natural values > 0 up until uint_16t max
 * size.
 */
typedef struct {
    flo_Uint16Entry *array;
    ptrdiff_t arrayLen;
    ptrdiff_t entries;
} flo_Uint16HashSet;

flo_Uint16HashSet flo_initUint16HashSet(uint16_t capacity, flo_Arena *perm);

bool flo_insertUint16HashSet(flo_Uint16HashSet *set, uint16_t id,
                             flo_Arena *perm);
flo_uint16_a flo_uint16HashSetToArray(flo_Uint16HashSet *set,
                                        flo_Arena *perm);

bool flo_containsUint16HashSet(flo_Uint16HashSet *set, uint16_t id);

flo_Uint16HashSet flo_copyUint16HashSet(flo_Uint16HashSet *originalSet,
                                        flo_Arena *perm);
void flo_resetUint16HashSet(flo_Uint16HashSet *set);

typedef struct {
    flo_Uint16HashSet *set;
    ptrdiff_t index;
} flo_Uint16HashSetIterator;

uint16_t flo_nextUint16HashSetIterator(flo_Uint16HashSetIterator *iterator);

bool flo_hasNextUint16HashSetIterator(flo_Uint16HashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
