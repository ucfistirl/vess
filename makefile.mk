#!/bin/make
# master makefile

# compilation selection macros

#   operating system automatically determined
#UNAME = `uname`
OS = $(UNAME:IRIX64=IRIX)
#   should be 'Performer' or 'OSG'
SG = OSG
#   should be 'OpenAL'
AL = OpenAL

# VESS directories

VSDIRS = \
    avatar \
    graphics \
    io \
    motion \
    sound \
    system \
    util

VSLIBS = \
    avatar/libvsAvatar.a \
    graphics/libvsGraphics.a \
    io/libvsIO.a \
    motion/libvsMotion.a \
    sound/libvsSound.a \
    system/libvsSystem.a \
    util/libvsUtil.a

VSTEST = test

VSALL = \
    $(VSDIRS) \
    $(VSTEST)

LIBRARY = libvess.a

# IRIX stuff
IRIX_Performer_CC_FLAGS = -n32 -mips3 -g -xansi -DSGI \
	-DPF_CPLUSPLUS_API=1 -woff 1681 -DIRIX
IRIX_CC = CC
IRIX_INCDIRS = -I/ist/projects/vr1/tools/openal/include \
               -I/ist/projects/vr1/tools/openal/linux/include

# Linux stuff
Linux_Performer_CC_FLAGS = -g -O -ansi -D__linux__ -D_XOPEN_SOURCE \
	-D_BSD_SOURCE -D_MISC_SOURCE -DPF_CPLUSPLUS_API=1 -Wno-deprecated
Linux_OSG_CC_FLAGS = -g -O -ansi -D__linux__ -D_XOPEN_SOURCE \
	-D_BSD_SOURCE -D_MISC_SOURCE -DVESS_DEBUG
Linux_CC = g++
Linux_INCDIRS = -I/ist/projects/nve/tools/openal/include \
                -I/ist/projects/nve/tools/openal/linux/include \
                -I/ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include \
                -I/usr/include/freetype2 \
                -I/ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText


# other stuff
AR_FLAGS = -rlc

# make rules

all:
	@$(MAKE) compile
	@$(MAKE) $(LIBRARY)
	@$(MAKE) testprogs

clean:
	@if test -f $(LIBRARY); then \
		rm -f $(LIBRARY); \
	fi
	@for i in $(VSDIRS); \
	do \
		if test ! -d $$i; then \
			echo "Skipping $$i; No such directory"; \
		else \
			echo "Making $$i (clean)"; \
			cd $$i; ${MAKE} clean \
				"OS=$(OS)" \
				"SG=$(SG)" \
				"WS=X" \
				"AL=$(AL)"; cd ..; \
		fi; \
	done
	@if test ! -d test; then \
		echo "Skipping test; No such directory"; \
	else \
		echo "Making test (clean)"; \
		cd test; ${MAKE} -f makefile.$(OS) clean; cd ..; \
	fi

dep:
	@for i in $(VSDIRS); \
	do \
		if test ! -d $$i; then \
			echo "Skipping $$i; No such directory"; \
		else \
			echo "Making $$i (dep) for $(OS)"; \
			cd $$i; \
			${MAKE} dep \
				"OS=$(OS)" \
				"SG=$(SG)" \
				"WS=X" \
				"AL=$(AL)" \
				"CC=$($(OS)_CC)" \
				"CC_FLAGS=$($(OS)_$(SG)_CC_FLAGS)" \
				"AR_FLAGS=$(AR_FLAGS)" \
				"EXTRA_INCDIRS=$($(OS)_INCDIRS)"; \
			cd ..; \
		fi; \
	done
	@if test ! -d test; then \
		echo "Skipping test; No such directory"; \
	else \
		echo "Making test (dep) for $(OS)"; \
		cd test; \
		${MAKE} -f makefile.$(OS) dep \
			"OS=$(OS)" \
			"SG=$(SG)" \
			"WS=X" \
			"AL=$(AL)" \
			"CC=$($(OS)_CC)" \
			"CC_FLAGS=$($(OS)_$(SG)_CC_FLAGS)" \
			"AR_FLAGS=$(AR_FLAGS)" \
			"EXTRA_INCDIRS=$($(OS)_INCDIRS)"; \
		cd ..; \
	fi

compile:
	@for i in $(VSDIRS); \
	do \
		if test ! -d $$i; then \
			echo "Skipping $$i; No such directory"; \
		else \
			echo "Making $$i (compile) for $(OS)"; \
			cd $$i; \
			${MAKE} \
				"OS=$(OS)" \
				"SG=$(SG)" \
				"WS=X" \
				"AL=$(AL)" \
				"CC=$($(OS)_CC)" \
				"CC_FLAGS=$($(OS)_$(SG)_CC_FLAGS)" \
				"AR_FLAGS=$(AR_FLAGS)" \
				"EXTRA_INCDIRS=$($(OS)_INCDIRS)"; \
			cd ..; \
		fi; \
	done

$(LIBRARY): $(VSLIBS)
	@for i in $(VSDIRS); \
	do \
		OBJS="$$OBJS "`ls $$i/obj/*.o`; \
	done; \
	echo "Building $(LIBRARY)"; \
	ar -ruc libvess.a $$OBJS

testprogs:
	@for i in $(VSTEST); \
	do \
		if test ! -d $$i; then \
			echo "Skipping $$i; No such directory"; \
		else \
			echo "Making $$i (testprogs) for $(OS)"; \
			cd $$i; \
			${MAKE} -f makefile.$(OS)\
				"OS=$(OS)" \
				"SG=$(SG)" \
				"WS=X" \
				"AL=$(AL)" \
				"CC=$($(OS)_CC)" \
				"CC_FLAGS=$($(OS)_$(SG)_CC_FLAGS)" \
				"AR_FLAGS=$(AR_FLAGS)" \
				"EXTRA_INCDIRS=$($(OS)_INCDIRS)"; \
			cd ..; \
		fi; \
	done
