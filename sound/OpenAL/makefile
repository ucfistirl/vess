#!/bin/make
# makefile for sound/OpenAL

.SUFFIXES: .c .c++ .o .h .h++

SG=OSG
WS=X
AL=OpenAL
CC=g++
CC_FLAGS=-D_XOPEN_SOURCE -D_BSD_SOURCE -D_MISC_SOURCE -DPF_CPLUSPLUS_API=1 -g -O -ansi -D__linux__ $(INCDIRS)

INCDIRS = -I. \
	  -I../../graphics/common \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../graphics/$(SG) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
	  vsSoundBuffer.o \
	  vsSoundListenerAttribute.o \
	  vsSoundPipe.o \
	  vsSoundSample.o \
	  vsSoundSourceAttribute.o \
	  vsSoundStream.o

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

vsSoundBuffer.o: vsSoundBuffer.h++
vsSoundListenerAttribute.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundListenerAttribute.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundListenerAttribute.o: /usr/include/sys/time.h /usr/include/features.h
vsSoundListenerAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundListenerAttribute.o: /usr/include/bits/types.h
vsSoundListenerAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundListenerAttribute.o: /usr/include/time.h /usr/include/bits/time.h
vsSoundListenerAttribute.o: /usr/include/sys/select.h
vsSoundListenerAttribute.o: /usr/include/bits/select.h
vsSoundListenerAttribute.o: /usr/include/bits/sigset.h /usr/include/stdio.h
vsSoundListenerAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsSoundListenerAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsSoundListenerAttribute.o: /usr/include/gconv.h
vsSoundListenerAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundListenerAttribute.o: /usr/include/bits/stdio_lim.h
vsSoundListenerAttribute.o: /usr/include/bits/sys_errlist.h
vsSoundListenerAttribute.o: /usr/include/getopt.h
vsSoundListenerAttribute.o: vsSoundListenerAttribute.h++
vsSoundListenerAttribute.o: ../../graphics/Performer/vsComponent.h++
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfNode.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr.h
vsSoundListenerAttribute.o: /usr/include/sys/types.h /usr/include/endian.h
vsSoundListenerAttribute.o: /usr/include/bits/endian.h
vsSoundListenerAttribute.o: /usr/include/sys/sysmacros.h
vsSoundListenerAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsSoundListenerAttribute.o: /usr/include/limits.h
vsSoundListenerAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSoundListenerAttribute.o: /usr/include/bits/posix1_lim.h
vsSoundListenerAttribute.o: /usr/include/bits/local_lim.h
vsSoundListenerAttribute.o: /usr/include/linux/limits.h
vsSoundListenerAttribute.o: /usr/include/bits/posix2_lim.h
vsSoundListenerAttribute.o: /usr/include/bits/xopen_lim.h
vsSoundListenerAttribute.o: /usr/include/unistd.h
vsSoundListenerAttribute.o: /usr/include/bits/posix_opt.h
vsSoundListenerAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsSoundListenerAttribute.o: /usr/include/bits/errno.h
vsSoundListenerAttribute.o: /usr/include/linux/errno.h
vsSoundListenerAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsSoundListenerAttribute.o: /usr/include/X11/Xlib.h
vsSoundListenerAttribute.o: /usr/include/X11/Xfuncproto.h
vsSoundListenerAttribute.o: /usr/include/X11/Xosdefs.h
vsSoundListenerAttribute.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsSoundListenerAttribute.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsSoundListenerAttribute.o: /usr/include/GL/glu.h
vsSoundListenerAttribute.o: /usr/include/Performer/opengl.h
vsSoundListenerAttribute.o: /usr/include/Performer/linuxstubs.h
vsSoundListenerAttribute.o: /usr/include/Performer/image.h
vsSoundListenerAttribute.o: /usr/include/stdlib.h
vsSoundListenerAttribute.o: /usr/include/bits/waitflags.h
vsSoundListenerAttribute.o: /usr/include/bits/waitstatus.h
vsSoundListenerAttribute.o: /usr/include/alloca.h /usr/include/string.h
vsSoundListenerAttribute.o: /usr/include/sys/prctl.h
vsSoundListenerAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsSoundListenerAttribute.o: /usr/include/bits/wordsize.h
vsSoundListenerAttribute.o: /usr/include/Performer/prmath.h
vsSoundListenerAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSoundListenerAttribute.o: /usr/include/bits/mathdef.h
vsSoundListenerAttribute.o: /usr/include/bits/mathcalls.h
vsSoundListenerAttribute.o: /usr/include/Performer/prstats.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfObject.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfType.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsSoundListenerAttribute.o: /usr/include/Performer/pfstats.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfLists.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsSoundListenerAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfList.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfStats.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfState.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsSoundListenerAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsSoundListenerAttribute.o: ../../util/common/vsGrowableArray.h++
vsSoundListenerAttribute.o: ../../util/common/vsVector.h++
vsSoundListenerAttribute.o: ../../util/common/vsGlobals.h++
vsSoundListenerAttribute.o: ../../graphics/Performer/vsAttribute.h++
vsSoundListenerAttribute.o: ../../util/common/vsObject.h++
vsSoundListenerAttribute.o: ../../util/common/vsTreeMap.h++
vsSoundListenerAttribute.o: ../../util/common/vsObjectMap.h++
vsSoundListenerAttribute.o: ../../graphics/Performer/vsNode.h++
vsSoundListenerAttribute.o: ../../util/common/vsMatrix.h++
vsSoundListenerAttribute.o: ../../util/common/vsQuat.h++ vsSoundBuffer.h++
vsSoundPipe.o: vsSoundPipe.h++ /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundPipe.o: /usr/include/stdio.h /usr/include/features.h
vsSoundPipe.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundPipe.o: /usr/include/bits/types.h /usr/include/libio.h
vsSoundPipe.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSoundPipe.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSoundPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundPipe.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsSoundPipe.o: /usr/include/getopt.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/alut.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/aluttypes.h
vsSoundSample.o: /usr/include/stdio.h /usr/include/features.h
vsSoundSample.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundSample.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundSample.o: /usr/include/bits/types.h /usr/include/libio.h
vsSoundSample.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSoundSample.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSoundSample.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundSample.o: /usr/include/bits/stdio_lim.h
vsSoundSample.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSoundSample.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSoundSample.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSoundSample.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSoundSample.o: vsSoundSample.h++ ../../util/common/vsGlobals.h++
vsSoundSample.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSoundSample.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsSoundSample.o: vsSoundBuffer.h++
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSourceAttribute.o: /usr/include/sys/time.h /usr/include/features.h
vsSoundSourceAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundSourceAttribute.o: /usr/include/bits/types.h
vsSoundSourceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundSourceAttribute.o: /usr/include/time.h /usr/include/bits/time.h
vsSoundSourceAttribute.o: /usr/include/sys/select.h
vsSoundSourceAttribute.o: /usr/include/bits/select.h
vsSoundSourceAttribute.o: /usr/include/bits/sigset.h /usr/include/stdio.h
vsSoundSourceAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsSoundSourceAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsSoundSourceAttribute.o: /usr/include/gconv.h
vsSoundSourceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundSourceAttribute.o: /usr/include/bits/stdio_lim.h
vsSoundSourceAttribute.o: /usr/include/bits/sys_errlist.h
vsSoundSourceAttribute.o: /usr/include/getopt.h vsSoundSourceAttribute.h++
vsSoundSourceAttribute.o: ../../graphics/Performer/vsComponent.h++
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfNode.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr.h
vsSoundSourceAttribute.o: /usr/include/sys/types.h /usr/include/endian.h
vsSoundSourceAttribute.o: /usr/include/bits/endian.h
vsSoundSourceAttribute.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsSoundSourceAttribute.o: /usr/include/malloc.h /usr/include/limits.h
vsSoundSourceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSoundSourceAttribute.o: /usr/include/bits/posix1_lim.h
vsSoundSourceAttribute.o: /usr/include/bits/local_lim.h
vsSoundSourceAttribute.o: /usr/include/linux/limits.h
vsSoundSourceAttribute.o: /usr/include/bits/posix2_lim.h
vsSoundSourceAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsSoundSourceAttribute.o: /usr/include/bits/posix_opt.h
vsSoundSourceAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsSoundSourceAttribute.o: /usr/include/bits/errno.h
vsSoundSourceAttribute.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsSoundSourceAttribute.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsSoundSourceAttribute.o: /usr/include/X11/Xfuncproto.h
vsSoundSourceAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsSoundSourceAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsSoundSourceAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsSoundSourceAttribute.o: /usr/include/Performer/opengl.h
vsSoundSourceAttribute.o: /usr/include/Performer/linuxstubs.h
vsSoundSourceAttribute.o: /usr/include/Performer/image.h
vsSoundSourceAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSoundSourceAttribute.o: /usr/include/bits/waitstatus.h
vsSoundSourceAttribute.o: /usr/include/alloca.h /usr/include/string.h
vsSoundSourceAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsSoundSourceAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsSoundSourceAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsSoundSourceAttribute.o: /usr/include/bits/huge_val.h
vsSoundSourceAttribute.o: /usr/include/bits/mathdef.h
vsSoundSourceAttribute.o: /usr/include/bits/mathcalls.h
vsSoundSourceAttribute.o: /usr/include/Performer/prstats.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfObject.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfType.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsSoundSourceAttribute.o: /usr/include/Performer/pfstats.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfLists.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsSoundSourceAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfList.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfStats.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfState.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsSoundSourceAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsSoundSourceAttribute.o: ../../util/common/vsGrowableArray.h++
vsSoundSourceAttribute.o: ../../util/common/vsVector.h++
vsSoundSourceAttribute.o: ../../util/common/vsGlobals.h++
vsSoundSourceAttribute.o: ../../graphics/Performer/vsAttribute.h++
vsSoundSourceAttribute.o: ../../util/common/vsObject.h++
vsSoundSourceAttribute.o: ../../util/common/vsTreeMap.h++
vsSoundSourceAttribute.o: ../../util/common/vsObjectMap.h++
vsSoundSourceAttribute.o: ../../graphics/Performer/vsNode.h++
vsSoundSourceAttribute.o: ../../util/common/vsMatrix.h++
vsSoundSourceAttribute.o: ../../util/common/vsQuat.h++ vsSoundSample.h++
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundSourceAttribute.o: vsSoundBuffer.h++ vsSoundStream.h++
vsSoundStream.o: /usr/include/stdio.h /usr/include/features.h
vsSoundStream.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundStream.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundStream.o: /usr/include/bits/types.h /usr/include/libio.h
vsSoundStream.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSoundStream.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSoundStream.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundStream.o: /usr/include/bits/stdio_lim.h
vsSoundStream.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSoundStream.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSoundStream.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSoundStream.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSoundStream.o: /usr/include/string.h ../../util/common/vsGlobals.h++
vsSoundStream.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSoundStream.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsSoundStream.o: vsSoundStream.h++
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundStream.o: vsSoundBuffer.h++
