#
# Makefile for Architecture Project 3
#

CC=cc
# if you want to use debugger, add -g to CFLAGS and LDFLAGS
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=CMP_MIPS_simulator.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=CMP
INCLUDES=CMP_MIPS_simulator.h

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
