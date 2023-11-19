#ifndef HASH_TRIE_COMMON_ITERATOR_H
#define HASH_TRIE_COMMON_ITERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hash/hashes.h"
#include "types.h"

#define FLO_TRIE_ITER_NODE(T, iterNodeName)                                    \
    typedef struct iterNodeName iterNodeName;                                  \
    struct iterNodeName {                                                      \
        /* NOLINTNEXTLINE */                                                   \
        iterNodeName *next;                                                    \
        /* NOLINTNEXTLINE */                                                   \
        T *set;                                                                \
        unsigned char index;                                                   \
    };

#define FLO_TRIE_ITERATOR(iterNodeType, iteratorName)                          \
    typedef struct {                                                           \
        /* NOLINTNEXTLINE */                                                   \
        iterNodeType *head;                                                    \
        /* NOLINTNEXTLINE */                                                   \
        iterNodeType *free;                                                    \
    }(iteratorName);

#define FLO_TRIE_ITERATOR_HEADER_FILE(setType, iterNodeType, iteratorType,     \
                                      returnType, createIteratorFunctionName,  \
                                      nextIteratorFunctionName)                \
    FLO_TRIE_ITER_NODE(setType, iterNodeType);                                 \
    FLO_TRIE_ITERATOR(iterNodeType, iteratorType);                             \
    /* NOLINTNEXTLINE */                                                       \
    iteratorType *createIteratorFunctionName(/* NOLINTNEXTLINE */              \
                                             setType *set, flo_Arena *perm);   \
    returnType /* NOLINTNEXTLINE */                                            \
    nextIteratorFunctionName(iteratorType *it, flo_Arena *perm);

#define FLO_TRIE_NEW_ITERATOR(stringSetType, iteratorType, iterNodeType,       \
                              functionName)                                    \
    /* NOLINTNEXTLINE */                                                       \
    iteratorType *functionName(/* NOLINTNEXTLINE */                            \
                               stringSetType *set, flo_Arena *perm) {          \
        /* NOLINTNEXTLINE */                                                   \
        iteratorType *it = FLO_NEW(perm, iteratorType, 1, FLO_ZERO_MEMORY);    \
        if (set != NULL) {                                                     \
            it->head = FLO_NEW(perm, iterNodeType, 1, FLO_ZERO_MEMORY);        \
            it->head->set = set;                                               \
        }                                                                      \
        return it;                                                             \
    }

#define FLO_TRIE_NEXT_ITERATOR(returnType, iteratorType, iterNodeType,         \
                               functionName)                                   \
    /* NOLINTNEXTLINE */                                                       \
    returnType functionName(iteratorType *it, flo_Arena *perm) {               \
        while (it->head) {                                                     \
            int index = it->head->index++;                                     \
            if (index == 0) {                                                  \
                return it->head->set->data;                                    \
            } else if (index == 5) {                                           \
                /* NOLINTNEXTLINE */                                           \
                iterNodeType *dead = it->head;                                 \
                it->head = dead->next;                                         \
                dead->next = it->free;                                         \
                it->free = dead;                                               \
            } else if (it->head->set->child[index - 1]) {                      \
                /* NOLINTNEXTLINE */                                           \
                iterNodeType *nextIter = it->free;                             \
                if (nextIter != NULL) {                                        \
                    it->free = it->free->next;                                 \
                    nextIter->index = 0;                                       \
                } else {                                                       \
                    nextIter =                                                 \
                        FLO_NEW(perm, iterNodeType, 1, FLO_ZERO_MEMORY);       \
                }                                                              \
                nextIter->set = it->head->set->child[index - 1];               \
                nextIter->next = it->head;                                     \
                it->head = nextIter;                                           \
            }                                                                  \
        }                                                                      \
        return (returnType){0};                                                \
    }

#define FLO_TRIE_ITERATOR_SOURCE_FILE(setType, iterNodeType, iteratorType,     \
                                      returnType, createIteratorFunctionName,  \
                                      nextIteratorFunctionName)                \
    FLO_TRIE_NEW_ITERATOR(setType, iteratorType, iterNodeType,                 \
                          createIteratorFunctionName)                          \
    FLO_TRIE_NEXT_ITERATOR(returnType, iteratorType, iterNodeType,             \
                           nextIteratorFunctionName)

#ifdef __cplusplus
}
#endif

#endif
