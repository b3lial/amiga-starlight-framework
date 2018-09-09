#ifndef __GRAPHICS_CONTROLLER_H__
#define __GRAPHICS_CONTROLLER_H__

#define MAX_VIEW_PORTS 5

void initPalView(void);
void addViewPort(struct BitMap *bitMap);
void startView(void);
void stopView(void);

#endif
