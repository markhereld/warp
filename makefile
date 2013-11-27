#

##include /usr/include/make/commondefs

CC = gcc

LIBS    = -framework OpenGL -framework GLUT -lm
CFILES  = test.c warp.c texture.c
TARGETS = warp
OBJECTS  = test.o warp.o texture.o

default		: $(TARGETS)

##include $(COMMONRULES)

warp: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

# dependencies
texture.o	: texture.h
warp.o	: warp.h
$(OBJECTS)	: defs.h

clean: 
	/bin/rm -f core *.o warp


