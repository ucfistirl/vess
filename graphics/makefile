#!/bin/make
# makefile for graphics

LIBNAME = libvsGraphics.a

#all: $(LIBNAME)
#$(LIBNAME):
all:
	@cd common; $(MAKE) \
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@cd $(SG); $(MAKE) \
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@cd $(SG)_$(WS); $(MAKE) \
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	ar $(AR_FLAGS) $(LIBNAME) obj/*.o

clean:
	@rm -f *.a
	@cd obj; rm -f *.o
	@cd common; $(MAKE) clean
	@cd Performer; $(MAKE) clean
	@cd OSG; $(MAKE) clean
	@cd Performer_X; $(MAKE) clean
	@cd Performer_Windows; $(MAKE) clean
	@cd OSG_X; $(MAKE) clean
	@cd OSG_Windows; $(MAKE) clean

dep:
	@cd common; $(MAKE) dep\
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@cd $(SG); $(MAKE) dep\
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@cd $(SG)_$(WS); $(MAKE) dep\
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
