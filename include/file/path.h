#ifndef FILE_PATH_H
#define FILE_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "memory/arena.h"
#include "text/string.h"

void flo_createPath(flo_String fileLocation, flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
