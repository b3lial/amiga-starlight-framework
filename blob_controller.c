#include <proto/dos.h>
#include "blob_controller.h"
#include "utils.h"

struct BitMap* loadBlob(const char* fileName, UBYTE depth, UWORD width, UWORD height){
    LONG fileSize = 0;

    BPTR blobFileHandle = Open(fileName, MODE_OLDFILE);
    if(!blobFileHandle){
        writeLogFS("Error: Could not read %s\n", fileName);
        return NULL;
    }
    Seek(blobFileHandle, 0, OFFSET_END);
    fileSize = Seek(blobFileHandle, 0, OFFSET_BEGINNING);
    writeLogFS("Blob %s has size %d\n", fileName, fileSize);
    Close(blobFileHandle);

    //BAUSTELLE

    return NULL;
}
