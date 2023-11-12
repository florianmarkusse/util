#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "error.h"
#include "macros.h"

typedef struct {
    char *beg;
    char *end;
    ptrdiff_t cap;
    void **jmp_buf;
} flo_Arena;

/*
 * Set up the arena values, except for the jmp_buf!
 */
__attribute((unused)) static inline flo_Arena flo_createArena(char *start,
                                                              ptrdiff_t cap) {
    flo_Arena result;

    result.beg = start;
    result.end = start + cap;
    result.cap = cap;

    return result;
}

__attribute((unused, malloc, alloc_size(2, 4),
             alloc_align(3))) static inline void *
flo_alloc(flo_Arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count,
          unsigned char flags) {
    FLO_ASSERT(align > 0);
    FLO_ASSERT((align & (align - 1)) == 0);

    ptrdiff_t avail = a->end - a->beg;
    ptrdiff_t padding = -(uintptr_t)a->beg & (align - 1);
    if (count > (avail - padding) / size) {
        FLO_ASSERT(false);
        if (flags & FLO_NULL_ON_FAIL) {
            return NULL;
        }
        __builtin_longjmp(a->jmp_buf, 1);
    }
    ptrdiff_t total = size * count;
    char *p = a->beg + padding;
    a->beg += padding + total;

    return flags & FLO_ZERO_MEMORY ? memset(p, 0, total) : p;
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

#endif
