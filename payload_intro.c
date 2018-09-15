#include <stdio.h>
#include <exec/types.h>
#include <proto/graphics.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>

#include <dos/dos.h>

#include "graphics_controller.h"
#include "payload_intro.h"
#include "starlight.h"
#include "utils.h"
#include "init.h"

WORD payloadIntroState = PAYLOAD_INTRO_INIT;
PLANEPTR bitplanes[PAYLOAD_INTRO_DEPTH];

WORD fsmPayloadIntro(void){
    switch(payloadIntroState){
        case PAYLOAD_INTRO_INIT:
            initPayloadIntro();
            payloadIntroState = PAYLOAD_INTRO_RUNNING;
            break;

        case PAYLOAD_INTRO_RUNNING:
            if(!executePayloadIntro()){
                payloadIntroState = PAYLOAD_INTRO_SHUTDOWN;
            }
            break;

        case PAYLOAD_INTRO_SHUTDOWN:
            exitPayloadIntro();
            return MODULE_FINISHED;
    }
    
    return MODULE_CONTINUE;
}

void initPayloadIntro(void){
    struct BitMap bitMap = { 0 };
    UWORD colortable[] = { BLACK, RED, GREEN, BLUE };
    
    UWORD i,j,k = 0;
    UBYTE patternColor = 0xff;
    UBYTE *displaymem = NULL;

    //Create View and ViewExtra memory structures
    initPalView(); 

    //Create Bitmap and add Bitplanes
    InitBitMap(&bitMap, PAYLOAD_INTRO_DEPTH, PAYLOAD_INTRO_WIDTH, 
            PAYLOAD_INTRO_HEIGHT);
    for(i=0; i<PAYLOAD_INTRO_DEPTH; i++){
        bitplanes[i] = NULL;
    }
    for(i=0; i<PAYLOAD_INTRO_DEPTH; i++)
    {
        bitplanes[i] = (PLANEPTR)AllocRaster(PAYLOAD_INTRO_WIDTH, 
                PAYLOAD_INTRO_HEIGHT);

        if (bitplanes[i] == NULL){
            writeLog("Error: Payload Intro, could not allocate BitPlanes\n");
            exitPayloadIntro();
            exitSystem(RETURN_ERROR); 
        }
        else {
            bitMap.Planes[i] = bitplanes[i];
        }
    }
    
    //Init Bitplanes with some Data
    for(i=0; i<PAYLOAD_INTRO_DEPTH; i++){
        displaymem = bitMap.Planes[i];
        for(j=0; j<PAYLOAD_INTRO_HEIGHT; j++){
            if(j%8==0){
                if(patternColor==0){
                    patternColor=0xff;
                }
                else{
                    patternColor=0;
                }
            }

            for(k=0; k<PAYLOAD_INTRO_WIDTH/8; k++){
                if(patternColor==0){
                    patternColor=0xff;
                }
                else{
                    patternColor=0;
                }
                displaymem[j*(PAYLOAD_INTRO_WIDTH/8) + k] = patternColor;
            
            }
        }
    }

    //Use Bitplanes tto create a ViewPort and add it to View
    addViewPort(&bitMap, colortable, PAYLOAD_INTRO_COLORS, 
            0, 0, PAYLOAD_INTRO_WIDTH, PAYLOAD_INTRO_HEIGHT);

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

BOOL executePayloadIntro(void){
    if(mouseClickDetected()){
        writeLog("Mouse click detected, stopping demo\n");
        return FALSE;
    }
    else{
        return TRUE;
    }
}

void exitPayloadIntro(void){
    stopView();

    cleanBitPlanes(bitplanes, PAYLOAD_INTRO_DEPTH, PAYLOAD_INTRO_WIDTH, 
            PAYLOAD_INTRO_HEIGHT);
}
