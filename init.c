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

#include "init.h"
#include "utils.h"

__far extern struct Custom custom;

UWORD olddmareq;
UWORD oldintena;
UWORD oldintreq;
UWORD oldadkcon;
ULONG oldview;
ULONG oldcopper;

//remember init method chosen by user
BOOL hasChosenSoftInit = FALSE;

void initSystem(BOOL softInit){
    hasChosenSoftInit = softInit;
    if(softInit){
        initSystemSoft();
    }
    else{
        initSystemRuthless();
    }
}

void exitSystem(BYTE errorCode){
    if(hasChosenSoftInit){
        exitSystemSoft(errorCode);
    }
    else{
        exitSystemRuthless(errorCode);
    }
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
    
    WaitTOF();
    OFF_SPRITE;
    oldview = (ULONG) GfxBase->ActiView;
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
    
    LoadView(0);
    WaitTOF();
    WaitTOF();
    OwnBlitter();
    WaitBlit();
    Forbid();
    
    //REF_REG_16( DMACON ) = 0x85e0; //0b1000010111100000;
    custom.dmacon = DMAF_SETCLR | DMAF_BLITHOG | DMAF_RASTER | DMAF_COPPER | 
        DMAF_BLITTER | DMAF_SPRITE; 
    //REF_REG_16( DMACON ) = 0x1f;   //0b0000000000011111;
    custom.dmacon = DMAF_DISK | DMAF_AUD3 | DMAF_AUD2 | DMAF_AUD1 | 
        DMAF_AUD0; 
    
    //REF_REG_16( INTENA ) = 0xC000; //0b1100000000000000;
    custom.intena =  INTB_SETCLR | INTB_INTEN; 
    //REF_REG_16( INTENA ) = 0x3FFF; //0b0011111111111111;
    custom.intena = INTB_EXTER | INTB_DSKSYNC | INTB_RBF | INTB_AUD3 | 
        INTB_AUD2 | INTB_AUD1 | INTB_AUD0 | INTB_BLIT | INTB_VERTB | 
        INTB_COPER | INTB_PORTS | INTB_SOFTINT | INTB_DSKBLK | INTB_TBE; 

    initLog();
}

/**
 * Restore old view and exit program
 */
void exitSystemSoft(BYTE errorCode){
    WaitTOF();
    ON_SPRITE;
    LoadView((struct View*) oldview); 
    
    writeLogInt("Soft reset shutdown with return code %d\n", errorCode);
    CloseLibrary((struct Library*) GfxBase);
    CloseLibrary((struct Library*) DOSBase);
    exit(errorCode);
}

/**
 * Restore Interrupts, DMA configuration, Copper
 * and exit program.
 */
void exitSystemRuthless(BYTE errorCode){
    custom.dmacon = 0x7fff;
    custom.dmacon = olddmareq;
    custom.intena = 0x7fff;
    custom.intena = oldintena;
    custom.intreq = 0x7fff;
    custom.intreq = oldintreq;
    custom.adkcon = 0x7fff;
    custom.adkcon = oldadkcon;
    
    custom.cop1lc = oldcopper;
    
    LoadView((struct View*) oldview);
    WaitTOF();
    WaitTOF();
    WaitBlit();
    DisownBlitter();
    Permit();

    writeLogInt("Ruthless reset shutdown with return code %d\n", errorCode);
    CloseLibrary((struct Library*) GfxBase);
    CloseLibrary((struct Library*) DOSBase);
    exit(errorCode);
}
