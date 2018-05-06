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
struct ViewPortExtra *vpextra = NULL;
struct ColorMap *cm = NULL;

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
    struct TagItem vcTags[] = {
        { VTAG_ATTACH_CM_SET, NULL },
        { VTAG_VIEWPORTEXTRA_SET, NULL },
        { VTAG_NORMAL_DISP_SET, NULL },
        { VTAG_END_CM, NULL }
    };
    UWORD colortable[] = { BLACK, RED, GREEN, BLUE };
    UWORD i=0;
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
                exitPayloadIntro();
                exitSystem(RETURN_ERROR); 
            }    
        }
        else{
            writeLog("Error: Payload Intro, could not get ViewExtra\n");
            exitPayloadIntro();
            exitSystem(RETURN_ERROR); 
        }
    }
    
    //Create Bitmap and add Bitplanes
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
            exitPayloadIntro();
            exitSystem(RETURN_ERROR); 
        }
        else {
            bitMap.Planes[i] = bitplanes[i];
        }
    }

    //Init RasInfo and add Bitmap
    rasInfo.BitMap = &bitMap;
    rasInfo.RxOffset = 0;
    rasInfo.RyOffset = 0;
    rasInfo.Next = NULL;

    //Init ViewPort and add RasInfo
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

            if( GetDisplayInfoData( NULL , (UBYTE *) &querydims ,
                        sizeof(struct DimensionInfo) , DTAG_DIMS, modeID) )
            {
                width = querydims.Nominal.MaxX - querydims.Nominal.MinX + 1;
                height = querydims.Nominal.MaxY - querydims.Nominal.MinY + 1;
                writeLogInt("width: %d\n", width);
                writeLogInt("height: %d\n", height);
                vpextra->DisplayClip = querydims.Nominal;

                /* Make a DisplayInfo and get ready to attach it */
                if( !(vcTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)) ){
                    writeLog("Error: Could not get DisplayInfo\n");
                    exitPayloadIntro();
                    exitSystem(RETURN_ERROR); 
                }
            }
            else
            {
                writeLog("Could not get DimensionInfo \n");
                exitPayloadIntro();
                exitSystem(RETURN_ERROR); 
            }
        }
        else{
            writeLog("Could not get ViewPortExtra\n");
            exitPayloadIntro();
            exitSystem(RETURN_ERROR); 
        }

        /* This is for backwards compatibility with, for example,   */
        /* a 1.3 screen saver utility that looks at the Modes field */
        viewPort.Modes = (UWORD) (modeID & 0x0000ffff);
    }

    //Create ColorMap
    cm = GetColorMap(PAYLOAD_INTRO_COLORS);
    if(!cm){
        writeLog("Could not get ColorMap\n");
        exitPayloadIntro();
        exitSystem(RETURN_ERROR); 
    }
    if(GfxBase->LibNode.lib_Version >= 36){
        vcTags[0].ti_Data = (ULONG) &viewPort;
        if( VideoControl(cm,vcTags) ){
            writeLog("Could not attach extended structures\n");
            exitPayloadIntro();
            exitSystem(RETURN_ERROR); 
        }
    }
    else{
        viewPort.ColorMap = cm;
    }
    LoadRGB4(&viewPort, colortable, PAYLOAD_INTRO_COLORS);

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
    cleanBitPlanes(bitplanes, PAYLOAD_INTRO_DEPTH, PAYLOAD_INTRO_WIDTH, 
            PAYLOAD_INTRO_HEIGHT);
    if(vextra){
        GfxFree(vextra);
        vextra = NULL;
    }
    if(vpextra){
        GfxFree(vpextra);
        vpextra = NULL;
    }
    if(monspec){
        CloseMonitor(monspec);
        monspec = NULL;
    }
    if(cm){
        FreeColorMap(cm);
        cm = NULL;
    }
}
