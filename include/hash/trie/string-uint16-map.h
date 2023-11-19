#ifndef HASH_TRIE_STRING_UINT16_MAP_H
#define HASH_TRIE_STRING_UINT16_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hash/hashes.h"
#include "types.h"

typedef struct flo_trie_StringUint16Map {
    struct flo_trie_StringUint16Map *child[4];
    flo_String key;
    uint16_t value;
} flo_trie_StringUint16Map;

uint16_t flo_trie_insertStringUint16Map(flo_String key, uint16_t value,
                                        flo_trie_StringUint16Map **set,
                                        flo_Arena *perm);
#ifdef __cplusplus
}
#endif

#endif
