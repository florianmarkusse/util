#ifndef HASH_HASH_COMPARISON_STATUS_H
#define HASH_HASH_COMPARISON_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HASH_COMPARISON_SUCCESS,
    HASH_COMPARISON_DIFFERENT_SIZES,
    HASH_COMPARISON_DIFFERENT_CONTENT,
    HASH_COMPARISON_NUM_STATUS
} flo_HashComparisonStatus;

static char *hashComparisonStatusStrings[HASH_COMPARISON_NUM_STATUS] = {
    "Success", "Collections have different sizes",
    "Collections have different content"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static char *
flo_hashComparisonStatusToString(flo_HashComparisonStatus status) {
    if (status >= 0 && status < HASH_COMPARISON_NUM_STATUS) {
        return hashComparisonStatusStrings[status];
    }
    return "Unknown hash comparison status code!";
}

#ifdef __cplusplus
}
#endif

#endif
