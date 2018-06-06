#ifndef __INIT_H__
#define __INIT_H__

#include <exec/types.h>

void initSystem(BOOL softInit);
void exitSystem(BYTE);

void initSystemRuthless(void);
void exitSystemRuthless(BYTE);

void initSystemSoft(void);
void exitSystemSoft(BYTE);

#endif
