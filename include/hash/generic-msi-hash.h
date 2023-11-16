#ifndef HASH_GENERIC_MSI_HASH_H
#define HASH_GENERIC_MSI_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
#include "hashes.h"
#include "memory/arena.h"
#include "types.h"

#define FLO_DECLARE_GENERIC_HASH_INDEX(T)                                      \
    struct {                                                                   \
        T *buf;                                                                \
        unsigned char exp;                                                     \
        ptrdiff_t len;                                                         \
    }

typedef FLO_DECLARE_GENERIC_HASH_INDEX(flo_String) flo_HashStringIndex;
#define FLO_CREATE_STRING_INDEX(exponent, perm)                                \
    (flo_HashStringIndex) {                                                    \
        .buf = FLO_NEW(perm, flo_String, (1 << (exponent)), FLO_ZERO_MEMORY),  \
        .exp = (exponent), .len = 0,                                           \
    }

#define FLO_ELEMENT_SIZE(T) (FLO_SIZEOF(T))

// If this ever changes types because it's too small, make sure to test out that
// it works.
__attribute((unused)) static inline int32_t
flo_indexLookup(uint64_t hash, int exp, int32_t idx) {
    uint32_t mask = ((uint32_t)1 << exp) - 1;
    uint32_t step = (uint32_t)(hash >> (64 - exp)) | 1;
    return (idx + step) & mask;
}

typedef struct {
    char *buf;
    unsigned char exp;
    ptrdiff_t len;
} SetSlice;

/**
 * Written assuming that flo_Arena bumps up! Otherwise the middle case statement
 * where we only do a times 1 alloc does not hold.
 */
__attribute((unused)) static void flo_newSet(void *setSlice, ptrdiff_t size,
                                             ptrdiff_t align, flo_Arena *a) {
    SetSlice *replica = (SetSlice *)setSlice;
    FLO_ASSERT(replica->exp > 0);

    if (replica->exp >= 31) {
        FLO_ASSERT(false);
        __builtin_longjmp(a->jmp_buf, 1);
    }

    ptrdiff_t cap = 1 << replica->exp;

    if (replica->buf == NULL) {
        replica->buf = flo_alloc(a, size, align, cap, FLO_ZERO_MEMORY);
    } else if (a->beg == replica->buf + size * cap) {
        memset(replica->buf, 0, size * cap);
        flo_alloc(a, size, 1, cap, FLO_ZERO_MEMORY);
        replica->exp++;
        replica->len = 0;
    } else {
        void *data = flo_alloc(a, 2 * size, align, cap, FLO_ZERO_MEMORY);
        replica->buf = data;
        replica->exp++;
        replica->len = 0;
    }
}

#define FLO_INDEX_INSERT(element, index, hashFunction)                         \
    _Generic((index).buf, flo_String *                                         \
             : FLO_INDEX_INSERT_STRING((element), (index), (hashFunction)),    \
               default                                                         \
             : false)

#define FLO_INDEX_INSERT_STRING(element, index, hashFunction)                  \
    ({                                                                         \
        bool insertSuccess = false;                                            \
        size_t hash = hashFunction(element);                                   \
        for (int32_t i = (int32_t)hash;;) {                                    \
            i = flo_indexLookup(hash, (index).exp, i);                         \
            if ((index).buf[i].len == 0) {                                     \
                if ((uint32_t)(index).len >=                                   \
                    ((uint32_t)1 << (index).exp) / 2) {                        \
                    insertSuccess = false;                                     \
                    break;                                                     \
                }                                                              \
                (index).len++;                                                 \
                (index).buf[i] = element;                                      \
                insertSuccess = true;                                          \
                break;                                                         \
            } else if (flo_stringEquals((index).buf[i], element)) {            \
                insertSuccess = true;                                          \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        insertSuccess;                                                         \
    })

#define FLO_REHASH(backingBuffer, index, hashFunction)                         \
    ({                                                                         \
        bool rehashSuccess = true;                                             \
        FLO_ASSERT((index).len == 0);                                          \
        for (int32_t i = 0; i < (backingBuffer).len; i++) {                    \
            flo_String s = (backingBuffer).buf[i];                             \
            if (!FLO_INDEX_INSERT(s, index, hashFunction)) {                   \
                rehashSuccess = false;                                         \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        rehashSuccess;                                                         \
    })

#define FLO_FULL_INSERT(element, backingBuffer, index, hashFunction, perm)     \
    do {                                                                       \
        if (!FLO_INDEX_INSERT((element), (index), (hashFunction))) {           \
            flo_newSet(&(index), FLO_SIZEOF(*(index).buf),                     \
                       FLO_ALIGNOF(*(index).buf), &(perm));                    \
            if (!FLO_REHASH((backingBuffer), (index), (hashFunction)) ||       \
                !FLO_INDEX_INSERT((element), (index), (hashFunction))) {       \
                FLO_PRINT_ERROR("Rehashing the table failed\n");               \
                __builtin_longjmp((perm).jmp_buf, 1);                          \
            }                                                                  \
        }                                                                      \
                                                                               \
        *FLO_PUSH(&(backingBuffer), &(perm)) = (element);                      \
    } while (0)

//
// bool flo_indexInsert(flo_String string, size_t hash,
//                     flo_String_HashIndex *index) {
//    for (int32_t i = (int32_t)hash;;) {
//        i = flo_indexLookup(hash, index->exp, i);
//        if (index->buf[i].len == 0) {
//            if ((uint32_t)index->len >= ((uint32_t)1 << index->exp) / 2) {
//                // Need to grow!
//                return false;
//            }
//            index->len++;
//            index->buf[i] = string;
//            return true;
//        } else if (flo_stringEquals(index->buf[i], string)) {
//            return true;
//        }
//    }
//}
//

#ifdef __cplusplus
}
#endif

#endif
