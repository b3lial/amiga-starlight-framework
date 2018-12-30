#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>
#include <dos/dos.h>

#include "starlight/starlight.h"
#include "doublebuffer.h"
#include "main.h"

WORD payloadDoubleBufferState = VIEW_DOUBLEBUFFER_INIT;
struct BitMap* doubleBufferScreen = NULL;

WORD fsmDoubleBuffer(void){
    switch(payloadDoubleBufferState){
        case VIEW_DOUBLEBUFFER_INIT:
            initDoubleBuffer();
            payloadDoubleBufferState = VIEW_DOUBLEBUFFER_RUNNING;
            break;

        case VIEW_DOUBLEBUFFER_RUNNING:
            if(!executeDoubleBuffer()){
                payloadDoubleBufferState = VIEW_DOUBLEBUFFER_SHUTDOWN;
            }
            break;

        case VIEW_DOUBLEBUFFER_SHUTDOWN:
            exitDoubleBuffer();
            return MODULE_FINISHED;
    }
    
    return MODULE_CONTINUE;
}

void initDoubleBuffer(void){
    UWORD colortable0[] = { BLACK, WHITE };
    BYTE i = 0;
    writeLog("\n== Initialize View: DoubleBuffer ==\n");

    //Create View and ViewExtra memory structures
    initView(); 

    //Create Bitmap for ViewPort
    doubleBufferScreen = createBitMap(VIEW_DOUBLEBUFFER_DEPTH, VIEW_DOUBLEBUFFER_WIDTH,
            VIEW_DOUBLEBUFFER_HEIGHT);
    for(i=0; i<VIEW_DOUBLEBUFFER_DEPTH; i++){
        BltClear(doubleBufferScreen->Planes[i], 
                (doubleBufferScreen->BytesPerRow) * (doubleBufferScreen->Rows), 1);
    }
    writeLogFS("Screen BitMap: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            doubleBufferScreen->BytesPerRow, doubleBufferScreen->Rows, 
            doubleBufferScreen->Flags, doubleBufferScreen->pad);
    
    //Add previously created BitMap to ViewPort so its shown on Screen
    addViewPort(doubleBufferScreen, NULL, colortable0, VIEW_DOUBLEBUFFER_COLORS, 
            0, 0, VIEW_DOUBLEBUFFER_WIDTH, VIEW_DOUBLEBUFFER_HEIGHT);

    //Make View visible
    startView();
}

BOOL executeDoubleBuffer(void){
    if(mouseClick()){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

void exitDoubleBuffer(void){
    stopView();
    cleanBitMap(doubleBufferScreen);
}
