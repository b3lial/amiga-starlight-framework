#include <exec/types.h>
#include <clib/graphics_protos.h>
#include <graphics/displayinfo.h>
#include <stdio.h>
#include <graphics/displayinfo.h>

#include "payload_intro.h"
#include "starlight.h"
#include "utils.h"

WORD initIntroSuccess = 0;
WORD exitIntro = 0;

extern ULONG finalMessage;

WORD fsmPayloadIntro(void){
    if(!initIntroSuccess){
        initPayloadIntro();
        initIntroSuccess = 1;
        return MODULE_CONTINUE;
    }
    else if(!exitIntro){
        executePayloadIntro();
        exitIntro = 1;
        return MODULE_CONTINUE;
    }
    else{
        exitPayloadIntro();
        return MODULE_FINISHED;
    }
}

void initPayloadIntro(void){
    struct DimensionInfo querydims;
    struct ViewPort viewPort;

    UWORD modeID=DEFAULT_MONITOR_ID | LORES_KEY;
    if (GetDisplayInfoData( NULL,(UBYTE *)&querydims, sizeof(struct DimensionInfo),
                        DTAG_DIMS, modeID ))
    {
        viewPort.DHeight = querydims.Nominal.MaxY - querydims.Nominal.MinY + 1;
        viewPort.DWidth = querydims.Nominal.MaxX - querydims.Nominal.MinX + 1;
        finalMessage = querydims.Nominal.MaxY - querydims.Nominal.MinY;
    }
    else{
    
    }
}

void executePayloadIntro(void){
    
}

void exitPayloadIntro(void){
    
}
