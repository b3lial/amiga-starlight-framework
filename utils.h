#ifndef __UTILS_H__
#define __UTILS_H__

#include <graphics/displayinfo.h>
#include <proto/graphics.h>

BOOL mouseClickDetected(void);
BOOL writeLogFS(const char* formatString, ...);
BOOL initLog(void);
BOOL writeLog(char*);
void cleanBitPlanes(PLANEPTR*, UBYTE, UWORD, UWORD);

#endif
