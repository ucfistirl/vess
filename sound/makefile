#!/bin/make
# makefile for sound

LIBNAME = libvsSound.a

all: $(LIBNAME)
$(LIBNAME):
	@cd $(AL); $(MAKE) \
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
	@cd $(AL); $(MAKE) clean

dep:
	@cd $(AL); $(MAKE) dep \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
