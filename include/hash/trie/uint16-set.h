#ifndef HASH_TRIE_UINT16_SET_H
#define HASH_TRIE_UINT16_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common-iterator.h"
#include "hash/hashes.h"
#include "memory/arena.h"

typedef struct flo_trie_Uint16Set flo_trie_Uint16Set;
struct flo_trie_Uint16Set {
    struct flo_trie_Uint16Set *child[4];
    uint16_t data;
};

bool flo_trie_insertUint16Set(uint16_t key, flo_trie_Uint16Set **set,
                              flo_Arena *perm);

FLO_TRIE_ITERATOR_HEADER_FILE(flo_trie_Uint16Set, flo_trie_Uint16IterNode,
                              flo_trie_Uint16Iterator, uint16_t,
                              flo_createUint16Iterator, flo_nextUint16Iterator);

#define FLO_FOR_EACH_TRIE_UINT16(element, intSet, perm)                        \
    for (flo_trie_Uint16Iterator *iter =                                       \
             flo_createUint16Iterator(intSet, perm);                           \
         ;)                                                                    \
        if (((element) = flo_nextUint16Iterator(iter, perm)) == 0)             \
            break;                                                             \
        else

#ifdef __cplusplus
}
#endif

#endif
