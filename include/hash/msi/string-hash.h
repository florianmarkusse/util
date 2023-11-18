#ifndef HASH_MSI_STRING_HASH_H
#define HASH_MSI_STRING_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "string.h"

typedef FLO_MSI_SET(flo_String) flo_msi_String;

bool flo_msi_insertString(flo_String string, size_t hash,
                          flo_msi_String *index);

// Below an example of rehashing with the old set and a growth factor of 0.5.
//
//
// void rehashIndex(flo_MSI_String *oldIndex,
//                  flo_MSI_String *newIndex) {
//     FLO_ASSERT(newIndex->len == 0);
//     for (int32_t i = 0; i < (1 << oldIndex->exp); i++) {
//         flo_String s = oldIndex->buf[i];
//         if (s.len > 0) {
//             flo_indexInsert(s, flo_hashString(s), newIndex);
//         }
//     }
// }
//
// flo_HashEntry indexInsert(flo_String string, flo_String_HashIndex *index,
//                           flo_Arena *perm) {
//     if ((uint32_t)index->len >= ((uint32_t)1 << index->exp) / 2) {
//         flo_String_HashIndex newIndex =
//             (flo_String_HashIndex){.exp = index->exp + 1};
//         flo_newMSISet(&newIndex, FLO_SIZEOF(*newIndex.buf),
//                       FLO_ALIGNOF(*newIndex.buf), perm);
//         rehashIndex(index, &newIndex);
//         *index = newIndex;
//     }
//     return flo_indexInsert(string, flo_hashString(string), index);
// }

#ifdef __cplusplus
}
#endif

#endif
