#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <string.h>

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

int initLog(void){
#ifdef DEMO_DEBUG
#ifdef __SASC
    BPTR logHandle = Open("ram:starlight-demo.log", MODE_NEWFILE);
#else
    BPTR logHandle = Open((const unsigned char*) "ram:starlight-demo.log", MODE_NEWFILE);
#endif
    if(logHandle==NULL){
        return 0;
    }
    
    Write(logHandle, logMessage, strlen(logMessage));
    
    Close(logHandle);
#endif
    return 1;    
}

int writeLogInt(char* formatString, int n){
    //TODO: Baustelle
    char str[DEMO_STR_MAX];
    return 1;
}

int writeLog(char* msg){
#ifdef DEMO_DEBUG
#ifdef __SASC
    BPTR logHandle = Open("ram:starlight-demo.log", MODE_OLDFILE);
#else
    BPTR logHandle = Open((const unsigned char*) "ram:starlight-demo.log", MODE_OLDFILE);
#endif
    if(logHandle==NULL){
        return 0;
    }
    
    Seek(logHandle, 0, OFFSET_END);
    Write(logHandle, msg, strlen(msg));
    
    Close(logHandle);
#endif
    return 1;
}
