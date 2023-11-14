#ifndef HASH_NEW_HASH_H
#define HASH_NEW_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
// TODO: fix this.
#include "flo/html-parser/definitions.h"
#include "hashes.h"
#include "memory/arena.h"
#include "types.h"

typedef struct {
    flo_String *buf;
    unsigned char exp;
    ptrdiff_t len;
} flo_String_HashIndex;

typedef struct {
    flo_String_HashIndex index;
    flo_String_d_a array;
} flo_String_ArrayWithHashIndex;

flo_String_HashIndex flo_createIndex(unsigned char exponent, flo_Arena *perm) {
    FLO_ASSERT(exponent > 0);
    FLO_ASSERT(exponent & (exponent - 1) == 0);

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
        flo_alloc(a, size, 1, cap, FLO_ZERO_MEMORY);
        replica->exp++;
    } else {
        void *data = flo_alloc(a, 2 * size, align, cap, FLO_ZERO_MEMORY);
        replica->buf = data;
        replica->exp++;
    }
}

bool flo_indexInsert(flo_String string, flo_String_HashIndex *index) {
    size_t hash = flo_hashString(string);

    for (int32_t i = (int32_t)hash;;) {
        i = flo_indexLookup(hash, index->exp, i);
        // TODO: this fails atm on the first try because index->buf is zero
        // initialized.
        if (index->buf[i].len == 0) {
            if ((uint32_t)index->len + 1 == ((uint32_t)1 << index->exp) / 2) {
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

bool flo_hashTableRehash(flo_String_ArrayWithHashIndex *arrWithIndex) {
    FLO_ASSERT(arrWithIndex->index.len == 0);
    for (int32_t i = 0; i < arrWithIndex->array.len;) {
        flo_String s = arrWithIndex->array.buf[i];
        if (!flo_indexInsert(s, &arrWithIndex->index)) {
            return false;
        }
    }
    return true;
}

bool flo_fullInsert(flo_String string,
                    flo_String_ArrayWithHashIndex *arrWithIndex,
                    flo_Arena *perm) {
    if (!flo_indexInsert(string, &arrWithIndex->index)) {
        flo_newSet(index, FLO_SIZEOF(typeof(arrWithIndex->index.buf)),
                   FLO_ALIGNOF(typeof(arrWithIndex->index.buf)), perm);
        if (flo_hashTableRehash(arrWithIndex) ||
            !flo_indexInsert(string, &arrWithIndex->index)) {
            FLO_PRINT_ERROR("Rehashing the table failed at string %.*s\n",
                            FLO_STRING_PRINT(string));
            __builtin_longjmp(perm->jmp_buf, 1);
        }
    }

    *FLO_PUSH(&arrWithIndex->array, perm) = string;

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
