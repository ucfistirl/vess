#!/bin/make
# makefile for util/common

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
	  vsGrowableArray.o \
	  vsMatrix.o \
	  vsObject.o \
	  vsObjectMap.o \
	  vsQuat.o \
	  vsTimer.o \
	  vsTreeMap.o \
	  vsUpdatable.o \
	  vsVector.o

LIBSRCS = $(LIBOBJS:.o=.c++)


all: $(LIBOBJS)

dep: depend
depend:
	makedepend -- $(CC_FLAGS) $(INCDIRS) -- $(LIBSRCS)

clean:
	rm -f *.o makefile.bak

.c++.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj

.c.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj
# DO NOT DELETE

vsGrowableArray.o: vsGrowableArray.h++ /usr/include/stdlib.h
vsGrowableArray.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsGrowableArray.o: /usr/include/gnu/stubs.h
vsGrowableArray.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsGrowableArray.o: /usr/include/bits/waitflags.h
vsGrowableArray.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsGrowableArray.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsGrowableArray.o: /usr/include/stdio.h /usr/include/bits/types.h
vsGrowableArray.o: /usr/include/libio.h /usr/include/_G_config.h
vsGrowableArray.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsGrowableArray.o: /usr/include/gconv.h
vsGrowableArray.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsGrowableArray.o: /usr/include/bits/stdio_lim.h
vsGrowableArray.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsGrowableArray.o: /usr/include/string.h vsGlobals.h++ /usr/include/math.h
vsGrowableArray.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsGrowableArray.o: /usr/include/bits/mathcalls.h
vsMatrix.o: /usr/include/math.h /usr/include/features.h
vsMatrix.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsMatrix.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsMatrix.o: /usr/include/bits/mathcalls.h /usr/include/stdio.h
vsMatrix.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsMatrix.o: /usr/include/bits/types.h /usr/include/libio.h
vsMatrix.o: /usr/include/_G_config.h /usr/include/wchar.h
vsMatrix.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsMatrix.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsMatrix.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsMatrix.o: /usr/include/getopt.h vsGlobals.h++ vsMatrix.h++ vsVector.h++
vsMatrix.o: vsQuat.h++
vsObject.o: /usr/include/stdio.h /usr/include/features.h
vsObject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsObject.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsObject.o: /usr/include/bits/types.h /usr/include/libio.h
vsObject.o: /usr/include/_G_config.h /usr/include/wchar.h
vsObject.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsObject.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsObject.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsObject.o: /usr/include/getopt.h vsGlobals.h++ /usr/include/math.h
vsObject.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsObject.o: /usr/include/bits/mathcalls.h vsObject.h++ vsTreeMap.h++
vsObject.o: vsGrowableArray.h++
vsObjectMap.o: vsObjectMap.h++ vsTreeMap.h++ vsGrowableArray.h++
vsObjectMap.o: /usr/include/stdio.h /usr/include/features.h
vsObjectMap.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsObjectMap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsObjectMap.o: /usr/include/bits/types.h /usr/include/libio.h
vsObjectMap.o: /usr/include/_G_config.h /usr/include/wchar.h
vsObjectMap.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsObjectMap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsObjectMap.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsObjectMap.o: /usr/include/getopt.h vsGlobals.h++ /usr/include/math.h
vsObjectMap.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsObjectMap.o: /usr/include/bits/mathcalls.h
vsQuat.o: vsQuat.h++ vsMatrix.h++ /usr/include/stdio.h
vsQuat.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsQuat.o: /usr/include/gnu/stubs.h
vsQuat.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsQuat.o: /usr/include/bits/types.h /usr/include/libio.h
vsQuat.o: /usr/include/_G_config.h /usr/include/wchar.h
vsQuat.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsQuat.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsQuat.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsQuat.o: /usr/include/getopt.h vsGlobals.h++ /usr/include/math.h
vsQuat.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsQuat.o: /usr/include/bits/mathcalls.h vsVector.h++
vsTimer.o: vsTimer.h++ vsGlobals.h++ /usr/include/math.h
vsTimer.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsTimer.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsTimer.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsTimer.o: /usr/include/stdio.h
vsTimer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTimer.o: /usr/include/bits/types.h /usr/include/libio.h
vsTimer.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTimer.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTimer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTimer.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsTimer.o: /usr/include/getopt.h /usr/include/sys/time.h /usr/include/time.h
vsTimer.o: /usr/include/bits/time.h /usr/include/sys/select.h
vsTimer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsTreeMap.o: /usr/include/stdlib.h /usr/include/features.h
vsTreeMap.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTreeMap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTreeMap.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsTreeMap.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTreeMap.o: /usr/include/alloca.h /usr/include/stdio.h
vsTreeMap.o: /usr/include/bits/types.h /usr/include/libio.h
vsTreeMap.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTreeMap.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTreeMap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTreeMap.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsTreeMap.o: /usr/include/getopt.h vsGlobals.h++ /usr/include/math.h
vsTreeMap.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTreeMap.o: /usr/include/bits/mathcalls.h vsTreeMap.h++ vsGrowableArray.h++
vsUpdatable.o: vsUpdatable.h++ vsObject.h++ /usr/include/stdio.h
vsUpdatable.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsUpdatable.o: /usr/include/gnu/stubs.h
vsUpdatable.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsUpdatable.o: /usr/include/bits/types.h /usr/include/libio.h
vsUpdatable.o: /usr/include/_G_config.h /usr/include/wchar.h
vsUpdatable.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsUpdatable.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsUpdatable.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsUpdatable.o: /usr/include/getopt.h vsTreeMap.h++ vsGrowableArray.h++
vsVector.o: /usr/include/stdio.h /usr/include/features.h
vsVector.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsVector.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsVector.o: /usr/include/bits/types.h /usr/include/libio.h
vsVector.o: /usr/include/_G_config.h /usr/include/wchar.h
vsVector.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsVector.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsVector.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsVector.o: /usr/include/getopt.h /usr/include/math.h
vsVector.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsVector.o: /usr/include/bits/mathcalls.h vsVector.h++ vsGlobals.h++
