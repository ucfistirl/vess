#!/bin/make
# makefile for io

LIBNAME = libvsIO.a

all: $(LIBNAME)
$(LIBNAME):
	@cd common; $(MAKE) \
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@cd $(WS); $(MAKE) \
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
	@cd $(OS); $(MAKE) \
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
	@cd X; $(MAKE) clean
	@cd Performer_X; $(MAKE) clean
	@cd OSG_X; $(MAKE) clean
	@cd Windows; $(MAKE) clean
	@cd Performer_Windows; $(MAKE) clean
	@cd OSG_Windows; $(MAKE) clean
	@cd IRIX; $(MAKE) clean
	@cd Linux; $(MAKE) clean

dep:
	@cd common; $(MAKE) dep\
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
	@cd $(WS); $(MAKE) dep\
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
	@cd $(OS); $(MAKE) dep\
		"OS=$(OS)" \
		"SG=$(SG)" \
		"WS=$(WS)" \
		"AL=$(AL)" \
		"CC=$(CC)" \
		"CC_FLAGS=$(CC_FLAGS)" \
		"EXTRA_INCDIRS=$(EXTRA_INCDIRS)"
