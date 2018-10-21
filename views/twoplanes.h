#ifndef __VIEW_TWOPLANES_H__
#define __VIEW_TWOPLANES_H__

#include <exec/types.h>
#include <proto/graphics.h>

#define VIEW_TWOPLANES_INIT 0
#define VIEW_TWOPLANES_RUNNING 1 
#define VIEW_TWOPLANES_SHUTDOWN 2

#define VIEW_TWOPLANES_DEPTH  1
#define VIEW_TWOPLANES_COLORS 2 
#define VIEW_TWOPLANES_WIDTH  320
#define VIEW_TWOPLANES_HEIGHT 256

#define BLACK 0x000
#define RED   0xf00
#define GREEN 0x0f0
#define BLUE  0x00f

WORD fsmTwoPlanes(void);
void initTwoPlanes(void);
BOOL executeTwoPlanes(void);
void exitTwoPlanes(void);

#endif
