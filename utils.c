#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <hardware/cia.h>

#include "utils.h"
#include "main.h"

char* logMessage = "Starlight Demo Logfile\n";
__far extern struct CIA ciaa;

BOOL mouseClickDetected(void){
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
#endif
