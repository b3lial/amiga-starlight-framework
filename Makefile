CC=m68k-amigaos-gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -noixemul 
SOURCES=starlight.c init.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=starlight

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean: 
	rm *.o
