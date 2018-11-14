#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <hardware/cia.h>

#include "utils.h"
#include "main.h"

BOOL mousePressed = FALSE;
char* logMessage = "Starlight Demo Logfile\n";
__far extern struct CIA ciaa;

BOOL mouseClick(void){
    if(!mousePressed && mouseCiaStatus()){
        mousePressed = TRUE;
        return FALSE;
    }
    else if (mousePressed && !mouseCiaStatus()){
        writeLog("Mouse click detected\n");
        mousePressed = FALSE;
        return TRUE;
    }
    else{
        return FALSE;
    }
}

BOOL mouseCiaStatus(void){
    if (ciaa.ciapra & CIAF_GAMEPORT0){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

#ifdef DEMO_DEBUG
BOOL initLog(void){
    BPTR logHandle = Open("ram:starlight-demo.log", MODE_NEWFILE);
    if(logHandle==NULL){
        return FALSE;
    }
    
    Write(logHandle, logMessage, strlen(logMessage));
    
    Close(logHandle);
    return TRUE;
}

/**
 * Write format string into logfile
 */
BOOL writeLogFS(const char* formatString, ... ){
    char str[DEMO_STR_MAX];
    va_list args;
    va_start( args, formatString );
    vsprintf( str , formatString, args );
    va_end( args );
    return writeLog(str);
}

/**
 * Write string into logfile
 */
BOOL writeLog(char* msg){
    BPTR logHandle = Open("ram:starlight-demo.log", MODE_OLDFILE);
    if(logHandle==NULL){
        return FALSE;
    }
    
    Seek(logHandle, 0, OFFSET_END);
    Write(logHandle, msg, strlen(msg));
    
    Close(logHandle);
    return TRUE;
}

/**
 * Writes msg and a character array into log file. Maybe not the most
 * performant implementation but who cares.
 */
BOOL writeArrayLog(char* msg, unsigned char* array, UWORD array_length){
    UWORD i;

    if(array_length<2){
        return FALSE;
    }
    if(!writeLog(msg)){
        return FALSE;
    }

    for(i=0; i<array_length-1; i++){
        if(i%5!=0){
            writeLogFS("0x%x, ", array[i]); 
        }
        else{
            writeLogFS("0x%x\n", array[i]); 
        }
    }
    writeLogFS("0x%x\n", array[array_length-1]); 
    return TRUE;
}

#else
#pragma GCC diagnostic ignored "-Wunused-parameter"
BOOL initLog(void){
    return TRUE;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
BOOL writeLogFS(const char* formatString, ...){
    return TRUE;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
BOOL writeLog(char* msg){
    return TRUE;
}

BOOL writeArrayLog(char* msg, unsigned char* array, UWORD array_length){
    return TRUE;
}
#endif

/**
 * Use FreeRaster to free an array of BitPlane memory
 */
void cleanBitPlanes(PLANEPTR* bmPlanes, UBYTE bmDepth, 
        UWORD bmWidth, UWORD bmHeight)
{
    UBYTE i=0;
    for(i=0; i<bmDepth; i++){
        if((bmPlanes[i]) != NULL){
            writeLogFS("Freeing BitPlane memory %d\n", i);
            FreeRaster((bmPlanes[i]), bmWidth, bmHeight);
            bmPlanes[i] = NULL;
        }
    }
}
