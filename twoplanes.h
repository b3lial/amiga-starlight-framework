#ifndef __VIEW_TWOPLANES_H__
#define __VIEW_TWOPLANES_H__

#include <exec/types.h>
#include <proto/graphics.h>

#define VIEW_TWOPLANES_DEPTH  1
#define VIEW_TWOPLANES_COLORS 2 
#define VIEW_TWOPLANES_WIDTH  320
#define VIEW_TWOPLANES_HEIGHT 256

#define VIEW_TWOPLANES_BP_HEIGHT (VIEW_TWOPLANES_HEIGHT/2)
#define VIEW_TWOPLANES_VP1_HEIGHT (VIEW_TWOPLANES_HEIGHT/2)
#define VIEW_TWOPLANES_VP2_HEIGHT (VIEW_TWOPLANES_HEIGHT/2-1)

#define VIEW_TWOPLANES_VP1_X 0
#define VIEW_TWOPLANES_VP1_Y 0
#define VIEW_TWOPLANES_VP2_X 0
#define VIEW_TWOPLANES_VP2_Y (VIEW_TWOPLANES_HEIGHT/2+1)

WORD fsmTwoPlanes(void);
void initTwoPlanes(void);
BOOL executeTwoPlanes(void);
void exitTwoPlanes(void);

#endif
