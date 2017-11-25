#include "utils.h"
#include "register.h" 
#include "customstdint.h"

void waitForMouseClick(void){
    uint8_t ciaapra = FIR0;
    while(ciaapra & FIR0){
        ciaapra = *( (volatile uint8_t*) (CIAAPRA) );
    }
}
