#include <exec/types.h>
#include <dos/dos.h>

#include "starlight/starlight.h"
#include "twoplanes.h"
#include "ballblob.h"
#include "doublebuffer.h"
#include "main.h"

WORD fsmCurrentState = FSM_START;
WORD fsmNextState = -1;

int main(void)
{
    while(fsmCurrentState!=FSM_QUIT){
        UWORD moduleStatus = NULL;
        
        switch(fsmCurrentState){
            case FSM_START:
                initSystem(TRUE);
                fsmNextState = FSM_TWOPLANES;
                break;
            
            case FSM_TWOPLANES:
                moduleStatus = fsmTwoPlanes();
                if(moduleStatus==MODULE_CONTINUE){
                    fsmNextState = FSM_TWOPLANES;
                }
                else{
                    fsmNextState = FSM_BALLBLOB;
                }
                break;
            
            case FSM_BALLBLOB:
                moduleStatus = fsmBallBlob();
                if(moduleStatus==MODULE_CONTINUE){
                    fsmNextState = FSM_BALLBLOB;
                }
                else{
                    fsmNextState = FSM_DOUBLEBUFFER;
                }
                break;

            case FSM_DOUBLEBUFFER:
                moduleStatus = fsmDoubleBuffer();
                if(moduleStatus==MODULE_CONTINUE){
                    fsmNextState = FSM_DOUBLEBUFFER;
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
                writeLogFS("Error: Main, unknown fsm status %d\n",
                        fsmCurrentState);
                break;
        }
    
        fsmCurrentState = fsmNextState;        
    }

    exitSystem(RETURN_OK);
}
