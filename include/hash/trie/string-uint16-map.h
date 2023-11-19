#ifndef HASH_TRIE_STRING_UINT16_MAP_H
#define HASH_TRIE_STRING_UINT16_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common-iterator.h"
#include "hash/hashes.h"
#include "types.h"

typedef struct {
    flo_String key;
    uint16_t value;
} flo_trie_StringUint16Data;

typedef struct flo_trie_StringUint16Map {
    struct flo_trie_StringUint16Map *child[4];
    flo_trie_StringUint16Data data;
} flo_trie_StringUint16Map;

uint16_t flo_trie_insertStringUint16Map(flo_String key, uint16_t value,
                                        flo_trie_StringUint16Map **set,
                                        flo_Arena *perm);

FLO_TRIE_ITERATOR_HEADER_FILE(flo_trie_StringUint16Map,
                              flo_trie_StringUint16IterNode,
                              flo_trie_StringUint16Iterator,
                              flo_trie_StringUint16Data,
                              flo_createStringUint16Iterator,
                              flo_nextStringUint16Iterator);

#define FLO_FOR_EACH_TRIE_STRING_UINT16(element, stringUint16Map, perm)        \
    for (flo_trie_StringUint16Iterator *iter =                                 \
             flo_createStringUint16Iterator(stringUint16Map, perm);            \
         ;)                                                                    \
        if (((element) = flo_nextStringUint16Iterator(iter, perm)).value == 0) \
            break;                                                             \
        else

#ifdef __cplusplus
}
#endif

#endif