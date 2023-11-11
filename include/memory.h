#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "error.h"

typedef struct {
    char *beg;
    char *end;
    ptrdiff_t cap;
    void **jmp_buf;
} flo_Arena;

#define FLO_ZERO_MEMORY 0x01
#define FLO_NULL_ON_FAIL 0x02

__attribute((unused, malloc, alloc_size(2, 4),
             alloc_align(3))) static inline void *
flo_alloc(flo_Arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count,
          unsigned char flags) {
    FLO_ASSERT(align > 0);
    FLO_ASSERT((align & (align - 1)) == 0);

    ptrdiff_t total = size * count;

    char *newEnd = a->end;
    newEnd -= total;
    newEnd -= (uintptr_t)a->end & !(align - 1); // fix alignment.

    if (newEnd < a->beg) {
        FLO_ASSERT(false);
        if (flags & FLO_NULL_ON_FAIL) {
            return NULL;
        }
        __builtin_longjmp(a->jmp_buf, 1);
    }

    a->end = newEnd;

    return flags & FLO_ZERO_MEMORY ? memset(a->end, 0, total) : a->end;
}

// TODO: Create macro?? Or devise solution to handle perm & scratch better.
__attribute((unused)) static void *flo_copyToArena(flo_Arena *arena, void *data,
                                                   ptrdiff_t size,
                                                   ptrdiff_t align,
                                                   ptrdiff_t count) {
    unsigned char *copy = flo_alloc(arena, size, align, count, 0);
    memcpy(copy, data, size * count);
    return copy;
}

#define FLO_STRINGIZEOF(x) (ptrdiff_t)sizeof(x)
#define FLO_COUNTOF(a) (FLO_STRINGIZEOF(a) / FLO_STRINGIZEOF(*(a)))
#define FLO_LENGTHOF(s) (FLO_COUNTOF(s) - 1)
#define FLO_ALIGNOF(t) (_Alignof(t))

#define FLO_NEW_2(a, t)                                                        \
    (t *)flo_alloc(a, FLO_STRINGIZEOF(t), FLO_ALIGNOF(t), 1, 0)
#define FLO_NEW_3(a, t, n)                                                     \
    (t *)flo_alloc(a, FLO_STRINGIZEOF(t), FLO_ALIGNOF(t), n, 0)
#define FLO_NEW_4(a, t, n, f)                                                  \
    (t *)flo_alloc(a, FLO_STRINGIZEOF(t), FLO_ALIGNOF(t), n, f)
#define FLO_NEW_X(a, b, c, d, e, ...) e
#define FLO_NEW(...)                                                           \
    FLO_NEW_X(__VA_ARGS__, FLO_NEW_4, FLO_NEW_3, FLO_NEW_2)                    \
    (__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
