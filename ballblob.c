#include <stdio.h>
#include <stdlib.h>
#include <exec/types.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>
#include <dos/dos.h>

#include "starlight/starlight.h"
#include "ballblob.h"
#include "main.h"

struct BitMap* ballBlob = NULL;
struct BitMap* ballBlobScreen = NULL;

void initBallBlob(void){
    UWORD colortable0[] = { BLACK, RED, GREEN, BLUE, BLACK, RED, GREEN, BLUE };
    BYTE i = 0;
    writeLog("\n== Initialize View: BallBlob ==\n");

    //Load Boingball Blob Sprite and its Colors
    ballBlob = loadBlob("img/ball_207_207_3.RAW", VIEW_BALLBLOB_DEPTH, 
            VIEW_BALLBLOB_BALL_WIDTH, VIEW_BALLBLOB_BALL_HEIGHT);
    if(ballBlob == NULL){
        writeLog("Error: Payload BallBlob, could not load ball blob\n");
        exitStarlight(); 
        exitBallBlob();
        exit(RETURN_ERROR);
    }
    writeLogFS("Ballblob BitMap: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            ballBlob->BytesPerRow, ballBlob->Rows, ballBlob->Flags, 
            ballBlob->pad);
    loadColorMap("img/ball_207_207_3.CMAP", colortable0, VIEW_BALLBLOB_COLORS); 

    //Create View and ViewExtra memory structures
    createNewView(); 

    //Create Bitmap for ViewPort
    ballBlobScreen = createBitMap(VIEW_BALLBLOB_DEPTH, VIEW_BALLBLOB_WIDTH,
            VIEW_BALLBLOB_HEIGHT);
    for(i=0; i<VIEW_BALLBLOB_DEPTH; i++){
        BltClear(ballBlobScreen->Planes[i], 
                (ballBlobScreen->BytesPerRow) * (ballBlobScreen->Rows), 1);
    }
    writeLogFS("Screen BitMap: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            ballBlobScreen->BytesPerRow, ballBlobScreen->Rows, 
            ballBlobScreen->Flags, ballBlobScreen->pad);
    
    //Add previously created BitMap to ViewPort so its shown on Screen
    addViewPort(ballBlobScreen, NULL, colortable0, VIEW_BALLBLOB_COLORS, FALSE, 
            0, 0, VIEW_BALLBLOB_WIDTH, VIEW_BALLBLOB_HEIGHT, 0, 0);

    //Copy Ball into ViewPort
    BltBitMap(ballBlob, 0, 0, ballBlobScreen, 60, 20, VIEW_BALLBLOB_BALL_WIDTH, 
            VIEW_BALLBLOB_BALL_HEIGHT, 0xC0, 0xff, 0);

    //Make View visible
    startView();
}

// just a wrapper because we do not have business logic here
// just quit on mouse click
BOOL executeBallBlob(void){
    return ((BOOL) !mouseClick());
}

void exitBallBlob(void){
    cleanBitMap(ballBlobScreen);
    cleanBitMap(ballBlob);
}
