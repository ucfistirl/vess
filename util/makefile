#!/bin/make
# makefile for util

LIBNAME = libvsUtil.a

SG=OSG
WS=X
AL=OpenAL
CC=g++
CC_FLAGS=-g -O -ansi -D__linux__ -D_XOPEN_SOURCE -D_BSD_SOURCE -D_MISC_SOURCE -DPF_CPLUSPLUS_API=1
AR_FLAGS=rulc

all: $(LIBNAME)
$(LIBNAME):
	@cd common; $(MAKE) \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@ar $(AR_FLAGS) $(LIBNAME) obj/*.o

clean:
	@rm -f *.a
	@cd obj; rm -f *.o
	@cd common; $(MAKE) clean

dep:
	@cd common; $(MAKE) dep \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
