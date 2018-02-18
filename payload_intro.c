#include <stdio.h>

#include <exec/types.h>
#include <proto/graphics.h>
#include <graphics/displayinfo.h>

#include "payload_intro.h"
#include "starlight.h"
#include "utils.h"

WORD initIntroSuccess = 0;
WORD exitIntro = 0;

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
        return;
    }

    viewPort.DHeight = querydims.Nominal.MaxY - querydims.Nominal.MinY + 1;
    writeLogInt("MaxX Resolution: %d\n", querydims.Nominal.MaxX - 
            querydims.Nominal.MinX + 1);
    viewPort.DWidth = querydims.Nominal.MaxX - querydims.Nominal.MinX + 1;
    writeLogInt("MaxY Resolution: %d\n", querydims.Nominal.MaxY - 
            querydims.Nominal.MinY + 1);
}

void executePayloadIntro(void){
    
}

void exitPayloadIntro(void){
    
}
