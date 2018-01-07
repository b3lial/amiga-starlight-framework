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
    initLog();
    
    while(fsmCurrentState!=FSM_QUIT){
        UWORD moduleStatus = NULL;
        
        switch(fsmCurrentState){
            case FSM_START:
                if(!initSystem()){
                    return 0;
                }
                fsmNextState = FSM_INTRO;
                break;
            
            case FSM_INTRO:
                moduleStatus = fsmPayloadIntro();
                if(moduleStatus==MODULE_CONTINUE){
                    fsmNextState = FSM_INTRO;
                }
                else{
                    fsmNextState = FSM_STOP;
                }
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

    writeLog("Shutting down\n");
    return 0;
}
