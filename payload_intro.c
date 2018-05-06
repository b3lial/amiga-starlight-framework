#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include <dos/dos.h>

#include "payload_intro.h"
#include "starlight.h"
#include "utils.h"
#include "init.h"

WORD payloadIntroState = PAYLOAD_INTRO_INIT;

struct ViewExtra *vextra = NULL;
struct MonitorSpec *monspec = NULL;
PLANEPTR bitplanes[PAYLOAD_INTRO_DEPTH];

struct TagItem vcTags[] =
{
    { VTAG_ATTACH_CM_SET, NULL },
    { VTAG_VIEWPORTEXTRA_SET, NULL },
    { VTAG_NORMAL_DISP_SET, NULL },
    { VTAG_END_CM, NULL }
};

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
    struct View view;
    struct ViewPort viewPort;
    struct BitMap bitMap;
    struct RasInfo rasInfo;

    struct ViewPortExtra *vpextra = NULL;
    struct ColorMap *cm = NULL;

    UBYTE i=0;
    UWORD width = 0;
    UWORD height = 0;

    //This demo runs in low res, 230x200
    UWORD modeID=DEFAULT_MONITOR_ID | LORES_KEY;

    //Create View, 
    InitView(&view);
    
    //Attach ViewExtra and MonitorSpec to View if possible
    if(GfxBase->LibNode.lib_Version >= 36)
    {
        vextra=GfxNew(VIEW_EXTRA_TYPE); 
        if( vextra ){
            GfxAssociate(&view , vextra);
            view.Modes |= EXTEND_VSTRUCT;

            //Initialize the MonitorSpec field of the ViewExtra
            monspec = OpenMonitor(NULL,modeID);
            if( monspec ){
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
    }
    
    //Create Bitmaps and Bitplanes
    InitBitMap(&bitMap, PAYLOAD_INTRO_DEPTH, PAYLOAD_INTRO_WIDTH, 
            PAYLOAD_INTRO_HEIGHT);
    for(i=0; i<PAYLOAD_INTRO_DEPTH; i++){
        bitplanes[i] = NULL;
    }
    for(i=0; i<PAYLOAD_INTRO_DEPTH; i++)
    {
        bitplanes[i] = (PLANEPTR)AllocRaster(PAYLOAD_INTRO_WIDTH, 
                PAYLOAD_INTRO_HEIGHT);

        if (bitplanes[i] == NULL){
            writeLog("Error: Payload Intro, could not allocate BitPlanes\n");
            cleanBitPlanes(bitplanes, PAYLOAD_INTRO_DEPTH, PAYLOAD_INTRO_WIDTH, 
                    PAYLOAD_INTRO_HEIGHT);
            if( vextra ){
                GfxFree(vextra);
            }
            if( monspec ){
                CloseMonitor(monspec);
            }
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
    viewPort.DWidth  = PAYLOAD_INTRO_WIDTH;
    viewPort.DHeight = PAYLOAD_INTRO_HEIGHT;

    //Init ViewPortExtra and attach it to ViewPort
    if(GfxBase->LibNode.lib_Version >= 36)
    {
        vpextra = GfxNew(VIEWPORT_EXTRA_TYPE);
        if( vpextra )
        {
            vcTags[1].ti_Data = (ULONG) vpextra;

            /* Initialize the DisplayClip field of the ViewPortExtra */
            if (!GetDisplayInfoData( NULL,(UBYTE *)&querydims, 
                sizeof(struct DimensionInfo), DTAG_DIMS, modeID ))
            {
                writeLog("Error: Payload Intro, GetDisplayInfoData() returned false\n");
                exitSystem(RETURN_ERROR); 
            }
            width = querydims.Nominal.MaxX - querydims.Nominal.MinX + 1;
            height = querydims.Nominal.MaxY - querydims.Nominal.MinY + 1;
            writeLogInt("width: %d\n", width);
            writeLogInt("height: %d\n", height);
            
            if( GetDisplayInfoData( NULL , (UBYTE *) &querydims ,
                        sizeof(querydims) , DTAG_DIMS, modeID) )
            {
                vpextra->DisplayClip = querydims.Nominal;

                /* Make a DisplayInfo and get ready to attach it */
                if( !(vcTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)) ){
                    writeLog("Error: Could not get DisplayInfo\n");
                    exitSystem(RETURN_ERROR); 
                }
            }
            else
            {
                writeLog("Could not get DimensionInfo \n");
                exitSystem(RETURN_ERROR); 
            }
        }
        else{
            writeLog("Could not get ViewPortExtra\n");
            exitSystem(RETURN_ERROR); 
        }

        /* This is for backwards compatibility with, for example,   */
        /* a 1.3 screen saver utility that looks at the Modes field */
        viewPort.Modes = (UWORD) (modeID & 0x0000ffff);
    }

    //TODO: ...
}

void cleanBitPlanes(PLANEPTR* bmPlanes, UBYTE bmDepth, 
        UWORD bmWidth, UWORD bmHeight)
{
    UBYTE i=0;
    for(i=0; i<bmDepth; i++){
        if((bmPlanes[i]) != NULL){
            FreeRaster((bmPlanes[i]), bmWidth, bmHeight);
            bmPlanes[i] = NULL;
        }
    }
}

BOOL executePayloadIntro(void){
    return FALSE;
}

void exitPayloadIntro(void){
    cleanBitPlanes(bitplanes, PAYLOAD_INTRO_DEPTH, PAYLOAD_INTRO_WIDTH, PAYLOAD_INTRO_HEIGHT);
    if(vextra){
        GfxFree(vextra);
        vextra = NULL;
    }
    if(monspec){
        CloseMonitor(monspec);
        monspec = NULL;
    }
}
