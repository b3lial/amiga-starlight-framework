#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <stdio.h>

#include "include/types.h"
#include "include/register.h"

uint16_t olddmareq;
uint16_t oldintena;
uint16_t oldintreq;
uint16_t oldadkcon;

uint32_t oldview;
uint32_t oldcopper;

const char* graphics_library_name = "graphics.library";
struct Library *gfxbase = 0;

int initSystem(){
    //store data in hardwareregisters ORed with $8000 
	//(bit 15 is a write-set bit when values are written 
	//back into the system)
    olddmareq = REF_REG_16( DMACONR );
    printf("storing DMACONR: 0x%x\n", olddmareq);
    olddmareq |= 0x8000;
    
    oldintena = REF_REG_16( INTENAR );
    printf("storing INTENAR: 0x%x\n", oldintena);
    oldintena |= 0x8000;
    
    oldintreq = REF_REG_16( INTREQR );
    printf("storing INTREQR: 0x%x\n", oldintreq);
    oldintreq |= 0x8000;
    
    oldadkcon = REF_REG_16( ADKCONR );
    printf("storing ADKCONR: 0x%x\n", oldadkcon);
    oldadkcon |= 0x8000;
    
    gfxbase = OpenLibrary((const unsigned char*) graphics_library_name, 0);
    if(gfxbase==0){
        printf("could not load %s\n", graphics_library_name);
        return 0;
    }
    else{
        printf("found %s at: 0x%x\n", graphics_library_name, gfxbase);
    }
    
    oldview = *( (uint32_t*) (&(((uint8_t*) gfxbase)[34])) );
    printf("storing oldview: 0x%x\n", oldview);
    oldcopper = *( (uint32_t*) (&(((uint8_t*) gfxbase)[38])) );
    printf("storing oldcopper: 0x%x\n", oldcopper);
    
    LoadView(0);
    WaitTOF();
    WaitTOF();
    OwnBlitter();
    WaitBlit();
    Forbid();
    
    REF_REG_16( DMACON ) = 0b1000010111100000;
    REF_REG_16( DMACON ) = 0b0000000000011111;
    REF_REG_16( INTENA ) = 0b1100000000000000;
    REF_REG_16( INTENA ) = 0b0011111111111111;
    
    //exit gracefully
    return 1;
}

void exitSystem(){
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
}
