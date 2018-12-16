#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <proto/graphics.h>
#include <proto/exec.h>

#include "starlight/starlight.h"

/**
 * Allocates memory for bitmap and its bitplanes
 */
struct BitMap* createBitMap(UBYTE depth, UWORD width, UWORD height){
    struct BitMap* newBitMap;
    BYTE i,j = 0;

    writeLogFS("Allocating memory for %dx%dx%d BitMap\n", depth, width, height);
    //Alloc BitMap structure and init with zero 
    newBitMap = AllocMem(sizeof(struct BitMap), MEMF_ANY);
    if(!newBitMap){
        writeLogFS("Error: Could not allocate Bitmap memory\n");
        return NULL;
    }
    memset(newBitMap, 0, sizeof(struct BitMap));
    InitBitMap(newBitMap, depth, width, height);

    for(i=0; i<depth; i++){
        newBitMap->Planes[i] = (PLANEPTR) AllocRaster(
                (newBitMap->BytesPerRow) * 8, newBitMap->Rows);
        if(newBitMap->Planes[i] == NULL){
            //error, free previously allocated memory
            writeLogFS("Error: Could not allocate Bitplane %d memory\n", i);
            for(j=i-1; j>=0; j--){
                FreeRaster(newBitMap->Planes[j], (newBitMap->BytesPerRow) * 8, 
                    newBitMap->Rows);
            }
            FreeMem(newBitMap, sizeof(struct BitMap));
            return NULL;
        }
    }

    return newBitMap;
}

/**
 * Free BitMMap memory and its BitPlanes
 */
void cleanBitMap(struct BitMap* bitmap){
    cleanBitPlanes(bitmap->Planes, bitmap->Depth, (bitmap->BytesPerRow)*8,
            bitmap->Rows);
    writeLogFS("Freeing %d bytes of BitMap memory\n", sizeof(struct BitMap));
    FreeMem(bitmap, sizeof(struct BitMap));
}

/**
 * Use FreeRaster to free an array of BitPlane memory
 */
void cleanBitPlanes(PLANEPTR* bmPlanes, UBYTE bmDepth, 
        UWORD bmWidth, UWORD bmHeight)
{
    UBYTE i=0;
    for(i=0; i<bmDepth; i++){
        if((bmPlanes[i]) != NULL){
            writeLogFS("Freeing BitPlane %d with size of %d bytes\n", 
                    i, bmWidth*bmHeight/8);
            FreeRaster(bmPlanes[i], bmWidth, bmHeight);
            bmPlanes[i] = NULL;
        }
    }
}
