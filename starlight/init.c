#include <stdio.h>
#include <stdlib.h>

#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <exec/types.h>

#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#include "starlight/starlight.h"

__far extern struct Custom custom;

UWORD olddmareq;
UWORD oldintena;
UWORD oldintreq;
UWORD oldadkcon;
ULONG oldview;
ULONG oldcopper;

/**
 * Disable Sprites, store old View, init logger.
 * Has to be called before using starlight framework. 
 */
void initStarlight(void){
    DOSBase = (struct DosLibrary*) OpenLibrary(DOSNAME, 0);
    if(DOSBase==0){
        exit(RETURN_ERROR);
    }
   
    GfxBase = (struct GfxBase*) OpenLibrary(GRAPHICSNAME, 0);
    if(GfxBase==0){
        printf("could not load %s\n", GRAPHICSNAME);
        exit(RETURN_ERROR);
    }
    
    oldview = (ULONG) GfxBase->ActiView;
    WaitTOF();
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
    OFF_SPRITE;
    initLog();
}

/**
 * Restores old View and frees memory. Must be called
 * before exiting. 
 */
void exitStarlight(void){
    // null view and double wait for dma before dma access
    WaitTOF();
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
    ON_SPRITE;

    // sprite dma working, now we can restore the workbench
    LoadView((struct View*) oldview); 
    WaitTOF();
    
    writeLogFS("Soft Starlight Shutdown successfully\n");

    // we switched back to the old view, so we can now delete
    // the previously created ones
    deleteAllViews();

    // final cleanup and we're gone
    CloseLibrary((struct Library*) GfxBase);
    CloseLibrary((struct Library*) DOSBase);
}
