#!/bin/make

VSLIBS = \
         corelib \
         math \
         scene \
         input \
         motion \
         sound \
         test

OS = `uname`

all clean dep:
	@for i in $(VSLIBS); \
	do \
		if test ! -d $$i; then \
			echo "Skipping $$i; No such directory"; \
		else \
			echo "Making $$i ($@) for ${OS}"; \
			cd $$i; ${MAKE} -f makefile.${OS} $@; cd ..; \
		fi \
	done

