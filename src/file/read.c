#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "file/read.h"
#include "log.h"
#include "memory/arena.h"

flo_FileStatus flo_readFile(flo_String srcPath, flo_String *buffer,
                            flo_Arena *perm) {
    // casting to char* here because a srcpath should not contain any weird
    // chars.
    FILE *srcFile = fopen((char *)srcPath.buf, "rbe");
    if (srcFile == NULL) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR("Failed to open file: ");
            FLO_ERROR(srcPath, FLO_NEWLINE);
        }
        return FILE_CANT_OPEN;
    }

    fseek(srcFile, 0, SEEK_END);
    ptrdiff_t dataLen = ftell(srcFile);
    rewind(srcFile);

    (*buffer).buf = FLO_NEW(perm, unsigned char, dataLen, FLO_NULL_ON_FAIL);
    if ((*buffer).buf == NULL) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Failed to allocate memory for file ")));
            FLO_ERROR(srcPath, FLO_NEWLINE);
        }
        fclose(srcFile);
        return FILE_CANT_ALLOCATE;
    }

    ptrdiff_t result = fread((*buffer).buf, 1, dataLen, srcFile);
    if (result != dataLen) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Failed to read the file contents of ")));
            FLO_ERROR(srcPath, FLO_NEWLINE);
        }
        fclose(srcFile);
        return FILE_CANT_READ;
    }

    (*buffer).len = dataLen;

    fclose(srcFile);
    return FILE_SUCCESS;
}
