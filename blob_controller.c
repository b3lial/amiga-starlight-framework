#include <proto/dos.h>
#include <proto/graphics.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include "blob_controller.h"
#include "utils.h"

/**
 * Load non-interlaced graphic blob from file system and copy it into bitplances
 */
struct BitMap* loadBlob(const char* fileName, UBYTE depth, UWORD width, 
        UWORD height){
    LONG fileSize, planeSize, dataRead = 0;
    UBYTE i = 0;
    struct BitMap* blobBitMap = NULL;
    BPTR blobFileHandle = NULL;
    writeLogFS("Trying to load blob %s\n", fileName);

    //Open input file
    blobFileHandle = Open(fileName, MODE_OLDFILE);
    if(!blobFileHandle){
        writeLogFS("Error: Could not read %s\n", fileName);
        return NULL;
    }

    //Get file size
    Seek(blobFileHandle, 0, OFFSET_END);
    fileSize = Seek(blobFileHandle, 0, OFFSET_BEGINNING);
    writeLogFS("Blob %s has size %d\n", fileName, fileSize);
    
    //Copy file content to raster
    blobBitMap = createBitMap(depth, width, height);
    if(blobBitMap == NULL){
        return NULL;
    }
    planeSize = (width*height)/8;
    for(i=0; i<depth; i++){
        dataRead = Read(blobFileHandle, blobBitMap->Planes[i], planeSize); 
        if(dataRead==-1){
            writeLogFS("Error: Could not read from Blob input file\n");
            Close(blobFileHandle);
            return NULL;
        }
        else if(dataRead==0){
            writeLogFS("All data read at bitplane %d\n", i);
            break;
        }
        else{
            writeLogFS("Read %d bytes for bitplane %d\n", dataRead, i);
            continue;
        }
    }

    Close(blobFileHandle);
    return blobBitMap;
}

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
        newBitMap->Planes[i] = (PLANEPTR) AllocRaster(width, height);
        if(newBitMap->Planes[i] == NULL){
            //error, free previously allocated memory
            writeLogFS("Error: Could not allocate Bitplane %d memory\n", i);
            for(j=i-1; j>=0; j--){
                FreeRaster(newBitMap->Planes[j], width, height);
            }
            FreeMem(newBitMap, sizeof(struct BitMap));
            return NULL;
        }
    }

    return newBitMap;
}
