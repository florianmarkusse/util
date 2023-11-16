#ifndef HASH_NEW_HASH_H
#define HASH_NEW_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
#include "hashes.h"
#include "memory/arena.h"
#include "types.h"

typedef struct {
    flo_String *buf;
    unsigned char exp;
    ptrdiff_t len;
} flo_String_HashIndex;

flo_String_HashIndex flo_createIndex(unsigned char exponent, flo_Arena *perm) {
    FLO_ASSERT(exponent > 0);
    FLO_ASSERT((exponent & (exponent - 1)) == 0);

    flo_String *buffer =
        FLO_NEW(perm, flo_String, 1 << exponent, FLO_ZERO_MEMORY);

    return (flo_String_HashIndex){.buf = buffer, .exp = exponent, .len = 0};
}

// If this ever changes types because it's too small, make sure to test out that
// it works.
int32_t flo_indexLookup(uint64_t hash, int exp, int32_t idx) {
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

bool flo_indexInsert(flo_String string, size_t hash,
                     flo_String_HashIndex *index) {
    for (int32_t i = (int32_t)hash;;) {
        i = flo_indexLookup(hash, index->exp, i);
        if (index->buf[i].len == 0) {
            if ((uint32_t)index->len >= ((uint32_t)1 << index->exp) / 2) {
                // Need to grow!
                return false;
            }
            index->len++;
            index->buf[i] = string;
            return true;
        } else if (flo_stringEquals(index->buf[i], string)) {
            return true;
        }
    }
}

bool flo_hashTableRehash(flo_String_d_a *backingBuffer,
                         flo_String_HashIndex *index) {
    FLO_ASSERT(index->len == 0);
    for (int32_t i = 0; i < backingBuffer->len; i++) {
        flo_String s = backingBuffer->buf[i];
        if (!flo_indexInsert(s, flo_hashString(s), index)) {
            return false;
        }
    }
    return true;
}

void flo_testFullInsert(flo_String string, flo_String_d_a *buffer,
                        flo_String_HashIndex *index, flo_Arena *perm) {
    FLO_PRINT_ERROR("at start inserting %.*s\n", FLO_STRING_PRINT(string));
    if (!flo_indexInsert(string, flo_hashString(string), index)) {
        FLO_PRINT_ERROR("resizing\n");
        flo_newSet(index, FLO_SIZEOF(*index->buf), FLO_ALIGNOF(*index->buf),
                   perm);
        FLO_PRINT_ERROR("in here\n");
        if (!flo_hashTableRehash(buffer, index) ||
            !flo_indexInsert(string, flo_hashString(string), index)) {
            FLO_PRINT_ERROR("Rehashing the table failed\n");
            __builtin_longjmp((perm)->jmp_buf, 1);
        }
        FLO_PRINT_ERROR("after\n");
    }

    for (ptrdiff_t i = 0; i < (1 << index->exp); i++) {
        if (index->buf[i].len != 0) {
            FLO_PRINT_ERROR("INNNNInside index is: %.*s at index %td\n",
                            FLO_STRING_PRINT(index->buf[i]), i);
        } else {
            FLO_PRINT_ERROR("Nothing at index %td\n", i);
        }
    }

    FLO_PRINT_ERROR("pushing\n");
    *FLO_PUSH(buffer, perm) = string;
}

#define FLO_REHASH(backingBuffer, index, hashFunction)                         \
    ({                                                                         \
        bool rehashSuccess = true;                                             \
        FLO_ASSERT((index)->len == 0);                                         \
        for (int32_t i = 0; i < (backingBuffer)->len; i++) {                   \
            flo_String s = (backingBuffer)->buf[i];                            \
            if (!flo_indexInsert(s, hashFunction(s), (index))) {               \
                rehashSuccess = false;                                         \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        rehashSuccess;                                                         \
    })

#define FLO_FULL_INSERT(string, backingBuffer, index, hashFunction, perm)      \
    if (!flo_indexInsert(string, hashFunction(string), index)) {               \
        flo_newSet(index, FLO_SIZEOF(*(index)->buf),                           \
                   FLO_ALIGNOF(*(index)->buf), perm);                          \
        if (!FLO_REHASH(backingBuffer, index, hashFunction) ||                 \
            !flo_indexInsert(string, hashFunction(string), index)) {           \
            FLO_PRINT_ERROR("Rehashing the table failed\n");                   \
            __builtin_longjmp((perm)->jmp_buf, 1);                             \
        }                                                                      \
    }                                                                          \
                                                                               \
    *FLO_PUSH(backingBuffer, perm) = string;

#ifdef __cplusplus
}
#endif

#endif
