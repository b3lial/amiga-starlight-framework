#ifndef __BLOB_CONTROLLER_H__
#define __BLOB_CONTROLLER_H__

#include <exec/types.h>

#define COLORMAP32_BYTE_SIZE(colorAmount) ((1+colorAmount*3+1)*4)
#define COLORMAP32_LONG_SIZE(colorAmount) (1+colorAmount*3+1)
#define SPREAD(v) ((ULONG)(v) << 24 | (ULONG)(v) << 16 | (ULONG)(v) << 8 | (v))

struct BitMap* loadBlob(const char*, UBYTE, UWORD, UWORD);
BOOL loadColorMap(char*, UWORD*, UWORD);
BOOL loadColorMap32(char* fileName, ULONG* map, UWORD colorAmount);

#endif
