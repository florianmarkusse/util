#include <string.h>

#include "hash/hash-constants.h"
#include "hash/hashes.h"
#include "hash/uint16-t-hash.h"
#include "memory/arena.h"

#define MAX_CAPACITY ((1U << 16U) - 1) // Maximum capacity for uint16_t

flo_Uint16HashSet flo_initUint16HashSet(uint16_t capacity, flo_Arena *perm) {
    return (flo_Uint16HashSet){
        .array = FLO_NEW(perm, flo_Uint16Entry, capacity, FLO_ZERO_MEMORY),
        .arrayLen = capacity,
        .entries = 0,
    };
}

bool flo_insertUint16HashSet(flo_Uint16HashSet *set, uint16_t id,
                             flo_Arena *perm) {
    uint16_t newIntHash = flo_hash16_xm3(id); // Calculate the hash once
    ptrdiff_t newIntProbes = 0;

    while (set->array[(newIntHash + newIntProbes) % set->arrayLen].value != 0) {
        if (set->array[(newIntHash + newIntProbes) % set->arrayLen].value ==
            id) {
            return true;
        }
        newIntProbes++;
    }

    bool didResize = false;
    if ((double)set->entries >= (double)set->arrayLen * FLO_GROWTH_FACTOR) {
        didResize = true;
        // See if it makes sense to grow.
        if ((double)set->arrayLen >= MAX_CAPACITY * 0.9) {
            FLO_ASSERT(false);
            FLO_PRINT_ERROR(
                "Hash set capacity would exceed the maximum capacity "
                "for uint16_t!\n");
            __builtin_longjmp(perm->jmp_buf, 1);
        }

        ptrdiff_t newCapacity = (set->arrayLen * 2 <= MAX_CAPACITY)
                                    ? set->arrayLen * 2
                                    : MAX_CAPACITY;
        flo_Uint16Entry *oldArray = set->array;
        set->array =
            FLO_NEW(perm, flo_Uint16Entry, newCapacity, FLO_ZERO_MEMORY);

        // Rehashing.
        for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
            ptrdiff_t probes = 0;
            ptrdiff_t hash = oldArray[i].hash;
            if (set->array[(hash + probes) % newCapacity].value != 0) {
                ptrdiff_t newIndex = oldArray[i].hash % newCapacity;
                while (set->array[newIndex].value != 0) {
                    probes++;
                }

                ptrdiff_t finalIndex = (hash + probes) % newCapacity;

                set->array[finalIndex].value = oldArray[i].value;
                set->array[finalIndex].hash = oldArray[i].hash;
            }
        }

        set->arrayLen = newCapacity;
    }

    if (didResize) {
        newIntProbes = 0;
        while (set->array[(newIntHash + newIntProbes) % set->arrayLen].value !=
               0) {
            newIntProbes++;
        }
    }

    ptrdiff_t finalIndex = (newIntHash + newIntProbes) % set->arrayLen;
    set->array[finalIndex].value = id;
    set->array[finalIndex].hash = newIntHash;

    set->entries++;

    return true;
}

flo_uint16_a flo_uint16HashSetToArray(flo_Uint16HashSet *set,
                                        flo_Arena *perm) {
    flo_uint16_a result;
    result.buf = FLO_NEW(perm, uint16_t, set->entries);
    result.len = set->entries;

    ptrdiff_t resultIndex = 0;
    for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
        if (set->array[i].value != 0) {
            result.buf[resultIndex++] = set->array[i].value;
        }
    }

    return result;
}

bool flo_containsUint16HashSet(flo_Uint16HashSet *set, uint16_t id) {
    ptrdiff_t index = flo_hash16_xm3(id) % set->arrayLen;

    while (set->array[index].value != 0) {
        if (set->array[index].value == id) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

flo_Uint16HashSet flo_copyUint16HashSet(flo_Uint16HashSet *originalSet,
                                        flo_Arena *perm) {
    flo_Uint16HashSet copy =
        flo_initUint16HashSet((uint16_t)originalSet->arrayLen, perm);
    memcpy(copy.array, originalSet->array,
           originalSet->arrayLen * sizeof(flo_Uint16Entry));
    copy.entries = originalSet->entries;

    return copy;
}

void flo_resetUint16HashSet(flo_Uint16HashSet *set) {
    if (set->array != NULL) {
        memset(set->array, 0, set->arrayLen * sizeof(flo_Uint16Entry));
    }
    set->entries = 0;
}

uint16_t flo_nextUint16HashSetIterator(flo_Uint16HashSetIterator *iterator) {
    flo_Uint16HashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].value != 0) {
            uint16_t value = set->array[iterator->index].value;
            iterator->index++;
            return value;
        }
        iterator->index++;
    }

    return 0;
}

bool flo_hasNextUint16HashSetIterator(flo_Uint16HashSetIterator *iterator) {
    flo_Uint16HashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].value != 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}
