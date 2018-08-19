#include <string.h>
#include <stdio.h>

#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <hardware/cia.h>

#include "utils.h"
#include "starlight.h"

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

BOOL writeLogInt(const char* formatString, int n){
    char str[DEMO_STR_MAX];
    sprintf(str, formatString, n);
    return writeLog(str);
}

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
BOOL writeLogInt(const char* formatString, int n){
    return TRUE;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
BOOL writeLog(char* msg){
    return TRUE;
}
#endif
