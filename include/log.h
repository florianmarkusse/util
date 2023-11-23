#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
#include "math.h"
#include "text/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#define FLO_LOG_WRITE_BUFFER_LENGTH 1 << 10

#define FLO_NEWLINE 0x01
#define FLO_FLUSH 0x02

typedef struct {
    unsigned char buf[FLO_LOG_WRITE_BUFFER_LENGTH];
    int fileDescriptor;
    ptrdiff_t len;
} flo_WriteBuffer;

static flo_WriteBuffer stdoutBuffer =
    (flo_WriteBuffer){.len = 0, .fileDescriptor = STDOUT_FILENO};
static flo_WriteBuffer stderrBuffer =
    (flo_WriteBuffer){.len = 0, .fileDescriptor = STDERR_FILENO};

typedef enum { FLO_STDOUT, FLO_STDERR } flo_BufferType;

// When adding to a buffer we can check first if the resulting size would be too
// much, and if so, automatically flush to the buffer.

// When the amount is too large, what do we do?? Just ignore it I guess lol, or
// split it up but is too hard.

bool flo_flushBuffer(flo_WriteBuffer *buffer) {
    for (ptrdiff_t bytesWritten = 0; bytesWritten < buffer->len;) {
        ptrdiff_t partialBytesWritten =
            write(buffer->fileDescriptor, buffer->buf + bytesWritten,
                  buffer->len - bytesWritten);
        if (partialBytesWritten < 0) {
            FLO_ASSERT(false);
            return -1;
        } else {
            bytesWritten += partialBytesWritten;
        }
    }

    buffer->len = 0;

    return 0;
}

flo_WriteBuffer *getBuffer(flo_BufferType bufferType) {
    if (bufferType == FLO_STDOUT) {
        return &stdoutBuffer;
    }
    return &stderrBuffer;
}

#define FLO_FLUSH_TO(bufferType) flo_flushBuffer(getBuffer(bufferType))

bool flo_appendToBuffer(flo_String data, flo_BufferType bufferType,
                        unsigned char flags) {
    flo_WriteBuffer *buffer = getBuffer(bufferType);
    for (ptrdiff_t bytesWritten = 0; bytesWritten < data.len;) {
        // the minimum of size remaining and what is left in the buffer.
        ptrdiff_t spaceInBuffer = (FLO_LOG_WRITE_BUFFER_LENGTH)-buffer->len;
        ptrdiff_t dataToWrite = data.len - bytesWritten;
        ptrdiff_t bytesToWrite = FLO_MIN(spaceInBuffer, dataToWrite);
        memcpy(buffer->buf + buffer->len, data.buf + bytesWritten,
               bytesToWrite);
        buffer->len += bytesToWrite;
        bytesWritten += bytesToWrite;
        if (bytesWritten < data.len) {
            flo_flushBuffer(buffer);
        }
    }

    if (flags & FLO_NEWLINE) {
        if (buffer->len >= FLO_LOG_WRITE_BUFFER_LENGTH) {
            flo_flushBuffer(buffer);
        }
        buffer->buf[buffer->len] = '\n';
        buffer->len++;
    }

    if (flags & FLO_FLUSH) {
        flo_flushBuffer(buffer);
    }

    return true;
}

bool flo_appendCStr(char *data, flo_BufferType bufferType,
                    unsigned char flags) {
    ptrdiff_t len = strlen(data);
    return flo_appendToBuffer(
        (flo_String){.len = len, .buf = (unsigned char *)data}, bufferType,
        flags);
}

bool flo_appendPtrdiff(ptrdiff_t data, flo_BufferType bufferType,
                       unsigned char flags) {
    unsigned char tmp[64];
    unsigned char *end = tmp + sizeof(tmp);
    unsigned char *beg = end;
    ptrdiff_t t = data > 0 ? -data : data;
    do {
        *--beg = '0' - t % 10;
    } while (t /= 10);
    if (data < 0) {
        *--beg = '-';
    }
    return flo_appendToBuffer(FLO_STRING_PTRS(beg, end), bufferType, flags);
}

bool flo_noAppend() { return false; }

#define FLO_LOG_DATA(data, bufferType, flags)                                  \
    _Generic((data),\
        flo_String:  flo_appendToBuffer, \
        char*:  flo_appendCStr, \
        ptrdiff_t: flo_appendPtrdiff,\
        default: flo_noAppend \
    )(data, bufferType, flags)

#define FLO_LOG_1(data) FLO_LOG_DATA(data, FLO_STDOUT, FLO_NEWLINE)
#define FLO_LOG_2(data, bufferType) FLO_LOG_DATA(data, bufferType, FLO_NEWLINE)
#define FLO_LOG_3(data, bufferType, flags)                                     \
    FLO_LOG_DATA((data), flags, bufferType)

#define FLO_LOG_CHOOSER_IMPL_1(arg1) FLO_LOG_1(arg1)
#define FLO_LOG_CHOOSER_IMPL_2(arg1, arg2) FLO_LOG_2(arg1, arg2)
#define FLO_LOG_CHOOSER_IMPL_3(arg1, arg2, arg3) FLO_LOG_3(arg1, arg2, arg3)
#define FLO_LOG_CHOOSER(...) FLO_LOG_CHOOSER_IMPL(__VA_ARGS__, 3, 2, 1)
#define FLO_LOG_CHOOSER_IMPL(_1, _2, _3, N, ...) FLO_LOG_CHOOSER_IMPL_##N

#define FLO_LOG(...) FLO_LOG_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
