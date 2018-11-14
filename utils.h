#ifndef __UTILS_H__
#define __UTILS_H__

#include <graphics/displayinfo.h>
#include <proto/graphics.h>
#include <exec/types.h>

BOOL mouseClick(void);
BOOL mouseCiaStatus(void);
BOOL writeLogFS(const char* formatString, ...);
BOOL writeArrayLog(char*, unsigned char*, UWORD);
BOOL initLog(void);
BOOL writeLog(char*);
void cleanBitPlanes(PLANEPTR*, UBYTE, UWORD, UWORD);

#endif
