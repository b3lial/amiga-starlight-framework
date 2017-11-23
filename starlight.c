#include "starlight.h"
#include "init.h"
#include "register.h"
#include "utils.h"

int main(void)
{
    initSystem();
    
    waitForMouseClick();

    exitSystem();
}
