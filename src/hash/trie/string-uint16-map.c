#include "hash/trie/string-uint16-map.h"

uint16_t flo_trie_insertStringUint16Map(flo_String key, uint16_t value,
                                        flo_trie_StringUint16Map **set,
                                        flo_Arena *perm) {
    FLO_ASSERT(key.len > 0);
    FLO_ASSERT(value != 0);
    for (uint64_t hash = flo_hashString(key); *set != NULL; hash <<= 2) {
        if (flo_stringEquals(key, (*set)->key)) {
            return (*set)->value;
        }
        set = &(*set)->child[hash >> 62];
    }
    *set = FLO_NEW(perm, flo_trie_StringUint16Map, 1, FLO_ZERO_MEMORY);
    (*set)->key = key;
    (*set)->value = value;
    return true;
}
