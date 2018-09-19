#ifndef __VIEW_INTRO_H__
#define __VIEW_INTRO_H__

#include <exec/types.h>
#include <proto/graphics.h>

#define VIEW_INTRO_INIT 0
#define VIEW_INTRO_RUNNING 1 
#define VIEW_INTRO_SHUTDOWN 2

#define VIEW_INTRO_DEPTH  1
#define VIEW_INTRO_COLORS 2 
#define VIEW_INTRO_WIDTH  320
#define VIEW_INTRO_HEIGHT 256

#define BLACK 0x000
#define RED   0xf00
#define GREEN 0x0f0
#define BLUE  0x00f

WORD fsmIntro(void);
void initIntro(void);
BOOL executeIntro(void);
void exitIntro(void);
void cleanBitPlanes(PLANEPTR*, UBYTE, UWORD, UWORD);

#endif
