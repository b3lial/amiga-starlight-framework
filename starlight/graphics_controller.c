#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <proto/graphics.h>
#include <proto/exec.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include "starlight/starlight.h"

struct ViewExtra *vextra = NULL;
struct View *view = NULL;
struct MonitorSpec *monspec = NULL;
ULONG modeID=PAL_MONITOR_ID | LORES_KEY;

struct DoubleBufferControl dbControl = { 0 };
UWORD vpPointer = 0;
struct ViewPort *viewPorts[MAX_VIEW_PORTS];
struct ViewPortExtra *viewPortExtras[MAX_VIEW_PORTS];
struct RasInfo *rasInfos[MAX_VIEW_PORTS];
struct ColorMap *colormaps[MAX_VIEW_PORTS];

void initView(void){
    //Create View, 
    view = AllocMem(sizeof(struct View), MEMF_ANY); 
    if(!view)
    {
        writeLog("Error: Graphics Controller, could not allocate View memory\n");
        exitSystem(RETURN_ERROR); 
    }
    InitView(view);

    //reset double buffer if previously used
    dbControl.active = FALSE; 
    
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

void addViewPort(struct BitMap *bitMap, struct BitMap *doubleBuffer, 
        void *colortable, UWORD colortableSize, BOOL useColorMap32,
		UWORD x, UWORD y, UWORD width, UWORD height, UWORD rxOffset,
        UWORD ryOffset){
    struct DimensionInfo querydims = { {0} };
    struct TagItem vcTags[] = {
        { VTAG_ATTACH_CM_SET, NULL },
        { VTAG_VIEWPORTEXTRA_SET, NULL },
        { VTAG_NORMAL_DISP_SET, NULL },
        { VTAG_END_CM, NULL }
    };

    if(useColorMap32 && GfxBase->LibNode.lib_Version < 39){
        writeLogFS("Error: Requesting 24 bit color depth but Kickstart Gfx version is %d\n",
        		GfxBase->LibNode.lib_Version);
        stopView();
        exitSystem(RETURN_ERROR);
    }

    writeLogFS("Creating Viewport: %d\n", vpPointer);

    if(vpPointer >= MAX_VIEW_PORTS){
        writeLog("No more ViewPorts allowed\n");
        return;
    }

    //Is this a double buffered ViewPort?
    if(dbControl.active && doubleBuffer!=NULL){
        writeLog("Error: Only one double buffered ViewPort allowed\n");
        stopView();
        exitSystem(RETURN_ERROR); 
    }
    else if(!dbControl.active && doubleBuffer!=NULL){
        writeLog("Double buffered ViewPort detected\n");
        dbControl.active = TRUE;
        dbControl.bm0 = bitMap;
        dbControl.bm1 = doubleBuffer;
        dbControl.index = vpPointer;
    }

    //Alloc memory for RasInfo struct
    rasInfos[vpPointer] = AllocMem(sizeof(struct RasInfo), MEMF_ANY);
    if(!rasInfos[vpPointer])
    {
        writeLogFS("Error: Graphics Controller, could not allocate RasInfo %d memory\n", 
                vpPointer);
        stopView();
        exitSystem(RETURN_ERROR); 
    }

    //Init RasInfo and add Bitmap
    rasInfos[vpPointer]->BitMap = bitMap;
    rasInfos[vpPointer]->RxOffset = rxOffset;
    rasInfos[vpPointer]->RyOffset = ryOffset;
    rasInfos[vpPointer]->Next = NULL;

    //Init ViewPort, add RasInfo to ViewPort and add ViewPort to View
    viewPorts[vpPointer] = AllocMem(sizeof(struct ViewPort), MEMF_ANY);
    if(!viewPorts[vpPointer])
    {
        writeLogFS("Error: Graphics Controller, could not allocate ViewPort %d memory\n", 
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
                writeLogFS("Detected Display Resolution: %d,%d,%d,%d\n", 
                        querydims.Nominal.MinX, querydims.Nominal.MinY, 
                        querydims.Nominal.MaxX, querydims.Nominal.MaxY);
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
            writeLogFS("Could not get ViewPortExtra %d\n", vpPointer);
            stopView();
            exitSystem(RETURN_ERROR); 
        }
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
    writeLogFS("Created a ColorMap with type %d\n", colormaps[vpPointer]->Type);

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

    //colormap loading is different for normal 4 bit colors and 32 bit colors
    if(!useColorMap32){
    	LoadRGB4(viewPorts[vpPointer], (UWORD*) colortable, colortableSize);
    }
    else{
    	LoadRGB32(viewPorts[vpPointer], (ULONG*) colortable);
    }

    vpPointer++;
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

    //Create two Copper lists if double buffering is active
    if(dbControl.active){
        //store copper lists for bm0
        dbControl.LOFCprList0 = view->LOFCprList;
        dbControl.SHFCprList0 = view->SHFCprList;
        //create and store copper lists for bm1
        view->LOFCprList = NULL;
        view->SHFCprList = NULL;
        rasInfos[dbControl.index]->BitMap = dbControl.bm1;
        MakeVPort( view, viewPorts[dbControl.index] );
        MrgCop( view );
        dbControl.LOFCprList1 = view->LOFCprList;
        dbControl.SHFCprList1 = view->SHFCprList;
        //initially, use copper lists for bm0 
        view->LOFCprList = dbControl.LOFCprList0;
        view->SHFCprList = dbControl.SHFCprList0;
    }

    LoadView( view );
    WaitTOF();
}

void changeBuffer(UBYTE bufferIndex){
    if(!dbControl.active){
       writeLog("Error: Can not swap buffer because no double buffering is active\n"); 
       return;
    }

    if(bufferIndex==0){
        view->LOFCprList = dbControl.LOFCprList0;
        view->SHFCprList = dbControl.SHFCprList0;
    }
    else{
        view->LOFCprList = dbControl.LOFCprList1;
        view->SHFCprList = dbControl.SHFCprList1;
    }
    LoadView( view );
    WaitTOF();
}

void stopView(void){
    WORD i;
    LoadView(NULL);
    WaitTOF();

    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(viewPorts[i]){
            FreeVPortCopLists(viewPorts[i]);

            writeLogFS("Freeing %d Bytes ViewPort memory\n", 
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
            writeLogFS("Freeing %d Bytes RasInfo memory\n", 
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
            writeLogFS("Freeing %d Bytes ViewPortExtra memory\n", 
                    sizeof(struct ViewPortExtra));
            GfxFree(viewPortExtras[i]);
            viewPortExtras[i] = NULL;
        }
        else{
            break;
        }
    }

    //we have to handle two cases here: single buffered and
    //double buffered
    if(view){
        if(dbControl.active){
            if(dbControl.LOFCprList0){
                writeLogFS("Freeing %d Bytes Copperlist LOFCprList0 memory\n", 
                    sizeof(struct cprlist));
                FreeCprList(dbControl.LOFCprList0);
                dbControl.LOFCprList0 = NULL;
            }
            if(dbControl.SHFCprList0){
                writeLogFS("Freeing %d Bytes Copperlist SHFCprList0 memory\n",
                    sizeof(struct cprlist));
                FreeCprList(dbControl.SHFCprList0); 
                dbControl.SHFCprList0 = NULL;
            }
            if(dbControl.LOFCprList1){
                writeLogFS("Freeing %d Bytes Copperlist LOFCprList1 memory\n", 
                    sizeof(struct cprlist));
                FreeCprList(dbControl.LOFCprList1);
                dbControl.LOFCprList1 = NULL;
            }
            if(dbControl.SHFCprList1){
                writeLogFS("Freeing %d Bytes Copperlist SHFCprList1 memory\n",
                    sizeof(struct cprlist));
                FreeCprList(dbControl.SHFCprList1); 
                dbControl.SHFCprList1 = NULL;
            }
        }
        else{
            if(view->LOFCprList){
                writeLogFS("Freeing %d Bytes Copperlist LOFCprList memory\n", 
                    sizeof(struct cprlist));
                FreeCprList(view->LOFCprList);
            }
            if(view->SHFCprList){
                writeLogFS("Freeing %d Bytes Copperlist SHFCprList memory\n",
                    sizeof(struct cprlist));
                FreeCprList(view->SHFCprList); 
            }
        }
        
        writeLogFS("Freeing %d Bytes View memory\n", sizeof(struct View));
        FreeMem(view, sizeof(struct View));
        view = NULL;
    }
    if(vextra){
        writeLogFS("Freeing %d Bytes ViewExtra memory\n", 
                sizeof(struct ViewExtra));
        GfxFree(vextra);
        vextra = NULL;
    }
    if(monspec){
        writeLogFS("Freeing %d Bytes Monitor memory\n", 
                sizeof(struct MonitorSpec));
        CloseMonitor(monspec);
        monspec = NULL;
    }

    for(i=0; i<MAX_VIEW_PORTS; i++){
        if(colormaps[i]){
            writeLogFS("Freeing %d Bytes ColorMap memory\n", 
                sizeof(struct ColorMap));
            FreeColorMap(colormaps[i]);
            colormaps[i] = NULL;
        }
        else{
            break;
        }
    }
}
