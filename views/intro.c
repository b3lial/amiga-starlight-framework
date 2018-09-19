#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include <dos/dos.h>

#include "views/intro.h"
#include "graphics_controller.h"
#include "main.h"
#include "utils.h"
#include "init.h"

WORD payloadIntroState = VIEW_INTRO_INIT;
PLANEPTR bitplanes1[VIEW_INTRO_DEPTH];
PLANEPTR bitplanes2[VIEW_INTRO_DEPTH];

WORD fsmIntro(void){
    switch(payloadIntroState){
        case VIEW_INTRO_INIT:
            initIntro();
            payloadIntroState = VIEW_INTRO_RUNNING;
            break;

        case VIEW_INTRO_RUNNING:
            if(!executeIntro()){
                payloadIntroState = VIEW_INTRO_SHUTDOWN;
            }
            break;

        case VIEW_INTRO_SHUTDOWN:
            exitIntro();
            return MODULE_FINISHED;
    }
    
    return MODULE_CONTINUE;
}

void initIntro(void){
    struct BitMap bitMap1 = { 0 };
    struct BitMap bitMap2 = { 0 };
    UWORD colortable1[] = { BLACK, RED };
    UWORD colortable2[] = { BLACK, BLUE };
    
    UWORD i,j,k = 0;
    UBYTE patternColor = 0xff;
    UBYTE *displaymem1 = NULL;
    UBYTE *displaymem2 = NULL;

    //Create View and ViewExtra memory structures
    initPalView(); 

    //Create Bitmap and add Bitplanes
    InitBitMap(&bitMap1, VIEW_INTRO_DEPTH, VIEW_INTRO_WIDTH, 
            VIEW_INTRO_HEIGHT/2);
    //InitBitMap(&bitMap2, VIEW_INTRO_DEPTH, VIEW_INTRO_WIDTH, 
    //        VIEW_INTRO_HEIGHT/2);
    for(i=0; i<VIEW_INTRO_DEPTH; i++){
        bitplanes1[i] = NULL;
    //    bitplanes2[i] = NULL;
    }
    for(i=0; i<VIEW_INTRO_DEPTH; i++)
    {
        bitplanes1[i] = (PLANEPTR)AllocRaster(VIEW_INTRO_WIDTH, 
                VIEW_INTRO_HEIGHT/2);
    //    bitplanes2[i] = (PLANEPTR)AllocRaster(VIEW_INTRO_WIDTH, 
    //            VIEW_INTRO_HEIGHT/2);

        if (bitplanes1[i] == NULL /*|| bitplanes2[i] == NULL*/){
            writeLog("Error: Payload Intro, could not allocate BitPlanes\n");
            exitIntro();
            exitSystem(RETURN_ERROR); 
        }
        else {
            bitMap1.Planes[i] = bitplanes1[i];
    //        bitMap2.Planes[i] = bitplanes2[i];
        }
    }
    
    //Init Bitplanes with some Data
    for(i=0; i<VIEW_INTRO_DEPTH; i++){
        displaymem1 = bitMap1.Planes[i];
        //displaymem2 = bitMap2.Planes[i];

        for(j=0; j<VIEW_INTRO_HEIGHT/2; j++){
            if(j%8==0){
                if(patternColor==0){
                    patternColor=0xff;
                }
                else{
                    patternColor=0;
                }
            }

            for(k=0; k<VIEW_INTRO_WIDTH/8; k++){
                if(patternColor==0){
                    patternColor=0xff;
                }
                else{
                    patternColor=0;
                }
                displaymem1[j*(VIEW_INTRO_WIDTH/8) + k] = patternColor;
                //displaymem2[j*(VIEW_INTRO_WIDTH/8) + k] = patternColor;
            }
        }
    }

    //Use Bitplanes to create a ViewPort and add it to View
    addViewPort(&bitMap1, colortable1, VIEW_INTRO_COLORS, 
            0, 0, VIEW_INTRO_WIDTH, VIEW_INTRO_HEIGHT/2);
    //addViewPort(&bitMap2, colortable2, VIEW_INTRO_COLORS, 
    //        0, VIEW_INTRO_HEIGHT/2, VIEW_INTRO_WIDTH, VIEW_INTRO_HEIGHT/2);

    //Make View visible
    startView();
}

void cleanBitPlanes(PLANEPTR* bmPlanes, UBYTE bmDepth, 
        UWORD bmWidth, UWORD bmHeight)
{
    UBYTE i=0;
    for(i=0; i<bmDepth; i++){
        if((bmPlanes[i]) != NULL){
            writeLogInt("Freeing BitPlane memory %d\n", i);
            FreeRaster((bmPlanes[i]), bmWidth, bmHeight);
            bmPlanes[i] = NULL;
        }
    }
}

BOOL executeIntro(void){
    if(mouseClickDetected()){
        writeLog("Mouse click detected, stopping demo\n");
        return FALSE;
    }
    else{
        return TRUE;
    }
}

void exitIntro(void){
    stopView();

    cleanBitPlanes(bitplanes1, VIEW_INTRO_DEPTH, VIEW_INTRO_WIDTH, 
            VIEW_INTRO_HEIGHT/2);
    cleanBitPlanes(bitplanes2, VIEW_INTRO_DEPTH, VIEW_INTRO_WIDTH, 
            VIEW_INTRO_HEIGHT/2);
}
