#include <stdio.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>

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
ULONG modeID=PAL_MONITOR_ID | LORES_KEY;

UWORD vpPointer = 0;
struct ViewPort *viewPorts[MAX_VIEW_PORTS];
struct ViewPortExtra *viewPortExtras[MAX_VIEW_PORTS];
struct RasInfo *rasInfos[MAX_VIEW_PORTS];
struct ColorMap *colormaps[MAX_VIEW_PORTS];

void initPalView(void){
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
    
    vpPointer = 0;
}

void addViewPort(struct BitMap *bitMap, UWORD *colortable, WORD colortableSize,
        WORD x, WORD y, WORD width, WORD height){
    struct DimensionInfo querydims = { {0} };
    struct TagItem vcTags[] = {
        { VTAG_ATTACH_CM_SET, NULL },
        { VTAG_VIEWPORTEXTRA_SET, NULL },
        { VTAG_NORMAL_DISP_SET, NULL },
        { VTAG_END_CM, NULL }
    };

    writeLogInt("Viewport: %d\n", vpPointer);

    if(vpPointer >= MAX_VIEW_PORTS){
        writeLog("No more ViewPorts allowed\n");
        return;
    }

    //Alloc memory for RasInfo struct
    rasInfos[vpPointer] = AllocMem(sizeof(struct RasInfo), MEMF_ANY);
    if(!rasInfos[vpPointer])
    {
        writeLogInt("Error: Graphics Controller, could not allocate RasInfo %d memory\n", 
                vpPointer);
        stopView();
        exitSystem(RETURN_ERROR); 
    }

    //Init RasInfo and add Bitmap
    rasInfos[vpPointer]->BitMap = bitMap;
    rasInfos[vpPointer]->RxOffset = 0;
    rasInfos[vpPointer]->RyOffset = 0;
    rasInfos[vpPointer]->Next = NULL;

    //Init ViewPort, add RasInfo to ViewPort and add ViewPort to View
    viewPorts[vpPointer] = AllocMem(sizeof(struct ViewPort), MEMF_ANY);
    if(!viewPorts[vpPointer])
    {
        writeLogInt("Error: Graphics Controller, could not allocate ViewPort %d memory\n", 
                vpPointer);
        stopView();
        exitSystem(RETURN_ERROR); 
    }
    InitVPort(viewPorts[vpPointer]);
    viewPorts[vpPointer]->RasInfo = rasInfos[vpPointer];
    viewPorts[vpPointer]->DWidth  = width;
    viewPorts[vpPointer]->DHeight = height;
    viewPorts[vpPointer]->DxOffset = x;
    viewPorts[vpPointer]->DyOffset = y;
    if(vpPointer==0)
    {
        view->ViewPort = viewPorts[vpPointer];
    }
    else
    {
        viewPorts[vpPointer-1]->Next = viewPorts[vpPointer];
    }

    //Init ViewPortExtra and attach it to ViewPort
    if(GfxBase->LibNode.lib_Version >= 36)
    {
        viewPortExtras[vpPointer] = GfxNew(VIEWPORT_EXTRA_TYPE);
        if( viewPortExtras[vpPointer] )
        {
            vcTags[1].ti_Data = (ULONG) viewPortExtras[vpPointer];

            if( GetDisplayInfoData( NULL , (UBYTE *) &querydims ,
                        sizeof(struct DimensionInfo) , DTAG_DIMS, modeID) )
            {
                writeLogInt("Detected Low Res MinX: %d\n", querydims.Nominal.MinX);
                writeLogInt("Detected Low Res MinY: %d\n", querydims.Nominal.MinY);
                writeLogInt("Detected Low Res MaxX: %d\n", querydims.Nominal.MaxX);
                writeLogInt("Detected Low Res MaxY: %d\n", querydims.Nominal.MaxY);
                viewPortExtras[vpPointer]->DisplayClip = querydims.Nominal;

                /* Make a DisplayInfo and get ready to attach it */
                if( !(vcTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)) ){
                    writeLog("Error: Could not get DisplayInfo\n");
                    stopView();
                    exitSystem(RETURN_ERROR); 
                }
            }
            else
            {
                writeLog("Could not get DimensionInfo \n");
                stopView();
                exitSystem(RETURN_ERROR); 
            }
        }
        else{
            writeLogInt("Could not get ViewPortExtra %d\n", vpPointer);
            stopView();
            exitSystem(RETURN_ERROR); 
        }

        /* This is for backwards compatibility with, for example,   */
        /* a 1.3 screen saver utility that looks at the Modes field */
        viewPorts[vpPointer]->Modes = (UWORD) (modeID & 0x0000ffff);

        //Create ColorMap
        colormaps[vpPointer] = GetColorMap(colortableSize);
        if(!colormaps[vpPointer]){
            writeLog("Could not get ColorMap\n");
            stopView();
            exitSystem(RETURN_ERROR); 
        }

        if(GfxBase->LibNode.lib_Version >= 36){
            vcTags[0].ti_Data = (ULONG) viewPorts[vpPointer];
            if( VideoControl(colormaps[vpPointer], vcTags) ){
                writeLog("Could not attach extended structures\n");
                stopView();
                exitSystem(RETURN_ERROR); 
            }
        }
        else{
            viewPorts[vpPointer]->ColorMap = colormaps[vpPointer];
        }
        LoadRGB4(viewPorts[vpPointer], colortable, colortableSize);
        
        vpPointer++;
    }
}

void startView(void){
    WORD i;

    //Create Copper Instructions
    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(viewPorts[i]){
            MakeVPort( view, viewPorts[i] );
        }
        else{
            break;
        }
    }
    MrgCop( view );

    //Display the View
    LoadView( view );
}

void stopView(void){
    WORD i;
    WaitTOF();
    LoadView(NULL);

    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(viewPorts[i]){
            FreeVPortCopLists(viewPorts[i]);

            writeLogInt("Freeing %d Bytes ViewPort memory\n", 
                    sizeof(struct ViewPort));
            FreeMem(viewPorts[i], sizeof(struct ViewPort));
            viewPorts[i] = NULL;
        }
        else{
            break;
        }
    }

    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(rasInfos[i]){
            writeLogInt("Freeing %d Bytes RasInfo memory\n", 
                    sizeof(struct RasInfo));
            FreeMem(rasInfos[i], sizeof(struct RasInfo));
            rasInfos[i] = NULL;
        }
        else{
            break;
        }
    }
    
    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(viewPortExtras[i]){
            writeLogInt("Freeing %d Bytes ViewPortExtra memory\n", 
                    sizeof(struct ViewPortExtra));
            GfxFree(viewPortExtras[i]);
            viewPortExtras[i] = NULL;
        }
        else{
            break;
        }
    }

    if(view){
        if(view->LOFCprList){
            writeLogInt("Freeing %d Bytes Copperlist LOFCprList memory\n", 
                sizeof(struct cprlist));
            FreeCprList(view->LOFCprList);
        }
        if(view->SHFCprList){
            writeLogInt("Freeing %d Bytes Copperlist SHFCprList memory\n",
                sizeof(struct cprlist));
            FreeCprList(view->SHFCprList); 
        }
        
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

    for(i=0; i<MAX_VIEW_PORTS; i++){
        if(colormaps[i]){
            writeLogInt("Freeing %d Bytes ColorMap memory\n", 
                sizeof(struct ColorMap));
            FreeColorMap(colormaps[i]);
            colormaps[i] = NULL;
        }
        else{
            break;
        }
    }
}

