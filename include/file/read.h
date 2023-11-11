#ifndef FILE_READ_H
#define FILE_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "file-status.h"
#include "memory.h"
#include "text/string.h"

flo_FileStatus flo_readFile(flo_String srcPath, flo_String *buffer,
                            flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
