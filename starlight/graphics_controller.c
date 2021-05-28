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

UWORD vpPointer = 0;
const ULONG modeID=PAL_MONITOR_ID | LORES_KEY;
struct ViewData vd;
struct ViewData oldVd;

/**
 * Create a new PAL View and backup the old View structure
 * pointers. Afterwards, you can add ViewPorts and finally
 * display the new View with startView(). 
 */
void createNewView(void){
    // Abort, if an old View exists which was not deleted before
    if(oldVd.view){
        writeLog("Error: Graphics Controller, create new View because old View exists\n");
        exitStarlight(RETURN_ERROR); 
    }

    // Backup current View into old View and delete current View structure
    oldVd = vd;
    memset(&vd, 0, sizeof(struct ViewData));

    //Create View, 
    vd.view = AllocMem(sizeof(struct View), MEMF_ANY); 
    if(!vd.view)
    {
        writeLog("Error: Graphics Controller, could not allocate View memory\n");
        exitStarlight(RETURN_ERROR); 
    }
    InitView(vd.view);

    //reset double buffer if previously used
    vd.dbControl.active = FALSE; 
    
    //Attach ViewExtra and MonitorSpec to View if possible
    if(GfxBase->LibNode.lib_Version >= 36)
    {
        vd.vextra=GfxNew(VIEW_EXTRA_TYPE); 
        if( vd.vextra ){
            GfxAssociate(vd.view , vd.vextra);
            vd.view->Modes |= EXTEND_VSTRUCT;

            //Initialize the MonitorSpec field of the ViewExtra
            vd.monspec = OpenMonitor(NULL,modeID);
            if( vd.monspec ){
                vd.vextra->Monitor = vd.monspec;
            }
            else{
                writeLog("Error: Payload Intro, could not get MonitorSpec\n");
                exitStarlight(RETURN_ERROR); 
            }    
        }
        else{
            writeLog("Error: Payload Intro, could not get ViewExtra\n");
            exitStarlight(RETURN_ERROR); 
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
        exitStarlight(RETURN_ERROR);
    }

    writeLogFS("Creating Viewport: %d\n", vpPointer);

    if(vpPointer >= MAX_VIEW_PORTS){
        writeLog("No more ViewPorts allowed\n");
        return;
    }

    //Is this a double buffered ViewPort?
    if(vd.dbControl.active && doubleBuffer!=NULL){
        writeLog("Error: Only one double buffered ViewPort allowed\n");
        exitStarlight(RETURN_ERROR); 
    }
    else if(!vd.dbControl.active && doubleBuffer!=NULL){
        writeLog("Double buffered ViewPort detected\n");
        vd.dbControl.active = TRUE;
        vd.dbControl.bm0 = bitMap;
        vd.dbControl.bm1 = doubleBuffer;
        vd.dbControl.index = vpPointer;
    }

    //Alloc memory for RasInfo struct
    vd.rasInfos[vpPointer] = AllocMem(sizeof(struct RasInfo), MEMF_ANY);
    if(!vd.rasInfos[vpPointer])
    {
        writeLogFS("Error: Graphics Controller, could not allocate RasInfo %d memory\n", 
                vpPointer);
        exitStarlight(RETURN_ERROR); 
    }

    //Init RasInfo and add Bitmap
    vd.rasInfos[vpPointer]->BitMap = bitMap;
    vd.rasInfos[vpPointer]->RxOffset = rxOffset;
    vd.rasInfos[vpPointer]->RyOffset = ryOffset;
    vd.rasInfos[vpPointer]->Next = NULL;

    //Init ViewPort, add RasInfo to ViewPort and add ViewPort to View
    vd.viewPorts[vpPointer] = AllocMem(sizeof(struct ViewPort), MEMF_ANY);
    if(!vd.viewPorts[vpPointer])
    {
        writeLogFS("Error: Graphics Controller, could not allocate ViewPort %d memory\n", 
                vpPointer);
        exitStarlight(RETURN_ERROR); 
    }
    InitVPort(vd.viewPorts[vpPointer]);
    vd.viewPorts[vpPointer]->RasInfo = vd.rasInfos[vpPointer];
    vd.viewPorts[vpPointer]->DWidth  = width;
    vd.viewPorts[vpPointer]->DHeight = height;
    vd.viewPorts[vpPointer]->DxOffset = x;
    vd.viewPorts[vpPointer]->DyOffset = y;
    if(vpPointer==0)
    {
        vd.view->ViewPort = vd.viewPorts[vpPointer];
    }
    else
    {
        vd.viewPorts[vpPointer-1]->Next = vd.viewPorts[vpPointer];
    }

    //Init ViewPortExtra and attach it to ViewPort
    if(GfxBase->LibNode.lib_Version >= 36)
    {
        vd.viewPortExtras[vpPointer] = GfxNew(VIEWPORT_EXTRA_TYPE);
        if( vd.viewPortExtras[vpPointer] )
        {
            vcTags[1].ti_Data = (ULONG) vd.viewPortExtras[vpPointer];

            if( GetDisplayInfoData( NULL , (UBYTE *) &querydims ,
                        sizeof(struct DimensionInfo) , DTAG_DIMS, modeID) )
            {
                writeLogFS("Detected Display Resolution: %d,%d,%d,%d\n", 
                        querydims.Nominal.MinX, querydims.Nominal.MinY, 
                        querydims.Nominal.MaxX, querydims.Nominal.MaxY);
                vd.viewPortExtras[vpPointer]->DisplayClip = querydims.Nominal;

                /* Make a DisplayInfo and get ready to attach it */
                if( !(vcTags[2].ti_Data = (ULONG) FindDisplayInfo(modeID)) ){
                    writeLog("Error: Could not get DisplayInfo\n");
                    exitStarlight(RETURN_ERROR); 
                }
            }
            else
            {
                writeLog("Could not get DimensionInfo \n");
                exitStarlight(RETURN_ERROR); 
            }
        }
        else{
            writeLogFS("Could not get ViewPortExtra %d\n", vpPointer);
            exitStarlight(RETURN_ERROR); 
        }
    }

    /* This is for backwards compatibility with, for example,   */
    /* a 1.3 screen saver utility that looks at the Modes field */
    vd.viewPorts[vpPointer]->Modes = (UWORD) (modeID & 0x0000ffff);

    //Create ColorMap
    vd.colormaps[vpPointer] = GetColorMap(colortableSize);
    if(!vd.colormaps[vpPointer]){
        writeLog("Could not get ColorMap\n");
        exitStarlight(RETURN_ERROR); 
    }
    writeLogFS("Created a ColorMap with type %d\n", vd.colormaps[vpPointer]->Type);

    if(GfxBase->LibNode.lib_Version >= 36){
        vcTags[0].ti_Data = (ULONG) vd.viewPorts[vpPointer];
        if( VideoControl(vd.colormaps[vpPointer], vcTags) ){
            writeLog("Could not attach extended structures\n");
            exitStarlight(RETURN_ERROR); 
        }
    }
    else{
        vd.viewPorts[vpPointer]->ColorMap = vd.colormaps[vpPointer];
    }

    //colormap loading is different for normal 4 bit colors and 32 bit colors
    if(!useColorMap32){
    	LoadRGB4(vd.viewPorts[vpPointer], (UWORD*) colortable, colortableSize);
    }
    else{
    	LoadRGB32(vd.viewPorts[vpPointer], (ULONG*) colortable);
    }

    vpPointer++;
}

void startView(void){
    WORD i;

    //Create Copper Instructions
    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(vd.viewPorts[i]){
            MakeVPort( vd.view, vd.viewPorts[i] );
        }
        else{
            break;
        }
    }
    MrgCop( vd.view );

    //Create two Copper lists if double buffering is active
    if(vd.dbControl.active){
        //store copper lists for bm0
        vd.dbControl.LOFCprList0 = vd.view->LOFCprList;
        vd.dbControl.SHFCprList0 = vd.view->SHFCprList;
        //create and store copper lists for bm1
        vd.view->LOFCprList = NULL;
        vd.view->SHFCprList = NULL;
        vd.rasInfos[vd.dbControl.index]->BitMap = vd.dbControl.bm1;
        MakeVPort( vd.view, vd.viewPorts[vd.dbControl.index] );
        MrgCop( vd.view );
        vd.dbControl.LOFCprList1 = vd.view->LOFCprList;
        vd.dbControl.SHFCprList1 = vd.view->SHFCprList;
        //initially, use copper lists for bm0 
        vd.view->LOFCprList = vd.dbControl.LOFCprList0;
        vd.view->SHFCprList = vd.dbControl.SHFCprList0;
    }

    WaitTOF();
    LoadView( vd.view );
    WaitTOF();

    // we switched to the new view, so the old view can now be
    // savely deleted
    deleteView(&oldVd);
    memset(&oldVd, 0, sizeof(struct ViewData));
}

void changeBuffer(UBYTE bufferIndex){
    if(!vd.dbControl.active){
       writeLog("Error: Can not swap buffer because no double buffering is active\n"); 
       return;
    }

    if(bufferIndex==0){
        vd.view->LOFCprList = vd.dbControl.LOFCprList0;
        vd.view->SHFCprList = vd.dbControl.SHFCprList0;
    }
    else{
        vd.view->LOFCprList = vd.dbControl.LOFCprList1;
        vd.view->SHFCprList = vd.dbControl.SHFCprList1;
    }

    WaitTOF();
    LoadView( vd.view );
    WaitTOF();
}

void deleteView(struct ViewData* currentVd){
    WORD i;

    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(currentVd->viewPorts[i]){
            FreeVPortCopLists(currentVd->viewPorts[i]);

            writeLogFS("Freeing %d Bytes ViewPort memory\n", 
                    sizeof(struct ViewPort));
            FreeMem(currentVd->viewPorts[i], sizeof(struct ViewPort));
            currentVd->viewPorts[i] = NULL;
        }
        else{
            break;
        }
    }

    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(currentVd->rasInfos[i]){
            writeLogFS("Freeing %d Bytes RasInfo memory\n", 
                    sizeof(struct RasInfo));
            FreeMem(currentVd->rasInfos[i], sizeof(struct RasInfo));
            currentVd->rasInfos[i] = NULL;
        }
        else{
            break;
        }
    }
    
    for(i=0;i<MAX_VIEW_PORTS;i++){
        if(currentVd->viewPortExtras[i]){
            writeLogFS("Freeing %d Bytes ViewPortExtra memory\n", 
                    sizeof(struct ViewPortExtra));
            GfxFree(currentVd->viewPortExtras[i]);
            currentVd->viewPortExtras[i] = NULL;
        }
        else{
            break;
        }
    }

    //we have to handle two cases here: single buffered and
    //double buffered
    if(currentVd->view){
        if(currentVd->dbControl.active){
            if(currentVd->dbControl.LOFCprList0){
                writeLogFS("Freeing %d Bytes Copperlist LOFCprList0 memory\n", 
                    sizeof(struct cprlist));
                FreeCprList(currentVd->dbControl.LOFCprList0);
                currentVd->dbControl.LOFCprList0 = NULL;
            }
            if(currentVd->dbControl.SHFCprList0){
                writeLogFS("Freeing %d Bytes Copperlist SHFCprList0 memory\n",
                    sizeof(struct cprlist));
                FreeCprList(currentVd->dbControl.SHFCprList0); 
                currentVd->dbControl.SHFCprList0 = NULL;
            }
            if(currentVd->dbControl.LOFCprList1){
                writeLogFS("Freeing %d Bytes Copperlist LOFCprList1 memory\n", 
                    sizeof(struct cprlist));
                FreeCprList(currentVd->dbControl.LOFCprList1);
                currentVd->dbControl.LOFCprList1 = NULL;
            }
            if(currentVd->dbControl.SHFCprList1){
                writeLogFS("Freeing %d Bytes Copperlist SHFCprList1 memory\n",
                    sizeof(struct cprlist));
                FreeCprList(currentVd->dbControl.SHFCprList1); 
                currentVd->dbControl.SHFCprList1 = NULL;
            }
        }
        else{
            if(currentVd->view->LOFCprList){
                writeLogFS("Freeing %d Bytes Copperlist LOFCprList memory\n", 
                    sizeof(struct cprlist));
                FreeCprList(currentVd->view->LOFCprList);
            }
            if(currentVd->view->SHFCprList){
                writeLogFS("Freeing %d Bytes Copperlist SHFCprList memory\n",
                    sizeof(struct cprlist));
                FreeCprList(currentVd->view->SHFCprList); 
            }
        }
        
        writeLogFS("Freeing %d Bytes View memory\n", sizeof(struct View));
        FreeMem(currentVd->view, sizeof(struct View));
        currentVd->view = NULL;
    }
    if(currentVd->vextra){
        writeLogFS("Freeing %d Bytes ViewExtra memory\n", 
                sizeof(struct ViewExtra));
        GfxFree(currentVd->vextra);
        currentVd->vextra = NULL;
    }
    if(currentVd->monspec){
        writeLogFS("Freeing %d Bytes Monitor memory\n", 
                sizeof(struct MonitorSpec));
        CloseMonitor(currentVd->monspec);
        currentVd->monspec = NULL;
    }

    for(i=0; i<MAX_VIEW_PORTS; i++){
        if(currentVd->colormaps[i]){
            writeLogFS("Freeing %d Bytes ColorMap memory\n", 
                sizeof(struct ColorMap));
            FreeColorMap(currentVd->colormaps[i]);
            currentVd->colormaps[i] = NULL;
        }
        else{
            break;
        }
    }
}

void deleteAllViews(void){
    deleteView(&vd);
    deleteView(&oldVd);
}
