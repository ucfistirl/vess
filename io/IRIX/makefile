#!/bin/make
# makefile for io/IRIX

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../system/$(SG) \
	  -I../../avatar/common \
	  -I../../motion/common \
	  -I../../io/common \
	  -I../../io/$(WS) \
	  -I../../io/$(SG)_$(WS) \
	  -I../../io/$(OS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsAscensionSerialTrackingSystem.o \
          vsEthernetMotionStar.o \
          vsFastrak.o \
          vsIS600.o \
          vsSerialPort.o

LIBSRCS = $(LIBOBJS:.o=.c++)


all: $(LIBOBJS)

dep: depend
depend:
	makedepend -- $(CC_FLAGS) $(INCDIRS) -- $(LIBSRCS)

clean:
	@rm -f *.o makefile.bak

.c++.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj

.c.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj
# DO NOT DELETE
