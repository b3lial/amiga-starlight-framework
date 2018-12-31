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

UBYTE squarePointer = 0;
UBYTE direction = 1;
UBYTE currentBitmap = 0;

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
    WaitTOF();
    WaitTOF();
    if(squarePointer<30){
        drawRect(doubleBufferScreen, 0, squarePointer, squarePointer, 32);
        squarePointer++; 
    }

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

void drawRect(struct BitMap* bitmap, UBYTE planeIndex, 
        UWORD xPos, UWORD yPos, UWORD size){
    PLANEPTR plane = bitmap->Planes[planeIndex];
    UWORD startX = xPos / 8;
    UWORD endX = (xPos + size) / 8;
    UWORD endY = yPos + size;
    UWORD x,y;
    UBYTE firstX = getFirstByte(xPos);
    UBYTE lastX = getLastByte(xPos + size);

    for(y=yPos; y<endY; y++){
        for(x=startX; x<endX; x++){
            if(x==startX){
                plane[y*bitmap->BytesPerRow + x] |= firstX;
            }
            else if(x==endX-1){
                plane[y*bitmap->BytesPerRow + x] |= lastX;
            }
            else {
                plane[y*bitmap->BytesPerRow + x] = 0xff;
            }
        }
    }

    return;
}

UBYTE getFirstByte(UWORD x){
    switch(x % 8){
        case 0: return 0xff;
        case 1: return 0x7f;
        case 2: return 0x3f;
        case 3: return 0x1f;
        case 4: return 0x0f;
        case 5: return 0x07;
        case 6: return 0x03;
        case 7: return 0x01;
    }
    return 0xff;
}

UBYTE getLastByte(UWORD x){
    switch(x % 8){
        case 0: return 0x00;
        case 1: return 0x80;
        case 2: return 0xC0;
        case 3: return 0xE0;
        case 4: return 0xF0;
        case 5: return 0xF8;
        case 6: return 0xFC;
        case 7: return 0xFE;
    }
    return 0x00;
}
