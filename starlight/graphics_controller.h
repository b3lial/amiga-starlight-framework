#ifndef __GRAPHICS_CONTROLLER_H__
#define __GRAPHICS_CONTROLLER_H__

#include <exec/types.h>

#define MAX_VIEW_PORTS 5

#define BLACK     0x000
#define WHITE     0xfff
#define RED       0xf00
#define DARKRED   0x100
#define GREEN     0x0f0
#define BLUE      0x00f
#define DARKBLUE  0x001

struct DoubleBufferControl{
    BOOL active;
    UBYTE index;
    struct BitMap *bm0;    
    struct BitMap *bm1;
    struct cprlist *LOFCprList0;   
    struct cprlist *SHFCprList0;
    struct cprlist *LOFCprList1;   
    struct cprlist *SHFCprList1;
};

struct ViewData{
    struct ViewExtra *vextra;
    struct View *view;
    struct MonitorSpec *monspec;
    struct DoubleBufferControl dbControl;
    struct ViewPort *viewPorts[MAX_VIEW_PORTS];
    struct ViewPortExtra *viewPortExtras[MAX_VIEW_PORTS];
    struct RasInfo *rasInfos[MAX_VIEW_PORTS];
    struct ColorMap *colormaps[MAX_VIEW_PORTS];
};

void createNewView(void);
void addViewPort(struct BitMap *bitMap, struct BitMap *doubleBuffer, 
        void *colortable, UWORD colortableSize, BOOL useColorMap32,
		UWORD x, UWORD y, UWORD width, UWORD height, UWORD rxOffset,
        UWORD ryOffset);
void startView(void);
void deleteView(struct ViewData*);
void deleteAllViews(void);
void changeBuffer(UBYTE);

#endif
