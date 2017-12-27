#include <exec/types.h>

#include "starlight.h"
#include "init.h"
#include "register.h"
#include "utils.h"
#include "payload_intro.h"

WORD fsmCurrentState = FSM_START;
WORD fsmNextState = -1;

int main(void)
{
    while(fsmCurrentState!=FSM_QUIT){
        switch(fsmCurrentState){
            case FSM_START: 
                initSystem();
                fsmNextState = FSM_INTRO;
                break;
            
            case FSM_INTRO:
                fsmNextState = fsmPayloadIntro();
                break;
            
            case FSM_STOP:
                waitForMouseClick();
                exitSystem();
                fsmNextState = FSM_QUIT;
                break;
            
            //something unexcpected happened, we better leave
            default:
                exitSystem();
                fsmNextState = FSM_QUIT;
                break;
        }
    
        fsmCurrentState = fsmNextState;        
    }
}
