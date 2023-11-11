#ifndef TEXT_STRING_H
#define TEXT_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
#include <memory.h>
#include <stdbool.h>
#include <stddef.h>

#include "../assert.h"

#define FLO_EMPTY_STRING                                                       \
    (flo_String) { NULL, 0 }
#define FLO_STRING(s)                                                          \
    (flo_String) { (unsigned char *)(s), sizeof(s) - 1 }
#define FLO_STRING_LEN(s, len)                                                 \
    (flo_String) { (unsigned char *)(s), len }
#define FLO_STRING_PRINTTRS(begin, end)                                        \
    (flo_String) { (unsigned char *)(begin), ((end) - (begin)) }

#define FLO_STRING_PRINT(string) (int)(string).len, (string).buf

typedef struct {
    unsigned char *buf;
    ptrdiff_t len;
} flo_String;

__attribute__((unused)) static inline bool flo_stringEquals(flo_String a,
                                                            flo_String b) {
    return a.len == b.len && !memcmp(a.buf, b.buf, a.len);
}
__attribute__((unused)) static inline flo_String
flo_stringCopy(flo_String dest, flo_String src) {
    FLO_ASSERT(dest.len >= src.len);

    memcpy(dest.buf, src.buf, src.len);
    dest.buf[src.len] = '\0';
    dest.len = src.len;
    return dest;
}
__attribute__((unused)) static inline unsigned char
flo_getChar(flo_String str, ptrdiff_t index) {
    FLO_ASSERT(index >= 0 && index < str.len);

    return str.buf[index];
}

__attribute__((unused)) static inline unsigned char *
flo_getCharPtr(flo_String str, ptrdiff_t index) {
    FLO_ASSERT(index >= 0 && index < str.len);

    return &str.buf[index];
}
__attribute__((unused)) static inline bool flo_containsChar(flo_String s,
                                                            unsigned char ch) {
    for (ptrdiff_t i = 0; i < s.len; i++) {
        if (s.buf[i] == ch) {
            return true;
        }
    }
    return false;
}

__attribute__((unused)) static inline flo_String
flo_splitString(flo_String s, unsigned char token, ptrdiff_t from) {
    FLO_ASSERT(from >= 0 && from < s.len);

    for (ptrdiff_t i = from; i < s.len; i++) {
        if (s.buf[i] == token) {
            return (flo_String){.buf = flo_getCharPtr(s, from),
                                .len = i - from};
        }
    }

    return (flo_String){.buf = flo_getCharPtr(s, from), .len = s.len - from};
}

__attribute__((unused)) static inline ptrdiff_t
flo_firstOccurenceOfFrom(flo_String s, unsigned char ch, ptrdiff_t from) {
    FLO_ASSERT(from >= 0 && from < s.len);

    for (ptrdiff_t i = from; i < s.len; i++) {
        if (s.buf[i] == ch) {
            return i;
        }
    }
    return -1;
}
__attribute__((unused)) static inline ptrdiff_t
flo_firstOccurenceOf(flo_String s, unsigned char ch) {
    return flo_firstOccurenceOfFrom(s, ch, 0);
}

#ifdef __cplusplus
}
#endif

#endif
