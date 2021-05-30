# amiga-starlight-framework
C Framework on top of AmigaOS to setup screen, video buffer, load images from file and logging functionality.
Avoids direct hardware access and uses AmigaOS APIs.

## Description
As a kid, I loved the Amiga but was not able to write code for this platform. In 2017, I planned a little demo project 
and read lots of tutorials. Nearly everybody used assembler and direct hardware access. Although this produced amazing demos,
I wanted to try something different: Using a C compiler and the AmigaOS APIs. To speed up development, I am working at 
this little framework which allows me to 

* set up the screen
* create and display video buffers 
* load images
* etc. 

with a few lines of C code and therefore to focus on the demo effects and not the boilerplate code.

## Structure
This Git repository contains the framework and an example project. 
The framework itself resides in the __starlight__ folder. 
Main.c in root directory contains a demo project which uses Starlight to display some
graphics and animations. It is implemented as a finite state machine because I
wanted an easy way to concatenate different effects (text scroller, rotating cube, etc). Each effect is a seperate 
[View](http://wiki.amigaos.net/wiki/Classic_Graphics_Primitives) and called after a mouse click:

* TwoPlanes demonstrates how to create two ViewPorts and change color sets at runtime.
* Ballblob loads a boing ball image into memory and displays it on screen. It demonstrates image loading from file.
* DoubleBuffer moves a square around the screen. It demonstrates how to use double video buffering.

## General Usage
The main idea of Starlight is

* Initialise the framework
* Create a new __View__ for your demo effect
* Allocate __BitPlanes__ which represent your video buffer
* Allocate color table arrays
* Create a set of __ViewPorts__ and add them to your __View__
* Display the __View__
* When the effect has finished, create and display a second __View__
* Free your previously allocated __BitPlanes__
* etc.
* Shutdown the framework

Starlight works as kind of state machine. Create a view, add Viewports, display View, create
next View, etc. It always stores the old View which is deleted AFTER you switch to the
new View. This allows a flicker free View switch mechanism.

## APIs
The following features are provided by my framework:

### Initialisation
The method **initStarlight(BOOL softInit)** takes over the graphical hardware. It 
stores the old View (usually the workbench screen), switches to a blank screen, loads neccessary libraries
and deactivates sprites. The Amiga operating system is still running. When your program has finished, a 
call to **exitStarlight()** restores the Workbench screen and deallocates
__View__, __ViewPort__, etc. data structures.

### Graphics
The graphics controller allows you to create a view, add ViewPorts to the View, display the View and destroy the View freeing
the previously allocated memory:
* **createNewView(void)**: Creates a low resolution screen.
* **addViewPort(struct BitMap *bitMap, struct BitMap *dBuffer, UWORD *colortable, WORD colortableSize, BOOL colormap32support, WORD x, WORD y, WORD width, WORD height, rX, rY)**: Adds
a ViewPort to the View. Parameters are the raster itself, its color table, position of the raster on screen and its size. Colormap32Support uses AGA for 24 bit color depth. Relative x and y coordinates can be used to set the position of a bitmap to the view port.
* **startView(void)**: Merges the copper list and displays the previously created View.
* **changeBuffer(UBYTE bufferIndex)**: Display first or second video buffer. 

### Bitmaps
Simple functions to create or delete Bitmaps and their corresponding bitplanes.
* **struct BitMap\* createBitMap(UBYTE depth, UWORD width, UWORD height)**: Allocate memory for BitMap structure and its bitplanes.
* **void cleanBitMap(struct BitMap\*)**: Free memory of a BitMap structure and its bitplanes. 
* **void cleanBitPlanes(PLANEPTR\* planes, UBYTE depth, UWORD width, UWORD height)**: Free bitplane array memory. Called by **cleanBitMap()**.

### Image Blobs
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
branch [here](https://s3.eu-central-1.amazonaws.com/codebuild-eu-central-1-4040-3447-3876-output-bucket/amiga/starlight-demo.tar.gz).

## Build
The Makefile works with [GCC](http://aminet.net/package/dev/gcc/m68k-amigaos-gcc). The code can be compiled with SAS-C as well.
