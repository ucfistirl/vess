#!/bin/make
# makefile for system

LIBNAME = libvsSystem.a

all: $(LIBNAME)
$(LIBNAME):
	@cd $(SG); $(MAKE) \
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
	@cd Performer; $(MAKE) clean
	@cd OSG; $(MAKE) clean

dep:
	@cd $(SG); $(MAKE) dep \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
