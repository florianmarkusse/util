#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
#include "text/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FLO_LOG_WRITE_BUFFER_LENGTH 1 << 10

typedef struct {
    unsigned char buf[FLO_LOG_WRITE_BUFFER_LENGTH];
    ptrdiff_t len;
} flo_WriteBuffer;

static flo_WriteBuffer stdoutBuffer = (flo_WriteBuffer){.len = 0};
static flo_WriteBuffer stderrBuffer = (flo_WriteBuffer){.len = 0};

typedef enum { FLO_STDOUT, FLO_STDERR } flo_BufferType;

// When adding to a buffer we can check first if the resulting size would be too
// much, and if so, automatically flush to the buffer.

// When the amount is too large, what do we do?? Just ignore it I guess lol, or
// split it up but is too hard.

bool flo_flushBuffer(flo_WriteBuffer *buffer) {
    for (ptrdiff_t bytesWritten = 0; bytesWritten < buffer->len;) {
        ptrdiff_t partialBytesWritten =
            write(STDOUT_FILENO, buffer->buf + bytesWritten,
                  buffer->len - bytesWritten);
        if (partialBytesWritten < 0) {
            FLO_ASSERT(false);
            return -1;
        } else {
            bytesWritten += partialBytesWritten;
        }
    }

    return 0;
}

flo_WriteBuffer *getBuffer(flo_BufferType bufferType) {
    if (bufferType == FLO_STDOUT) {
        return &stdoutBuffer;
    }
    return &stderrBuffer;
}

#define FLO_FLUSH(bufferType) flo_flushBuffer(getBuffer(bufferType))

bool flo_appendToBuffer(flo_BufferType bufferType, flo_String data) {
    if (data.len == 0) {
        return true;
    }

    if (data.len > FLO_LOG_WRITE_BUFFER_LENGTH) {
        FLO_ASSERT(false);
        return false;
    }

    // TODO: make it so that we flush automatically if we are too full...

    flo_WriteBuffer *buffer;
    if (bufferType == FLO_STDOUT) {
        buffer = &stdoutBuffer;
    } else {
        buffer = &stderrBuffer;
    }

    memcpy(buffer->buf + buffer->len, data.buf, data.len);
    buffer->len += data.len;

    return true;
}

bool flo_appendCStr(flo_BufferType bufferType, char *data) {
    ptrdiff_t len = strlen(data);
    return flo_appendToBuffer(
        bufferType, (flo_String){.len = len, .buf = (unsigned char *)data});
}

bool flo_noAppend() { return false; }

#define GET_TYPE(data) _Generic((data), default : typeof(data))

#define FLO_LOG_DATA(bufferType, data)                                         \
    _Generic((data),\
        flo_String:  flo_appendToBuffer, \
        char*:  flo_appendCStr, \
        default: flo_noAppend \
    )(bufferType, data)

#define FLO_LOG_1(data) FLO_LOG_DATA(FLO_STDOUT, data)
#define FLO_LOG_2(bufferType, data) FLO_LOG_DATA(bufferType, data)
#define FLO_LOG_X(a, b, c, ...) c
#define FLO_LOG(...) FLO_LOG_X(__VA_ARGS__, FLO_LOG_2, FLO_LOG_1)(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
