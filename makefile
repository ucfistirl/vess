#!/bin/make
# booster makefile

# determine operating system
UNAME = `uname`
OS = $(UNAME:IRIX64=IRIX)

# call the main makefile
all:
	@echo "Making for $(UNAME)"
	@${MAKE} -f makefile.mk all "UNAME=$(UNAME)"

.DEFAULT:
	@echo "Making for $(UNAME)"
	@${MAKE} -f makefile.mk $@ "UNAME=$(UNAME)"
