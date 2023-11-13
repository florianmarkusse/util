#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hash/hash-constants.h"
#include "hash/hashes.h"
#include "hash/string-hash.h"
#include "memory/arena.h"

#define MAX_CAPACITY ((1U << 18U) - 1)

flo_StringHashSet flo_initStringHashSet(ptrdiff_t capacity, flo_Arena *perm) {
    return (flo_StringHashSet){
        .arrayLen = capacity,
        .entries = 0,
        .array = FLO_NEW(perm, flo_StringHashEntry, capacity, FLO_ZERO_MEMORY)};
}

flo_StringInsert flo_insertStringHashSet(flo_StringHashSet *set,
                                         flo_String string, flo_Arena *perm) {
    size_t newStringHash = flo_hashString(string);
    ptrdiff_t newStringProbes = 0;

    while (set->array[(newStringHash + newStringProbes) % set->arrayLen]
               .string.buf != NULL) {
        flo_StringHashEntry entry =
            set->array[(newStringHash + newStringProbes) % set->arrayLen];
        if (flo_stringEquals(entry.string, string)) {
            return (flo_StringInsert){.entryIndex = entry.entryIndex,
                                      .wasInserted = false};
        }
        newStringProbes++;
    }

    bool didResize = false;
    if ((double)set->entries >= (double)set->arrayLen * FLO_GROWTH_FACTOR) {
        didResize = true;
        if ((double)set->arrayLen >= MAX_CAPACITY * 0.9) {
            FLO_PRINT_ERROR(
                "Hash set capacity would exceed the maximum capacity: %d!\n",
                MAX_CAPACITY);
            __builtin_longjmp(perm->jmp_buf, 1);
        }

        ptrdiff_t newCapacity = (set->arrayLen * 2 <= MAX_CAPACITY)
                                    ? set->arrayLen * 2
                                    : MAX_CAPACITY;
        flo_StringHashEntry *oldArray = set->array;
        set->array =
            FLO_NEW(perm, flo_StringHashEntry, newCapacity, FLO_ZERO_MEMORY);

        // Rehashing.
        for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
            if (set->array[i].string.buf != NULL) {
                ptrdiff_t probes = 0;
                ptrdiff_t hash = oldArray[i].hash;
                while (set->array[(hash + probes) % newCapacity].string.buf !=
                       NULL) {
                    probes++;
                }
                ptrdiff_t finalIndex = (hash + probes) % newCapacity;
                set->array[finalIndex].string = oldArray[i].string;
                set->array[finalIndex].entryIndex = oldArray[i].entryIndex;
                set->array[finalIndex].hash = hash;
            }
        }

        set->arrayLen = newCapacity;
    }

    if (didResize) {
        newStringProbes = 0;
        while (set->array[(newStringHash + newStringProbes) % set->arrayLen]
                   .string.buf != NULL) {
            newStringProbes++;
        }
    }

    // increment entries before setting the entry index - 0 used as not found.
    set->entries++;

    ptrdiff_t finalIndex = (newStringHash + newStringProbes) % set->arrayLen;
    set->array[finalIndex] = (flo_StringHashEntry){
        .string = string, .hash = newStringHash, .entryIndex = set->entries};

    return (flo_StringInsert){.entryIndex = set->entries, .wasInserted = true};
}

ptrdiff_t flo_containsStringHashSet(flo_StringHashSet *set, flo_String string) {
    size_t hash = flo_hashString(string);

    ptrdiff_t probes = 0;
    while (set->array[(hash + probes) % set->arrayLen].string.buf != NULL) {
        flo_StringHashEntry entry = set->array[(hash + probes) % set->arrayLen];
        if (flo_stringEquals(entry.string, string)) {
            return entry.entryIndex;
        }
        probes++;
    }

    return 0;
}

flo_StringHashSet flo_copyStringHashSet(flo_StringHashSet *originalSet,
                                        flo_Arena *perm) {
    flo_StringHashSet copy = flo_initStringHashSet(originalSet->arrayLen, perm);
    memcpy(copy.array, originalSet->array,
           originalSet->arrayLen * sizeof(flo_StringHashEntry));
    copy.entries = originalSet->entries;

    return copy;
}

flo_HashComparisonStatus flo_equalsStringHashSet(flo_StringHashSet *set1,
                                                 flo_StringHashSet *set2) {
    if (set1->entries != set2->entries) {
        return HASH_COMPARISON_DIFFERENT_SIZES;
    }

    flo_String element;

    flo_StringHashSetIterator iterator =
        (flo_StringHashSetIterator){.set = set1, .index = 0};
    while ((element = flo_nextStringHashSetIterator(&iterator)).len != 0) {
        if (!flo_containsStringHashSet(set2, element)) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    iterator = (flo_StringHashSetIterator){.set = set2, .index = 0};
    while ((element = flo_nextStringHashSetIterator(&iterator)).len != 0) {
        if (!flo_containsStringHashSet(set1, element)) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    return HASH_COMPARISON_SUCCESS;
}

flo_String flo_nextStringHashSetIterator(flo_StringHashSetIterator *iterator) {
    flo_StringHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].string.buf != NULL) {
            return set->array[iterator->index++].string;
        }
        iterator->index++;
    }

    return FLO_EMPTY_STRING;
}

bool flo_hasNextStringHashSetIterator(flo_StringHashSetIterator *iterator) {
    flo_StringHashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].string.buf != NULL) {
            return true;
        }
        iterator->index++;
    }
    return false;
}