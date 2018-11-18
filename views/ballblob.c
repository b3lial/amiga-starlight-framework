#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>
#include <proto/exec.h>

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
struct BitMap* ballBlob = NULL;
struct BitMap* bitMap0 = NULL;

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
    UWORD colortable0[] = { BLACK, RED, GREEN, BLUE, BLACK, RED, GREEN, BLUE };
    BYTE i = 0;
    writeLog("\n== Initialize View: BallBlob ==\n");

    //Load Boingball Blob
    ballBlob = loadBlob("img/square_24x24_3", VIEW_BALLBLOB_DEPTH, 
            VIEW_BALLBLOB_BALL_WIDTH, VIEW_BALLBLOB_BALL_HEIGHT);
    if(ballBlob == NULL){
        writeLog("Error: Payload BallBlob, could not load ball blob\n");
        exitBallBlob();
        exitSystem(RETURN_ERROR); 
    }
    writeLogFS("Ballblob BitMap: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            ballBlob->BytesPerRow, ballBlob->Rows, ballBlob->Flags, 
            ballBlob->pad);

    //Create View and ViewExtra memory structures
    initView(); 

    //Create Bitmap and add Bitplanes
    bitMap0 = createBitMap(VIEW_BALLBLOB_DEPTH, VIEW_BALLBLOB_WIDTH,
            VIEW_BALLBLOB_HEIGHT);
    for(i=0; i<VIEW_BALLBLOB_DEPTH; i++){
        BltClear(bitMap0->Planes[i], (bitMap0->BytesPerRow) * (bitMap0->Rows), 1);
    }
    writeLogFS("Screen BitMap: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            bitMap0->BytesPerRow, bitMap0->Rows, bitMap0->Flags, 
            bitMap0->pad);
    
    //Use Bitplanes to create a ViewPort and add it to View
    addViewPort(bitMap0, colortable0, VIEW_BALLBLOB_COLORS, 
            0, 0, VIEW_BALLBLOB_WIDTH, VIEW_BALLBLOB_HEIGHT);

    //Copy Ball into ViewPort
    BltBitMap(ballBlob, 0, 0, bitMap0, 0, 0, VIEW_BALLBLOB_BALL_WIDTH, 
            VIEW_BALLBLOB_BALL_HEIGHT, 0xC0, 0xff, 0);

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
    cleanBitMap(bitMap0);
    cleanBitMap(ballBlob);
}
