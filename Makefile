CC=m68k-amigaos-gcc
CFLAGS = -D__far="" -Wall -O2 -I. 
LDFLAGS = -noixemul 
SOURCES=main.c init.c utils.c views/twoplanes.c graphics_controller.c \
		blob_controller.c views/ballblob.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=starlight-gcc-build

all: $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean: 
	rm *.o views/*.o views/*.uaem *.lnk *.info *.uaem starlight \
		starlight-demo starlight-gcc-build starlight-sasc-build
