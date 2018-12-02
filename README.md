# amiga-starlight-framework
C Framework on top of AmigaOS which provides APIs for easy View/ViewPort creation and logging functionality. 

## Description
As a kid, I loved the Amiga but was not able to write code for this platform. In 2017, I planned a little demo project 
and read lots of tutorials. Nearly everybody used assembler and direct hardware access. Although this produced amazing demos,
I wanted to try something different: Using a C compiler and the AmigaOS APIs. To speed up development, I am working at 
this little framework which allows me to focus on the demo effects and not the AmigaOS.

## Features
Main.c contains a demo project which uses the Starlight Framework. It is implemented as a finite state machine because I
wanted an easy way to concatenate different effects (text scroller, rotating cube, etc). Each effect is a seperate 
[View](http://wiki.amigaos.net/wiki/Classic_Graphics_Primitives). The first effect is called twoplanes and resides in 
views/twoplanes.c. It creates two ViewPorts which display a chessboard and waits for a mouse click to switch
to the next View ballblob. Ballblob loads a boing ball image into memory and displays it on screen.  
The following features are provided by my framework:

### Initialisation
The method **initSystem(BOOL softInit)** takes over the graphical hardware. It has two modes:
* *softInit*: Stores the old View (usually the workbench screen), switches to a blank screen, loads neccessary libraries
and deactivates sprites. The Amiga operating system is still running.
* *ruthless*: Stores the old View (usually the workbench screen), its copper list, switches to a blank screen, loads 
neccessary libraries and deactivates sprites. The AmigaOS is halted (corresponding interrupts are disabled) and DMA channels 
are reprogrammed. This is the only part of the framework which needs direct hardware access and its there due to historical
reasons.

When your program has finished, a call to **exitSystemSoft(BYTE errorCode)** restores the Workbench screen.

### Graphics
The graphics controller allows you to create a view, add ViewPorts to the View, display the View and destroy the View freeing
the previously allocated memory:
* **initView(void)**: Creates a low resolution screen.
* **addViewPort(struct BitMap *bitMap, UWORD *colortable, WORD colortableSize, WORD x, WORD y, WORD width, WORD height)**: Adds
a ViewPort to the View. Parameters are the raster itself, its color table, position of the raster on screen and its size.
* **startView(void)**: Merges the copper list and displays the previously created View.
* **stopView(void)**: Frees memory and destroys the current View.
* **struct BitMap\* createBitMap(UBYTE depth, UWORD width, UWORD height)**: Allocate memory for BitMap structure and its bitplanes.
* **void cleanBitMap(struct BitMap\*)**: Free memory of a BitMap structure and its bitplanes. 
* **void cleanBitPlanes(PLANEPTR\* planes, UBYTE depth, UWORD width, UWORD height)**: Free bitplane array memory. Called by **cleanBitMap()**.

### Bitmaps
You can load non-interlaced images as Bitmaps into memory and blit the results into your ViewPorts:
* **struct BitMap\* loadBlob(const char\* image, UBYTE size, UWORD width, UWORD height)**: Allocate memory for a BitMap structure. 
Load the image into this BitMap and return it to caller.
* **BOOL loadColorMap(char\* fileName, UWORD\* map, UBYTE mapLength)**: Load a set of bytes from file **fileName** into **map** 
which represents a color map for a previously loaded image. 

### Logging
The method **initLog(void)** creates the logfile *ram:starlight-demo.log*. You can add new lines with **writeLog(char\* msg)**
and **writeLogFS(const char\* formatString, ...)**. A sequence of bytes can be written to the log file using
**writeArrayLog(char\* logmsg, unsigned char\* array, UWORD length)**.

## Download Binary
You can download the the compilation result of the current master
branch [here](https://s3.eu-central-1.amazonaws.com/codebuild-eu-central-1-4040-3447-3876-output-bucket/amiga/starlight-gcc-build).
Packaging not yet implemented on my build server, therefore you have to add ball_207_207_3.RAW and ball_207_207_3.CMAP before
execution.

## Build
The Makefile works with [GCC](http://aminet.net/package/dev/gcc/m68k-amigaos-gcc). The code can be compiled with SAS-C as well.
