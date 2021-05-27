#include <stdio.h>
#include <unistd.h>
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

struct BitMap* doubleBufferScreen0 = NULL;
struct BitMap* doubleBufferScreen1 = NULL;

UWORD squarePointerX = 0;
UWORD squarePointerY = 0;
BYTE directionX = 1;
BYTE directionY = 1;
UBYTE currentBitmap = 0;

void initDoubleBuffer(void){
    UWORD colortable0[] = { BLACK, WHITE };
    BYTE i = 0;
    writeLog("\n== Initialize View: DoubleBuffer ==\n");

    //Create View and ViewExtra memory structures
    createNewView(); 

    //Create Bitmap for ViewPort
    doubleBufferScreen0 = createBitMap(VIEW_DOUBLEBUFFER_DEPTH, 
            VIEW_DOUBLEBUFFER_WIDTH, VIEW_DOUBLEBUFFER_HEIGHT);
    doubleBufferScreen1 = createBitMap(VIEW_DOUBLEBUFFER_DEPTH, 
            VIEW_DOUBLEBUFFER_WIDTH, VIEW_DOUBLEBUFFER_HEIGHT);
    for(i=0; i<VIEW_DOUBLEBUFFER_DEPTH; i++){
        BltClear(doubleBufferScreen0->Planes[i], 
                (doubleBufferScreen0->BytesPerRow) * (doubleBufferScreen0->Rows), 1);
        BltClear(doubleBufferScreen1->Planes[i], 
                (doubleBufferScreen1->BytesPerRow) * (doubleBufferScreen1->Rows), 1);
    }
    writeLogFS("Screen BitMap 0: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            doubleBufferScreen0->BytesPerRow, doubleBufferScreen0->Rows, 
            doubleBufferScreen0->Flags, doubleBufferScreen0->pad);
    writeLogFS("Screen BitMap 1: BytesPerRow: %d, Rows: %d, Flags: %d, pad: %d\n",
            doubleBufferScreen1->BytesPerRow, doubleBufferScreen1->Rows, 
            doubleBufferScreen1->Flags, doubleBufferScreen1->pad);
    
    //Add previously created BitMap to ViewPort so its shown on Screen
    addViewPort(doubleBufferScreen0, doubleBufferScreen1, colortable0,
            VIEW_DOUBLEBUFFER_COLORS, FALSE, 0, 0, VIEW_DOUBLEBUFFER_WIDTH, 
            VIEW_DOUBLEBUFFER_HEIGHT, 0, 0);

    //Make View visible
    startView();
}

BOOL executeDoubleBuffer(void){    
    BYTE i = 0;
    struct BitMap* bitmap;
    struct BitMap* old;

    //chose next bitmap
    if(currentBitmap == 0){
        bitmap = doubleBufferScreen1;
        old = doubleBufferScreen0;
    }
    else{
        bitmap = doubleBufferScreen0;
        old = doubleBufferScreen1;
    }

    //draw and flip buffer
    drawRect(bitmap, 0, squarePointerX, squarePointerY, 
            VIEW_DOUBLEBUFFER_SQUARESIZE);
    currentBitmap ^= 1;
    changeBuffer(currentBitmap);
    for(i=0; i<VIEW_DOUBLEBUFFER_DEPTH; i++){
        BltClear(old->Planes[i], (old->BytesPerRow) * (old->Rows), 1);
    }

    //check whether we change animation direction
    if(squarePointerX==VIEW_DOUBLEBUFFER_WIDTH-VIEW_DOUBLEBUFFER_SQUARESIZE-1 
            && directionX == 1){
        directionX = -1;
    }
    else if(squarePointerX == 0 && directionX == -1){
        directionX = 1;
    }
    squarePointerX+=directionX;

    if(squarePointerY==VIEW_DOUBLEBUFFER_HEIGHT-VIEW_DOUBLEBUFFER_SQUARESIZE-1 
            && directionY == 1){
        directionY = -1;
    }
    else if(squarePointerY == 0 && directionY == -1){
        directionY = 1;
    }
    squarePointerY+=directionY;

    //abort this view?
    return !mouseClick();
}

void exitDoubleBuffer(void){
    cleanBitMap(doubleBufferScreen0);
    cleanBitMap(doubleBufferScreen1);
}

void drawRect(struct BitMap* bitmap, UBYTE planeIndex, 
        UWORD xPos, UWORD yPos, UWORD size){
    PLANEPTR plane = bitmap->Planes[planeIndex];
    UWORD startX = xPos / 8;
    UWORD endX = ((xPos + size) / 8) + 1;
    UWORD endY = yPos + size;
    UWORD x,y;
    UBYTE firstX = getFirstByte(xPos);
    UBYTE lastX = getLastByte(xPos + size);

    for(y=yPos; y<endY; y++){
        for(x=startX; x<endX; x++){
            if(x==startX){
                plane[y*bitmap->BytesPerRow + x] |= firstX;
            }
            else if(x==endX -1){
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
