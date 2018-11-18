#include <proto/dos.h>
#include <proto/graphics.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <string.h>

#include "blob_controller.h"
#include "graphics_controller.h"
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
    writeLogFS("Blob %s has file size %d\n", fileName, fileSize);
    
    //Copy file content to raster
    blobBitMap = createBitMap(depth, width, height);
    if(blobBitMap == NULL){
        return NULL;
    }
    planeSize = (blobBitMap->Rows) * (blobBitMap->BytesPerRow);
    writeLogFS("Calculated plane size: %d\n", planeSize);
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
