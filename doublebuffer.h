#ifndef __VIEW_DOUBLEBUFFER_H__
#define __VIEW_DOUBLEBUFFER_H__

#include <exec/types.h>
#include <proto/graphics.h>

#define VIEW_DOUBLEBUFFER_DEPTH  1 
#define VIEW_DOUBLEBUFFER_COLORS 2 
#define VIEW_DOUBLEBUFFER_WIDTH  320
#define VIEW_DOUBLEBUFFER_HEIGHT 256

#define VIEW_DOUBLEBUFFER_SQUARESIZE 64 

WORD fsmDoubleBuffer(void);
void initDoubleBuffer(void);
BOOL executeDoubleBuffer(void);
void exitDoubleBuffer(void);

void drawRect(struct BitMap* bitmap, UBYTE planeIndex, 
        UWORD xPos, UWORD yPos, UWORD size);
UBYTE getFirstByte(UWORD x);
UBYTE getLastByte(UWORD x);

#endif
