#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "register.h" 
#include "starlight.h"

char* logMessage = "Starlight Demo Logfile\n";

void waitForMouseClick(void){
    UBYTE ciaapra = FIR0;
    while(ciaapra & FIR0){
        ciaapra = *( (volatile UBYTE*) (CIAAPRA) );
    }
}

#ifdef DEMO_DEBUG
int initLog(void){
    BPTR logHandle = Open("ram:starlight-demo.log", MODE_NEWFILE);
    if(logHandle==NULL){
        return 0;
    }
    
    Write(logHandle, logMessage, strlen(logMessage));
    
    Close(logHandle);
    return 1;    
}

int writeLogInt(const char* formatString, int n){
    char str[DEMO_STR_MAX];
    sprintf(str, formatString, n);
    return writeLog(str);
}

int writeLog(char* msg){
    BPTR logHandle = Open("ram:starlight-demo.log", MODE_OLDFILE);
    if(logHandle==NULL){
        return 0;
    }
    
    Seek(logHandle, 0, OFFSET_END);
    Write(logHandle, msg, strlen(msg));
    
    Close(logHandle);
    return 1;
}
#else
#pragma GCC diagnostic ignored "-Wunused-parameter"
int initLog(void){
    return 1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
int writeLogInt(const char* formatString, int n){
    return 1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
int writeLog(char* msg){
    return 1;
}
#endif
