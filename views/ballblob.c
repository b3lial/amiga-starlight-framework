#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include <dos/dos.h>

#include "views/ballblob.h"
#include "graphics_controller.h"
#include "blob_controller.h"
#include "main.h"
#include "utils.h"
#include "init.h"

WORD payloadBallBlobState = VIEW_BALLBLOB_INIT;
PLANEPTR bitplanes0[VIEW_BALLBLOB_DEPTH];

WORD fsmBallBlob(void){
    switch(payloadBallBlobState){
        case VIEW_BALLBLOB_INIT:
            initBallBlob();
            payloadBallBlobState = VIEW_BALLBLOB_RUNNING;
            break;

        case VIEW_BALLBLOB_RUNNING:
            if(!executeBallBlob()){
                payloadBallBlobState = VIEW_BALLBLOB_SHUTDOWN;
            }
            break;

        case VIEW_BALLBLOB_SHUTDOWN:
            exitBallBlob();
            return MODULE_FINISHED;
    }
    
    return MODULE_CONTINUE;
}

void initBallBlob(void){
    struct BitMap bitMap0 = { 0 };
    UWORD colortable0[] = { BLACK, RED, GREEN, BLUE, BLACK, RED, GREEN, BLUE };
    UBYTE i = 0;
    writeLog("== Initialize View: BallBlob ==\n");

    //Load Boingball Blob
    loadBlob("img/ball", 3, 200, 199);

    //Create View and ViewExtra memory structures
    initPalView(); 

    //Create Bitmap and add Bitplanes
    InitBitMap(&bitMap0, VIEW_BALLBLOB_DEPTH, VIEW_BALLBLOB_WIDTH, 
            VIEW_BALLBLOB_HEIGHT);
    for(i=0; i<VIEW_BALLBLOB_DEPTH; i++){
        bitplanes0[i] = NULL;
    }

    for(i=0; i<VIEW_BALLBLOB_DEPTH; i++)
    {
        bitplanes0[i] = (PLANEPTR)AllocRaster(VIEW_BALLBLOB_WIDTH, 
                VIEW_BALLBLOB_HEIGHT);

        if (bitplanes0[i] == NULL){
            writeLog("Error: Payload BallBlob, could not allocate BitPlanes\n");
            exitBallBlob();
            exitSystem(RETURN_ERROR); 
        }
        else {
            BltClear(bitplanes0[i], 
                    VIEW_BALLBLOB_WIDTH*VIEW_BALLBLOB_HEIGHT/8,1);
            bitMap0.Planes[i] = bitplanes0[i];
        }
    }
    
    //Use Bitplanes to create a ViewPort and add it to View
    addViewPort(&bitMap0, colortable0, VIEW_BALLBLOB_COLORS, 
            0, 0, VIEW_BALLBLOB_WIDTH, VIEW_BALLBLOB_HEIGHT);

    //Make View visible
    startView();
}

BOOL executeBallBlob(void){
    if(mouseClick()){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

void exitBallBlob(void){
    stopView();

    cleanBitPlanes(bitplanes0, VIEW_BALLBLOB_DEPTH, VIEW_BALLBLOB_WIDTH, 
            VIEW_BALLBLOB_HEIGHT);
}
