#ifndef __PAYLOAD_INTRO_H__
#define __PAYLOAD_INTRO_H__

#include <exec/types.h>

#define PAYLOAD_INTRO_INIT 0
#define PAYLOAD_INTRO_RUNNING 1 
#define PAYLOAD_INTRO_SHUTDOWN 2

WORD fsmPayloadIntro(void);
void initPayloadIntro(void);
BOOL executePayloadIntro(void);
void exitPayloadIntro(void);

#endif
