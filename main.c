#include <exec/types.h>
#include <dos/dos.h>

#include "main.h"
#include "init.h"
#include "utils.h"
#include "views/intro.h"

WORD fsmCurrentState = FSM_START;
WORD fsmNextState = -1;

int main(void)
{
    while(fsmCurrentState!=FSM_QUIT){
        UWORD moduleStatus = NULL;
        
        switch(fsmCurrentState){
            case FSM_START:
                initSystem(TRUE);
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
                fsmNextState = FSM_QUIT;
                break;
            
            //something unexcpected happened, we better leave
            default:
                fsmNextState = FSM_QUIT;
                writeLogInt("Error: Main, unknown fsm status %d\n",
                        fsmCurrentState);
                break;
        }
    
        fsmCurrentState = fsmNextState;        
    }

    exitSystem(RETURN_OK);
}