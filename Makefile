CC=m68k-amigaos-gcc
CFLAGS = -D__far="" -Wall -Wextra -Wno-pointer-sign -O2 
LDFLAGS = -noixemul 
SOURCES=starlight.c init.c utils.c payload_intro.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=starlight

all: $(SOURCES) $(EXECUTABLE) deploy

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
deploy: starlight
	cp starlight /home/belial/Roms/Amiga/Uploads
clean: 
	rm *.o *.lnk *.info *.uaem starlight
