# Makefile for connect4.exe
# To compile all necessary files simply type make at the command line prompt.
# This makefile runs with GNU make (version 3.??) only

# If you want to have sound enabled, set HAVEMMLIBS to 1
# Note: Trailing space characters are not allowed here! 
HAVEMMLIBS = 1
# To optimize set OPTIMIZE to 1
OPTIMIZE = 1
# To use method (E2) for compiling as described in emxdev.doc/emxdev.inf
# set USEOMFLIBS to 1
USEOMFLIBS = 1
ifeq ($(USEOMFLIBS),0)
   O = .o						# extension of object files
else
   O = .obj
endif

ifeq ($(OPTIMIZE),0)			# use the following options for debugging 
   CFLAGS = -g -O0 -Wall -Zmtd 
else								# or for optimization:
   CFLAGS = -O2 -fomit-frame-pointer -Wall -Zmtd
endif

# Flags for linking
ifeq ($(USEOMFLIBS),0)
   LDFLAGS = -Zmtd
else
   LDFLAGS = -Zmtd -Zomf
   CFLAGS := $(CFLAGS) -Zomf
endif
ifneq ($(OPTIMIZE),0)		
# The options -s doesn't optimize but strips the symbol table which
# is added to each file. So the code produced with -s isn't faster but the 
# executable is smaller. One the other hand, stripping symbols makes 
# debugging impossible.
   LDFLAGS:= $(LDFLAGS) -s
endif

OBJ = game$O rand$O misc$O errmsgs$O pmgame$O profile$O connect4$O mmsound$O
RES = connect4.res
CC = gcc
PP = cpp -P
RM = rm
MV = move
ifeq ($(HAVEMMLIBS),0)
   LIBS=-los2 
else
   CFLAGS:= $(CFLAGS) -DUSEMMSOUND
   LIBS=-los2me -los2
endif

# This is an implicit rule. It causes that any source code that has
# to be compiled from *.c to *.o is compiled like the following example:
# foo.o: foo.c foo.h
# 		$(CC) $(CFLAGS) -c foo.c
%$O:%.c	
	$(CC) $(CFLAGS) -c $<

# The first (default) rule in this makefile. It is taken if make is 
# evoken without any parameter 
all: connect4.exe connect4.hlp

connect4.exe: $(RES) $(OBJ) connect4.def
	$(CC) $(LDFLAGS) -o $@ $(OBJ) connect4.def $(LIBS)
	rc $(RES) $@


# The online help compiler (IPFC) doesn't do any preprocessing. Therefore
# we simply use the c-preprocessor of our compiler. If you want to use
# another preprocessor you have to find out how to invoke him so that
# he doesn't create linenumber informations. This command should then 
# replace the macro definition of $(PP) at the beginning of this makefile.
connect4.hlp: connect4.i0 defs.h
	$(PP) $< >connect4.ipf && ipfc /S connect4.ipf


# This target is defined to produce a text only version of the program for 
# testing the game algorithms.
# To make testgame.exe you have to type "make testgame.exe" 
# It's simple, isn't it?
testgame.exe: testgame.c game$O rand$O misc$O
	$(CC) -DDUMP_DISPLAY -DPRINT_STATS $(CFLAGS) -o testgame.exe $^

rand$O: rand.c rand.h

misc$O: misc.c misc.h

profile$O: profile.c profile.h connect4.h

errmsgs$O: errmsgs.c errmsgs.h 

# Do a bit more optimization for the minimax algorithm.
game$O: game.c game.h
	$(CC) $(CFLAGS) -funroll-loops -c $<

pmgame$O: pmgame.c pmgame.h game.h defs.h

mmsound$O: mmsound.c mmsound.h defs.h

connect4$O: connect4.c connect4.h game.h profile.h defs.h

connect4.res: connect4.rc help.rc dialog.dlg rhand.ptr yhand.ptr defs.h
	rc -r connect4.rc 



# To use the next two targets you need the unix tool rm (removes files)
clean:
	$(RM) -f $(OBJ) $(RES) connect4

veryclean: clean
	$(RM) -f connect4.exe testgame.exe
