#ifndef HASH_MSI_COMMON_H
#define HASH_MSI_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Common definitions for MSI string hash.
 * https://nullprogram.com/blog/2022/08/08/
 *
 * This is a double hashed, open address hash table. One can easily add a
 * hashmap on top of this if the need arises.
 * For example, instead of creating a FLO_MSI_SET of string, create one of
 * {
 *  flo_String key;
 *  ValueType {
 *    ...
 *  }
 * }
 * and create a contains function that returns this struct or the value
 * directly.
 *
 * Note that this is not super
 * friendly to using arenas. Resizing is supported but using the hash-trie
 * implementation is preferred.
 *
 * Support for the following features (may need to
 * be implemented manually):
 * - Insertion / contains checks
 * - Retrieving the index of the string of the underlaying hash table.
 * - Deletion can be implemented by means of a tombstone, for example.
 *
 * The caller has to take care of the following:
 * - The hashing of the key. See hashes.h file for common hashes.
 * - Ensuring the set has enough size to add another entry into the set, this
 * includes the growth factor.
 * - How to rehash the hash set if the capacity is exceeded. For example,
 *   - By going over the values of the backing buffer
 *   - By allocating another set and rehashing the values of the old set into
 * the new set.
 */

#include "types.h"

#define FLO_MSI_SET(T)                                                         \
    struct {                                                                   \
        T *buf;                                                                \
        unsigned char exp;                                                     \
        ptrdiff_t len;                                                         \
    }

#define FOR_EACH_MSI_ELEMENT(element, msiSet)                                  \
    for (ptrdiff_t _index = 0; _index < (1 << (msiSet).exp); ++_index)         \
        if (((element) = (msiSet).buf[_index]).len == 0)                       \
            continue;                                                          \
        else

typedef struct {
    char *buf;
    unsigned char exp;
    ptrdiff_t len;
} SetSlice;

#define FLO_NEW_MSI_SET(T, exponent, perm)                                     \
    ({                                                                         \
        T newSet = (T){.exp = (exponent)};                                     \
        flo_msi_newSet(&newSet, FLO_SIZEOF(*newSet.buf),                       \
                       FLO_ALIGNOF(*newSet.buf), perm);                        \
        newSet;                                                                \
    })

// If this ever changes types because it's too small, make sure to test out that
// it works.
__attribute((unused)) static inline int32_t
flo_indexLookup(uint64_t hash, int exp, int32_t idx) {
    uint32_t mask = ((uint32_t)1 << exp) - 1;
    uint32_t step = (uint32_t)(hash >> (64 - exp)) | 1;
    return (idx + step) & mask;
}

/**
 * Written assuming that flo_Arena bumps up! Otherwise the middle case statement
 * where we only do a times 1 alloc does not hold.
 */
__attribute((unused)) static void
flo_msi_newSet(void *setSlice, ptrdiff_t size, ptrdiff_t align, flo_Arena *a) {
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

#ifdef __cplusplus
}
#endif

#endif
