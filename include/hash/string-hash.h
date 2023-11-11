#ifndef HASH_STRING_HASH_H
#define HASH_STRING_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/definitions.h"
#include "hash-comparison-status.h"
#include "memory.h"
#include "text/string.h"

/**
 * Hashing with linear probing for natural values > 0 up until ptrdiff_t max
 * size.
 */

typedef struct {
    ptrdiff_t entryIndex;
    bool wasInserted;
} flo_StringInsert;

typedef struct {
    ptrdiff_t entryIndex;
    size_t hash;
    flo_String string;
} flo_StringHashEntry;

typedef struct {
    flo_StringHashEntry *array;
    ptrdiff_t arrayLen;
    ptrdiff_t entries;
} flo_StringHashSet;

flo_StringHashSet flo_initStringHashSet(ptrdiff_t capacity, flo_Arena *perm);

flo_StringInsert flo_insertStringHashSet(flo_StringHashSet *set,
                                         flo_String string, flo_Arena *perm);

ptrdiff_t flo_containsStringHashSet(flo_StringHashSet *set, flo_String string);

flo_StringHashSet flo_copyStringHashSet(flo_StringHashSet *originalSet,
                                        flo_Arena *perm);

flo_HashComparisonStatus flo_equalsStringHashSet(flo_StringHashSet *set1,
                                                 flo_StringHashSet *set2);

typedef struct {
    flo_StringHashSet *set;
    ptrdiff_t index;
} flo_StringHashSetIterator;

flo_String flo_nextStringHashSetIterator(flo_StringHashSetIterator *iterator);

bool flo_hasNextStringHashSetIterator(flo_StringHashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
