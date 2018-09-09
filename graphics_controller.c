#include <stdio.h>
#include <exec/types.h>
#include <exec/memory.h>

#include <proto/graphics.h>
#include <proto/exec.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include "graphics_controller.h"
#include "init.h"
#include "utils.h"

struct ViewExtra *vextra = NULL;
struct View *view = NULL;
struct MonitorSpec *monspec = NULL;

UWORD vpPointer = 0;
struct ViewPort *viewPorts[MAX_VIEW_PORTS];
struct ViewPortExtra *viewPortExtras[MAX_VIEW_PORTS];
struct RasInfo *rasInfos[MAX_VIEW_PORTS];

void initPalView(void){
    ULONG modeID=PAL_MONITOR_ID | LORES_KEY;
    vpPointer = 0;

    //Create View, 
    view = AllocMem(sizeof(struct View), MEMF_ANY); 
    if(!view)
    {
        writeLog("Error: Graphics Controller, could not allocate View memory\n");
        exitSystem(RETURN_ERROR); 
    }
    InitView(view);
    
    //Attach ViewExtra and MonitorSpec to View if possible
    if(GfxBase->LibNode.lib_Version >= 36)
    {
        vextra=GfxNew(VIEW_EXTRA_TYPE); 
        if( vextra ){
            GfxAssociate(view , vextra);
            view->Modes |= EXTEND_VSTRUCT;

            //Initialize the MonitorSpec field of the ViewExtra
            monspec = OpenMonitor(NULL,modeID);
            if( monspec ){
                vextra->Monitor=monspec;
            }
            else{
                writeLog("Error: Payload Intro, could not get MonitorSpec\n");
                stopView();
                exitSystem(RETURN_ERROR); 
            }    
        }
        else{
            writeLog("Error: Payload Intro, could not get ViewExtra\n");
            stopView();
            exitSystem(RETURN_ERROR); 
        }
    }
}

void addViewPort(struct BitMap *bitMap){
    struct TagItem vcTags[] = {
        { VTAG_ATTACH_CM_SET, NULL },
        { VTAG_VIEWPORTEXTRA_SET, NULL },
        { VTAG_NORMAL_DISP_SET, NULL },
        { VTAG_END_CM, NULL }
    };

    //Alloc memory for RasInfo struct
    rasInfos[vpPointer] = AllocMem(sizeof(struct RasInfo), MEMF_ANY);
    if(!rasInfos[vpPointer])
    {
        writeLogInt("Error: Graphics Controller, could not allocate RasInfo %d memory\n", 
                vpPointer);
        exitSystem(RETURN_ERROR); 
    }

    //Init RasInfo and add Bitmap
    rasInfos[vpPointer]->BitMap = bitMap;
    //BAUSTELLE
    rasInfos[vpPointer]->RxOffset = 0;
    rasInfos[vpPointer]->RyOffset = 0;
    rasInfos[vpPointer]->Next = NULL;

    //Init ViewPort, add RasInfo to ViewPort and add ViewPort to View
    InitVPort(&viewPort);
    viewPort.RasInfo = &rasInfo;
    viewPort.DWidth  = PAYLOAD_INTRO_WIDTH;
    viewPort.DHeight = PAYLOAD_INTRO_HEIGHT;
    view.ViewPort = &viewPort;

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
                writeLogInt("Detected Low Res MinX: %d\n", querydims.Nominal.MinX);
                writeLogInt("Detected Low Res MinY: %d\n", querydims.Nominal.MinY);
                writeLogInt("Detected Low Res MaxX: %d\n", querydims.Nominal.MaxX);
                writeLogInt("Detected Low Res MaxY: %d\n", querydims.Nominal.MaxY);
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
}

void startView(void){

}

void stopView(void){
    if(view){
        writeLogInt("Freeing %d Bytes View memory\n", sizeof(struct View));
        FreeMem(view, sizeof(struct View));
        view = NULL;
    }
    if(vextra){
        writeLogInt("Freeing %d Bytes ViewExtra memory\n", 
                sizeof(struct ViewExtra));
        GfxFree(vextra);
        vextra = NULL;
    }
    if(monspec){
        writeLogInt("Freeing %d Bytes Monitor memory\n", 
                sizeof(struct MonitorSpec));
        CloseMonitor(monspec);
        monspec = NULL;
    }
}

