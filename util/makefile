#!/bin/make
# makefile for util

LIBNAME = libvsUtil.a

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
