#ifndef __REGISTER_H__
#define __REGISTER_H__

#define REF_REG_32(x)  *( (volatile ULONG*) (BASE + x) )
#define REF_REG_16(x)  *( (volatile UWORD*) (BASE + x) )
#define REF_REG_8(x)   *( (volatile UBYTE*) (BASE + x) )

#define BASE           0xDFF000

#define FMODE		   0x1FC
#define BPLCON0		   0x100
#define COP1LCH		   0x080
#define INTENA		   0x09A
#define INTENAR		   0x01C
#define VPOSR		   0x004
#define VHPOSR 		   0x006
#define COLOR00 	   0x180
#define COLOR01 	   0x182
#define COLOR02 	   0x184
#define COLOR03 	   0x186
#define COLOR04 	   0x188
#define COLOR05 	   0x18A
#define COLOR06 	   0x18C
#define COLOR07 	   0x18E
#define COLOR08 	   0x190
#define COLOR09 	   0x192
#define COLOR10 	   0x194
#define COLOR11 	   0x196
#define COLOR12 	   0x198
#define COLOR13 	   0x19A
#define COLOR14 	   0x19C
#define COLOR15 	   0x19E
#define COLOR16        0x1A0
#define COLOR17        0x1A2
#define COLOR18        0x1A4
#define COLOR19        0x1A6
#define COLOR20 	   0x1A8
#define COLOR21 	   0x1AA
#define COLOR22 	   0x1AC
#define COLOR23 	   0x1AE
#define COLOR24 	   0x1B0
#define COLOR25 	   0x1B2
#define COLOR26        0x1B4
#define COLOR27        0x1B6
#define COLOR28        0x1B8
#define COLOR29        0x1BA
#define COLOR30        0x1BC
#define COLOR31        0x1BE
#define DIWSTRT    	   0x08E
#define DIWSTOP        0x090
#define DDFSTRT 	   0x092
#define DDFSTOP		   0x094
#define BPL1MOD        0x108
#define BPL2MOD		   0x10A
#define DMACON		   0x096
#define DMACONR		   0x002
#define ADKCON		   0x09e
#define ADKCONR		   0x010
#define INTREQR		   0x01e
#define INTREQ		   0x09c
#define BPL1PTH        0x0e0
#define BPL1PTL        0x0e2
#define BPL2PTH        0x0e4
#define BPL2PTL        0x0e6
#define BPL3PTH        0x0e8
#define BPL3PTL        0x0ea
#define BPL4PTH        0x0ec
#define BPL4PTL        0x0ee
#define BPL5PTH        0x0f0
#define BPL5PTL        0x0f2

#define SPR0PTH        0x120
#define SPR0PTL        0x122
#define SPR1PTH        0x124
#define SPR1PTL        0x126
#define SPR2PTH        0x128
#define SPR2PTL        0x12A
#define SPR3PTH        0x12C
#define SPR3PTL        0x12E
#define SPR4PTH        0x130
#define SPR4PTL        0x132
#define SPR5PTH        0x134
#define SPR5PTL        0x136
#define SPR6PTH        0x138
#define SPR6PTL        0x13A
#define SPR7PTH        0x13C
#define SPR7PTL        0x13E

#define BLTCON0        0x040
#define BLTCON1        0x042
#define BLTAFWM        0x044
#define BLTALWM        0x046
#define BLTCPTH        0x048
#define BLTCPTL        0x04A
#define BLTBPTH        0x04C
#define BLTBPTL        0x04E
#define BLTAPTH        0x050
#define BLTAPTL        0x052
#define BLTDPTH        0x054
#define BLTDPTL        0x056
#define BLTSIZE        0x058
#define BLTCON0L       0x05A
#define BLTSIZV        0x05C
#define BLTSIZH        0x05E
#define BLTCMOD        0x060
#define BLTBMOD        0x062
#define BLTAMOD        0x064
#define BLTDMOD        0x066
#define BLTCDAT        0x070
#define BLTBDAT        0x072
#define BLTADAT        0x074

#define CIAAPRA		   0xBFE001

#endif
