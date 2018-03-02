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

struct ViewExtra *vextra;
struct MonitorSpec *monspec;

//playfield 
UWORD width = 0;
UWORD height = 0;
PLANEPTR bitplanes[PAYLOAD_INTRO_DEPTH];

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
    struct ViewPortExtra *vpextra;

    struct View view;
    struct ViewPort viewPort;
    struct BitMap bitMap;
    struct RasInfo rasInfo;
    struct ColorMap *cm;

    //get low res resolution and use max area
    UWORD modeID=DEFAULT_MONITOR_ID | LORES_KEY;
    if (!GetDisplayInfoData( NULL,(UBYTE *)&querydims, 
                sizeof(struct DimensionInfo), DTAG_DIMS, modeID ))
    {
        writeLog("Error: Payload Intro, GetDisplayInfoData() returned false");
        exitSystem(RETURN_ERROR); 
    }
    width = querydims.Nominal.MaxX - querydims.Nominal.MinX + 1;
    height = querydims.Nominal.MaxY - querydims.Nominal.MinY + 1;
    writeLogInt("width: %d\n", width);
    writeLogInt("height: %d\n", height);

    //Create View, attach ViewExtra and MonitorSpec
    InitView(&view);
    vextra=GfxNew(VIEW_EXTRA_TYPE); 
    if( vextra ){
        GfxAssociate(&view , vextra);
        view.Modes |= EXTEND_VSTRUCT;

        //Initialize the MonitorSpec field of the ViewExtra
        if( monspec=OpenMonitor(NULL,modeID) ){
            vextra->Monitor=monspec;
        }
        else{
            writeLog("Error: Payload Intro, could not get MonitorSpec\n");
            GfxFree(vextra);
            exitSystem(RETURN_ERROR); 
        }    
    }
    else{
        writeLog("Error: Payload Intro, could not get ViewExtra\n");
        exitSystem(RETURN_ERROR); 
    }
    
    //Create Bitmaps and Bitplanes
    InitBitMap(&bitMap, PAYLOAD_INTRO_DEPTH, width, height);
    for(UBYTE i=0; i<PAYLOAD_INTRO_DEPTH; i++){
        bitplanes[i] = NULL;
    }
    for(UBYTE i=0; i<PAYLOAD_INTRO_DEPTH; i++)
    {
        bitplanes[i] = (PLANEPTR)AllocRaster(width, height);

        if (bitplanes[i] == NULL){
            writeLog("Error: Payload Intro, could not allocate BitPlanes\n");
            cleanBitPlanes(bitplanes, PAYLOAD_INTRO_DEPTH, width, height);
            GfxFree(vextra);
            CloseMonitor(monspec);
            exitSystem(RETURN_ERROR); 
        }
        else {
            bitMap.Planes[i] = bitplanes[i];
        }
    }

    //Init RasInfos
    rasInfo.BitMap = &bitMap;
    rasInfo.RxOffset = 0;
    rasInfo.RyOffset = 0;
    rasInfo.Next = NULL;

    //Init ViewPort
    InitVPort(&viewPort);
    viewPort.RasInfo = &rasInfo;
    viewPort.DWidth  = width; //TODO: -1?
    viewPort.DHeight = height; //TODO: -1?
}

void cleanBitPlanes(PLANEPTR* bmPlanes, UBYTE bmDepth, 
        UWORD bmWidth, UWORD bmHeight)
{
    for(UBYTE i=0; i<bmDepth; i++){
        if((bmPlanes[i]) != NULL){
            FreeRaster((bmPlanes[i]), bmWidth, bmHeight);
        }
        else{
            return;
        }
    }
}

BOOL executePayloadIntro(void){
    return FALSE;
}

void exitPayloadIntro(void){
    cleanBitPlanes(bitplanes, PAYLOAD_INTRO_DEPTH, width, height);
    GfxFree(vextra);
    CloseMonitor(monspec);
}
