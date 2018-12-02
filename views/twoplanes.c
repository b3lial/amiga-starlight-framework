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
struct BitMap* bitMap0;
struct BitMap* bitMap1;
BOOL mouseDown = FALSE;
UWORD colortable1[] = { BLACK, DARKRED }; 
UWORD colortable2[] = { BLACK, DARKBLUE }; 
extern struct ViewPort *viewPorts[MAX_VIEW_PORTS];

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
    
    UWORD i,j,k = 0;
    UBYTE patternColor = 0xff;
    UBYTE *displaymem1 = NULL;
    UBYTE *displaymem2 = NULL;
    writeLog("\n== Initialize View: TwoPlanes ==\n");

    //Create View and ViewExtra memory structures
    initView(); 

    //Create Bitmap and add Bitplanes
    bitMap0 = createBitMap(VIEW_TWOPLANES_DEPTH, VIEW_TWOPLANES_WIDTH, 
            VIEW_TWOPLANES_BP_HEIGHT);
    bitMap1 = createBitMap(VIEW_TWOPLANES_DEPTH, VIEW_TWOPLANES_WIDTH, 
            VIEW_TWOPLANES_BP_HEIGHT);
    
    //Init Bitplanes with some Data
    for(i=0; i<VIEW_TWOPLANES_DEPTH; i++){
        displaymem1 = bitMap0->Planes[i];
        displaymem2 = bitMap1->Planes[i];

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
    addViewPort(bitMap0, colortable1, VIEW_TWOPLANES_COLORS, 
            VIEW_TWOPLANES_VP1_X, VIEW_TWOPLANES_VP1_Y, 
            VIEW_TWOPLANES_WIDTH, VIEW_TWOPLANES_VP1_HEIGHT);
    addViewPort(bitMap1, colortable2, VIEW_TWOPLANES_COLORS, 
            VIEW_TWOPLANES_VP2_X, VIEW_TWOPLANES_VP2_Y, 
            VIEW_TWOPLANES_WIDTH, VIEW_TWOPLANES_VP2_HEIGHT);

    //Make View visible
    startView();
}

BOOL executeTwoPlanes(void){
    //change colors of chess board
    WORD redCounter = 0x0100;
    WORD blueCounter = 0x0001;
    colortable1[1]+=redCounter;
    writeLogFS("Red: 0x%x\n", colortable1[1]);
    if(colortable1[1] == 0x0F00 || colortable1[1] == 0x0000){
        redCounter = -redCounter;
        writeLogFS("Inverting Red Counter: %d\n", redCounter);
        
    }
    colortable2[1]+=blueCounter;
    writeLogFS("Blue: 0x%x\n", colortable2[1]);
    if(colortable2[1] == 0x000F || colortable2[1] == 0x0000){
        blueCounter = -blueCounter;
        writeLogFS("Inverting Blue Counter: %d\n", blueCounter);
    }
    WaitTOF();
    LoadRGB4(viewPorts[0], colortable1, VIEW_TWOPLANES_COLORS); 
    LoadRGB4(viewPorts[1], colortable2, VIEW_TWOPLANES_COLORS); 

    //abort upon mouse click
    if(mouseClick()){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

void exitTwoPlanes(void){
    stopView();
    cleanBitMap(bitMap0);
    cleanBitMap(bitMap1);
}
