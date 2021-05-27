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
        switch(fsmCurrentState){
            case FSM_START:
                initSystem(TRUE);
                fsmNextState = FSM_TWOPLANES_INIT;
                break;
            
            case FSM_TWOPLANES_INIT:
                initTwoPlanes();
                fsmNextState = FSM_TWOPLANES_RUN;
                break;

            case FSM_TWOPLANES_RUN:
                if(!executeTwoPlanes()){
                    fsmNextState = FSM_BALLBLOB_INIT;
                }
                else{
                    fsmNextState = FSM_TWOPLANES_RUN;
                }
                break;
            
            case FSM_BALLBLOB_INIT:
                initBallBlob();
                exitTwoPlanes();
                fsmNextState = FSM_BALLBLOB_RUN;
                break;

            case FSM_BALLBLOB_RUN:
                if(!executeBallBlob()){
                    fsmNextState = FSM_DOUBLEBUFFER_INIT;
                }
                else{
                    fsmNextState = FSM_BALLBLOB_RUN;
                }
                break;

            case FSM_DOUBLEBUFFER_INIT:
                initDoubleBuffer();
                exitBallBlob();
                fsmNextState = FSM_DOUBLEBUFFER_RUN;
                break;

            case FSM_DOUBLEBUFFER_RUN:
                if(executeDoubleBuffer()){
                    fsmNextState = FSM_STOP;
                }
                else{
                    fsmNextState = FSM_DOUBLEBUFFER_RUN;
                }
                break;

            case FSM_STOP:
                exitDoubleBuffer();
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
