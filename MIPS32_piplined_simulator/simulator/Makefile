#
# Makefile for Architecture Project 2
#

CC=cc
# if you want to use debugger, add -g to CFLAGS and LDFLAGS
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=Pipelined_MIPS_simulator.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=pipeline
INCLUDES=Pipelined_MIPS_simulator.h

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
