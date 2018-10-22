#ifndef __GRAPHICS_CONTROLLER_H__
#define __GRAPHICS_CONTROLLER_H__

#include <exec/types.h>

#define MAX_VIEW_PORTS 5

#define BLACK 0x000
#define RED   0xf00
#define GREEN 0x0f0
#define BLUE  0x00f

void initPalView(void);
void addViewPort(struct BitMap *bitMap, UWORD *colortable, WORD colortableSize,
        WORD x, WORD y, WORD width, WORD height);
void startView(void);
void stopView(void);

#endif
