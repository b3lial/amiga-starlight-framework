#include <exec/types.h>

#include "payload_intro.h"
#include "starlight.h"

WORD initIntroSuccess = 0;
WORD exitIntro = 0;

WORD fsmPayloadIntro(void){
    if(!initIntroSuccess){
        initPayloadIntro();
        initIntroSuccess = 1;
        return FSM_INTRO;
    }
    else if(!exitIntro){
        executePayloadIntro();
        exitIntro = 1;
        return FSM_INTRO;
    }
    else{
        exitPayloadIntro();
        return FSM_STOP;
    }
}

void initPayloadIntro(void){
    
}

void executePayloadIntro(void){
    
}

void exitPayloadIntro(void){
    
}
