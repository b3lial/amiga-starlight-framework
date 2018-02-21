#include <stdio.h>

#include <exec/types.h>
#include <proto/graphics.h>
#include <graphics/displayinfo.h>
#include <dos/dos.h>

#include "payload_intro.h"
#include "starlight.h"
#include "utils.h"
#include "init.h"

WORD payloadIntroState = PAYLOAD_INTRO_INIT;

WORD fsmPayloadIntro(void){
    switch(payloadIntroState){
        case PAYLOAD_INTRO_INIT:
            initPayloadIntro();
            payloadIntroState = PAYLOAD_INTRO_RUNNING;
            break;

        case PAYLOAD_INTRO_RUNNING:
            if(!executePayloadIntro()){
                payloadIntroState = PAYLOAD_INTRO_SHUTDOWN;
            }
            break;

        case PAYLOAD_INTRO_SHUTDOWN:
            exitPayloadIntro();
            return MODULE_FINISHED;
    }
    
    return MODULE_CONTINUE;
}

void initPayloadIntro(void){
    struct DimensionInfo querydims;
    struct ViewExtra *vextra;
    struct ViewPortExtra *vpextra;
    struct MonitorSpec *monspec;

    struct ViewPort viewPort;
    struct BitMap bitMap;
    struct RasInfo rasInfo;
    struct ColorMap *cm;

    UWORD modeID=DEFAULT_MONITOR_ID | LORES_KEY;
    if (!GetDisplayInfoData( NULL,(UBYTE *)&querydims, 
                sizeof(struct DimensionInfo), DTAG_DIMS, modeID ))
    {
        writeLog("Error: Payload Intro, GetDisplayInfoData() returned false");
        exitSystem(RETURN_ERROR); 
    }

    viewPort.DHeight = querydims.Nominal.MaxY - querydims.Nominal.MinY + 1;
    viewPort.DWidth = querydims.Nominal.MaxX - querydims.Nominal.MinX + 1;
    writeLogInt("viewPort.DHeight: %d\n", viewPort.DHeight);
    writeLogInt("viewPort.DWidth: %d\n", viewPort.DWidth);

    //TODO: implement me
}

BOOL executePayloadIntro(void){
    return FALSE;
}

void exitPayloadIntro(void){

}
