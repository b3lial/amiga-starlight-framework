#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include <dos/dos.h>

#include "views/twoplanes.h"
#include "graphics_controller.h"
#include "main.h"
#include "utils.h"
#include "init.h"

WORD payloadTwoPlanesState = VIEW_TWOPLANES_INIT;
PLANEPTR bitplanes1[VIEW_TWOPLANES_DEPTH];
PLANEPTR bitplanes2[VIEW_TWOPLANES_DEPTH];
BOOL mouseDown = FALSE;

WORD fsmTwoPlanes(void){
    switch(payloadTwoPlanesState){
        case VIEW_TWOPLANES_INIT:
            initTwoPlanes();
            payloadTwoPlanesState = VIEW_TWOPLANES_RUNNING;
            break;

        case VIEW_TWOPLANES_RUNNING:
            if(!executeTwoPlanes()){
                payloadTwoPlanesState = VIEW_TWOPLANES_SHUTDOWN;
            }
            break;

        case VIEW_TWOPLANES_SHUTDOWN:
            exitTwoPlanes();
            return MODULE_FINISHED;
    }
    
    return MODULE_CONTINUE;
}

void initTwoPlanes(void){
    struct BitMap bitMap1 = { 0 };
    struct BitMap bitMap2 = { 0 };
    UWORD colortable1[] = { BLACK, RED };
    UWORD colortable2[] = { BLACK, BLUE };
    
    UWORD i,j,k = 0;
    UBYTE patternColor = 0xff;
    UBYTE *displaymem1 = NULL;
    UBYTE *displaymem2 = NULL;
    writeLog("== Initialize View: TwoPlanes ==\n");

    //Create View and ViewExtra memory structures
    initPalView(); 

    //Create Bitmap and add Bitplanes
    InitBitMap(&bitMap1, VIEW_TWOPLANES_DEPTH, VIEW_TWOPLANES_WIDTH, 
            VIEW_TWOPLANES_BP_HEIGHT);
    InitBitMap(&bitMap2, VIEW_TWOPLANES_DEPTH, VIEW_TWOPLANES_WIDTH, 
            VIEW_TWOPLANES_BP_HEIGHT);
    for(i=0; i<VIEW_TWOPLANES_DEPTH; i++){
        bitplanes1[i] = NULL;
        bitplanes2[i] = NULL;
    }
    for(i=0; i<VIEW_TWOPLANES_DEPTH; i++)
    {
        bitplanes1[i] = (PLANEPTR)AllocRaster(VIEW_TWOPLANES_WIDTH, 
                VIEW_TWOPLANES_BP_HEIGHT);
        bitplanes2[i] = (PLANEPTR)AllocRaster(VIEW_TWOPLANES_WIDTH, 
                VIEW_TWOPLANES_BP_HEIGHT);

        if (bitplanes1[i] == NULL || bitplanes2[i] == NULL){
            writeLog("Error: Payload TwoPlanes, could not allocate BitPlanes\n");
            exitTwoPlanes();
            exitSystem(RETURN_ERROR); 
        }
        else {
            bitMap1.Planes[i] = bitplanes1[i];
            bitMap2.Planes[i] = bitplanes2[i];
        }
    }
    
    //Init Bitplanes with some Data
    for(i=0; i<VIEW_TWOPLANES_DEPTH; i++){
        displaymem1 = bitMap1.Planes[i];
        displaymem2 = bitMap2.Planes[i];

        for(j=0; j<VIEW_TWOPLANES_BP_HEIGHT; j++){
            if(j%8==0){
                if(patternColor==0){
                    patternColor=0xff;
                }
                else{
                    patternColor=0;
                }
            }

            for(k=0; k<VIEW_TWOPLANES_WIDTH/8; k++){
                if(patternColor==0){
                    patternColor=0xff;
                }
                else{
                    patternColor=0;
                }
                displaymem1[j*(VIEW_TWOPLANES_WIDTH/8) + k] = patternColor;
                displaymem2[j*(VIEW_TWOPLANES_WIDTH/8) + k] = patternColor;
            }
        }
    }

    //Use Bitplanes to create a ViewPort and add it to View
    addViewPort(&bitMap1, colortable1, VIEW_TWOPLANES_COLORS, 
            VIEW_TWOPLANES_VP1_X, VIEW_TWOPLANES_VP1_Y, 
            VIEW_TWOPLANES_WIDTH, VIEW_TWOPLANES_VP1_HEIGHT);
    addViewPort(&bitMap2, colortable2, VIEW_TWOPLANES_COLORS, 
            VIEW_TWOPLANES_VP2_X, VIEW_TWOPLANES_VP2_Y, 
            VIEW_TWOPLANES_WIDTH, VIEW_TWOPLANES_VP2_HEIGHT);

    //Make View visible
    startView();
}

BOOL executeTwoPlanes(void){
    if(mouseClick()){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

void exitTwoPlanes(void){
    stopView();

    cleanBitPlanes(bitplanes1, VIEW_TWOPLANES_DEPTH, VIEW_TWOPLANES_WIDTH, 
            VIEW_TWOPLANES_HEIGHT/2);
    cleanBitPlanes(bitplanes2, VIEW_TWOPLANES_DEPTH, VIEW_TWOPLANES_WIDTH, 
            VIEW_TWOPLANES_HEIGHT/2);
}
