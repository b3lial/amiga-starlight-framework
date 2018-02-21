#include <stdio.h>
#include <stdlib.h>

#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/exec.h>

#include "register.h"
#include "register_dmacon.h"
#include "register_intena.h"
#include "utils.h"

UWORD olddmareq;
UWORD oldintena;
UWORD oldintreq;
UWORD oldadkcon;

ULONG oldview;
ULONG oldcopper;

void initSystem(void){
    //store data in hardwareregisters ORed with $8000 
    //(bit 15 is a write-set bit when values are written 
    //back into the system)
    olddmareq = REF_REG_16( DMACONR );
    olddmareq |= 0x8000;
    
    oldintena = REF_REG_16( INTENAR );
    oldintena |= 0x8000;
    
    oldintreq = REF_REG_16( INTREQR );
    oldintreq |= 0x8000;
    
    oldadkcon = REF_REG_16( ADKCONR );
    oldadkcon |= 0x8000;
   
    GfxBase = (struct GfxBase*) OpenLibrary(GRAPHICSNAME, 0);
    if(GfxBase==0){
        printf("could not load %s\n", GRAPHICSNAME);
        exit(RETURN_ERROR);
    }
    
    DOSBase = (struct DosLibrary*) OpenLibrary(DOSNAME, 0);
    if(DOSBase==0){
        printf("could not load %s\n", DOSNAME);
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
    REF_REG_16( DMACON ) = DMACON_SET | BLTPRI | BPLEN | COPEN | 
                           BLTEN | SPREN; //enable
    //REF_REG_16( DMACON ) = 0x1f;   //0b0000000000011111;
    REF_REG_16( DMACON ) = DSKEN | AUD3EN | AUD2EN | AUD1EN | AUD0EN; //disable
    
    //REF_REG_16( INTENA ) = 0xC000; //0b1100000000000000;
    REF_REG_16( INTENA ) = INTENA_SET | INTEN; //enable
    //REF_REG_16( INTENA ) = 0x3FFF; //0b0011111111111111;
    REF_REG_16( INTENA ) = EXTER | DSKSYN | RBF | AUD3 | AUD2 | AUD1 | 
                           AUD0 | BLIT | VERTB | COPER | PORTS | 
                           SOFT | DSKBLK | TBE; //disable

    initLog();
}

/**
 * Restore Interrupts, DMA configuration, Copper
 * and exit program.
 */
void exitSystem(BYTE errorCode){
    REF_REG_16( DMACON ) = 0x7fff;
    REF_REG_16( DMACON ) = olddmareq;
    REF_REG_16( INTENA ) = 0x7fff;
    REF_REG_16( INTENA ) = oldintena;
    REF_REG_16( INTREQ ) = 0x7fff;
    REF_REG_16( INTREQ ) = oldintreq;
    REF_REG_16( ADKCON ) = 0x7fff;
    REF_REG_16( ADKCON ) = oldadkcon;
    
    REF_REG_32( COP1LCH ) = oldcopper;
    
    LoadView((struct View*) oldview);
    WaitTOF();
    WaitTOF();
    WaitBlit();
    DisownBlitter();
    Permit();
    CloseLibrary((struct Library*) GfxBase);
    CloseLibrary((struct Library*) DOSBase);

    writeLogInt("Shutdown with return code %d\n", errorCode);
    exit(errorCode);
}
