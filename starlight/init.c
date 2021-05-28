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

//remember init method chosen by user
BOOL hasChosenSoftInit = FALSE;

/**
 * Disable Sprites, store old View, init logger.
 * Has to be called before using starlight framework. 
 */
void initStarlight(BOOL softInit){
    hasChosenSoftInit = softInit;
    if(softInit){
        initSystemSoft();
    }
    else{
        initSystemRuthless();
    }
}

/**
 * Restores old View and frees memory. Must be called
 * before exiting. 
 */
void exitStarlight(BYTE errorCode){
    if(hasChosenSoftInit){
        exitSystemSoft();
    }
    else{
        exitSystemRuthless();
    }

    // we switched back to the old view, so we can now delete
    // the previously created ones
    deleteAllViews();

    // final cleanup and we're gone
    CloseLibrary((struct Library*) GfxBase);
    CloseLibrary((struct Library*) DOSBase);
    exit(errorCode);
}

/**
 * Init system the nice way: Keep AmigaOS running,
 * just load necessary libs, disable sprite dma and store old View
 */
void initSystemSoft(void){
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
 * Init system the hard way with direct hardware access: 
 * Load necessary libraries, store old copper, old, disable interrupts 
 * and unused DMA
 */
void initSystemRuthless(void){
    //store data in hardwareregisters ORed with $8000 
    //(bit 15 is a write-set bit when values are written 
    //back into the system)
    olddmareq = custom.dmaconr;
    olddmareq |= 0x8000;
    
    oldintena = custom.intenar;
    oldintena |= 0x8000;
    
    oldintreq = custom.intreqr;
    oldintreq |= 0x8000;
    
    oldadkcon = custom.adkconr;
    oldadkcon |= 0x8000;
   
    DOSBase = (struct DosLibrary*) OpenLibrary(DOSNAME, 0);
    if(DOSBase==0){
        exit(RETURN_ERROR);
    }

    GfxBase = (struct GfxBase*) OpenLibrary(GRAPHICSNAME, 0);
    if(GfxBase==0){
        printf("could not load %s\n", GRAPHICSNAME);
        exit(RETURN_ERROR);
    }
    
    oldview = *( (ULONG*) (&(((UBYTE*) GfxBase)[34])) );
    oldcopper = *( (ULONG*) (&(((UBYTE*) GfxBase)[38])) );
    
    WaitTOF();
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
    OwnBlitter();
    WaitBlit();
    Forbid();
   
    //set custom dma
    custom.dmacon = DMAF_SETCLR | DMAF_BLITHOG | DMAF_RASTER | DMAF_COPPER | 
        DMAF_BLITTER | DMAF_SPRITE; 
    custom.dmacon = DMAF_DISK | DMAF_AUD3 | DMAF_AUD2 | DMAF_AUD1 | 
        DMAF_AUD0; 
    
    //set custom interrupts
    custom.intena =  INTB_SETCLR | INTB_INTEN; 
    custom.intena = INTB_EXTER | INTB_DSKSYNC | INTB_RBF | INTB_AUD3 | 
        INTB_AUD2 | INTB_AUD1 | INTB_AUD0 | INTB_BLIT | INTB_VERTB | 
        INTB_COPER | INTB_PORTS | INTB_SOFTINT | INTB_DSKBLK | INTB_TBE; 

    initLog();
}

/**
 * Restore old view and exit program
 */
void exitSystemSoft(void){
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
}

/**
 * Restore Interrupts, DMA configuration, Copper
 * and exit program.
 */
void exitSystemRuthless(void){
    // null view and double wait for dma before dma access
    WaitTOF();
    LoadView(NULL);
    WaitTOF();
    WaitTOF();

    // activate dma and interrupts
    custom.dmacon = 0x7fff;
    custom.dmacon = olddmareq;
    custom.intena = 0x7fff;
    custom.intena = oldintena;
    custom.intreq = 0x7fff;
    custom.intreq = oldintreq;
    custom.adkcon = 0x7fff;
    custom.adkcon = oldadkcon;
    custom.cop1lc = oldcopper;
    
    // free blitter
    WaitBlit();
    DisownBlitter();
    Permit();

    // os is running again, we can restore workbench screen now
    WaitTOF();
    LoadView((struct View*) oldview);
    WaitTOF();

    writeLogFS("Ruthless Starlight Shutdown successfully\n");
}
