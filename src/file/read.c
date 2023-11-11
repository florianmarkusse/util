#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "file/read.h"
#include "memory.h"

flo_FileStatus flo_readFile(flo_String srcPath, flo_String *buffer,
                            flo_Arena *perm) {
    // casting to char* here because a srcpath should not contain any weird
    // chars.
    FILE *srcFile = fopen((char *)srcPath.buf, "rbe");
    if (srcFile == NULL) {
        FLO_PRINT_ERROR("Failed to open source file: %s\n", srcPath.buf);
        return FILE_CANT_OPEN;
    }

    fseek(srcFile, 0, SEEK_END);
    ptrdiff_t dataLen = ftell(srcFile);
    rewind(srcFile);

    (*buffer).buf = FLO_NEW(perm, unsigned char, dataLen, FLO_NULL_ON_FAIL);
    if ((*buffer).buf == NULL) {
        FLO_PRINT_ERROR("Failed to allocate memory.\n");
        fclose(srcFile);
        return FILE_CANT_ALLOCATE;
    }

    ptrdiff_t result = fread((*buffer).buf, 1, dataLen, srcFile);
    if (result != dataLen) {
        FLO_PRINT_ERROR("Failed to read the file.\n");
        fclose(srcFile);
        return FILE_CANT_READ;
    }

    (*buffer).len = dataLen;

    fclose(srcFile);
    return FILE_SUCCESS;
}
