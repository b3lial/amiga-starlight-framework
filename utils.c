#include <exec/types.h>

#include "utils.h"
#include "register.h" 

void waitForMouseClick(void){
    UBYTE ciaapra = FIR0;
    while(ciaapra & FIR0){
        ciaapra = *( (volatile UBYTE*) (CIAAPRA) );
    }
}
