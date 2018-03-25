#ifndef __PAYLOAD_INTRO_H__
#define __PAYLOAD_INTRO_H__

#include <exec/types.h>
#include <proto/graphics.h>

#define PAYLOAD_INTRO_INIT 0
#define PAYLOAD_INTRO_RUNNING 1 
#define PAYLOAD_INTRO_SHUTDOWN 2

#define PAYLOAD_INTRO_DEPTH  2
#define PAYLOAD_INTRO_WIDTH  320
#define PAYLOAD_INTRO_HEIGHT 200

WORD fsmPayloadIntro(void);
void initPayloadIntro(void);
BOOL executePayloadIntro(void);
void exitPayloadIntro(void);
void cleanBitPlanes(PLANEPTR*, UBYTE, UWORD, UWORD);

#endif
