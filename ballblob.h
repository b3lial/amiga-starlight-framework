#ifndef __VIEW_BALLBLOB_H__
#define __VIEW_BALLBLOB_H__

#include <exec/types.h>
#include <proto/graphics.h>

#define VIEW_BALLBLOB_INIT 0
#define VIEW_BALLBLOB_RUNNING 1 
#define VIEW_BALLBLOB_SHUTDOWN 2

#define VIEW_BALLBLOB_DEPTH  3 
#define VIEW_BALLBLOB_COLORS 8 
#define VIEW_BALLBLOB_WIDTH  320
#define VIEW_BALLBLOB_HEIGHT 256

#define VIEW_BALLBLOB_BALL_WIDTH 207 //24  
#define VIEW_BALLBLOB_BALL_HEIGHT 207 //207 

WORD fsmBallBlob(void);
void initBallBlob(void);
BOOL executeBallBlob(void);
void exitBallBlob(void);

#endif
