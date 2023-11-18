#include "hash/msi/string-hash.h"

bool flo_msi_insertString(flo_String string, size_t hash,
                          flo_msi_String *index) {
    for (int32_t i = (int32_t)hash;;) {
        i = flo_indexLookup(hash, index->exp, i);
        if (index->buf[i].len == 0) {
            index->len++;
            index->buf[i] = string;
            return true;
        } else if (flo_stringEquals(index->buf[i], string)) {
            return false;
        }
    }
}
