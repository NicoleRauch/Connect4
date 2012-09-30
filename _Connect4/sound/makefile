##################################
# Makefile for the mmsound sample
#
# using emx 0.8h, mm4emx 1.0
#
#
# Marc E.E. van Woerkom, 2/94
#

CC=gcc
CFLAGS=-Wall
LFLAGS=-los2me -liostream -s


all: mmsound.exe

mmsound.exe: mmsound.o mmsound.def
	$(CC) -o mmsound.exe mmsound.o mmsound.def $(LFLAGS)

mmsound.o: mmsound.c
	$(CC) $(CFLAGS) -c mmsound.c

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# clean up
clean:
	del *.o

