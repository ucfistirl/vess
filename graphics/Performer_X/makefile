#!/bin/make
# makefile for graphics/Performer_X

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../util/common \
          -I/usr/include/gcc-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsPane.o \
          vsPipe.o \
          vsScreen.o \
          vsWindow.o

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

vsPane.o: vsPane.h++ /usr/include/Performer/pf/pfChannel.h
vsPane.o: /usr/include/math.h /usr/include/features.h
vsPane.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsPane.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsPane.o: /usr/include/bits/mathcalls.h /usr/include/string.h
vsPane.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsPane.o: /usr/include/Performer/pf/pfUpdatable.h /usr/include/Performer/pf.h
vsPane.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsPane.o: /usr/include/bits/types.h /usr/include/time.h /usr/include/endian.h
vsPane.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsPane.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsPane.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsPane.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
vsPane.o: /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h
vsPane.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsPane.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsPane.o: /usr/include/getopt.h /usr/include/ulocks.h /usr/include/malloc.h
vsPane.o: /usr/include/limits.h
vsPane.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsPane.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsPane.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsPane.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsPane.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsPane.o: /usr/include/errno.h /usr/include/bits/errno.h
vsPane.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsPane.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsPane.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsPane.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h /usr/include/GL/glx.h
vsPane.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsPane.o: /usr/include/Performer/opengl.h /usr/include/Performer/linuxstubs.h
vsPane.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsPane.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsPane.o: /usr/include/alloca.h /usr/include/sys/prctl.h
vsPane.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsPane.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsPane.o: /usr/include/Performer/prstats.h
vsPane.o: /usr/include/Performer/pr/pfLinMath.h
vsPane.o: /usr/include/Performer/pr/pfObject.h
vsPane.o: /usr/include/Performer/pr/pfMemory.h
vsPane.o: /usr/include/Performer/pr/pfType.h
vsPane.o: /usr/include/Performer/pr/pfStruct.h
vsPane.o: /usr/include/Performer/pfstats.h
vsPane.o: /usr/include/Performer/pf/pfBuffer.h
vsPane.o: /usr/include/Performer/pf/pfUpdate.h
vsPane.o: /usr/include/Performer/pf/pfLists.h
vsPane.o: /usr/include/Performer/pf/pfPipe.h
vsPane.o: /usr/include/Performer/pr/pfVideoChannel.h
vsPane.o: /usr/include/Performer/pr/pfList.h
vsPane.o: /usr/include/Performer/pr/pfCalligraphic.h
vsPane.o: /usr/include/Performer/pr/pfStats.h
vsPane.o: /usr/include/Performer/pr/pfDispList.h
vsPane.o: /usr/include/Performer/pr/pfGeoState.h
vsPane.o: /usr/include/Performer/pr/pfState.h
vsPane.o: /usr/include/Performer/pr/pfByteBank.h
vsPane.o: /usr/include/Performer/pr/pfGSetBank.h
vsPane.o: /usr/include/Performer/pf/pfPipeWindow.h
vsPane.o: /usr/include/Performer/pr/pfWindow.h
vsPane.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsPane.o: /usr/include/Performer/pf/pfEarthSky.h
vsPane.o: /usr/include/Performer/pf/pfLODState.h
vsPane.o: /usr/include/Performer/pf/pfFrameStats.h
vsPane.o: /usr/include/Performer/pf/pfScene.h
vsPane.o: /usr/include/Performer/pf/pfGroup.h
vsPane.o: /usr/include/Performer/pf/pfNode.h
vsPane.o: /usr/include/Performer/pr/pfGeoMath.h
vsPane.o: /usr/include/Performer/pr/pfGeoSet.h
vsPane.o: /usr/include/Performer/pr/pfFlux.h
vsPane.o: /usr/include/Performer/pr/pfEngine.h
vsPane.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsPane.o: /usr/include/Performer/pr/pfPassList.h
vsPane.o: /usr/include/Performer/pr/pfFBState.h vsWindow.h++ vsScreen.h++
vsPane.o: vsPipe.h++ ../../util/common/vsGlobals.h++
vsPane.o: ../../util/common/vsGrowableArray.h++
vsPane.o: ../../graphics/Performer/vsView.h++ ../../util/common/vsVector.h++
vsPane.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsPane.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsPane.o: ../../graphics/Performer/vsComponent.h++
vsPane.o: ../../graphics/Performer/vsAttribute.h++
vsPane.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsPane.o: ../../util/common/vsObjectMap.h++
vsPane.o: ../../graphics/Performer/vsNode.h++
vsPane.o: ../../graphics/Performer/vsScene.h++
vsPane.o: ../../graphics/Performer/vsLightAttribute.h++
vsPane.o: /usr/include/Performer/pr/pfLight.h
vsPane.o: /usr/include/Performer/pf/pfLightSource.h
vsPane.o: ../../graphics/Performer/vsGeometry.h++
vsPane.o: /usr/include/Performer/pf/pfGeode.h
vsPipe.o: vsPipe.h++ /usr/include/Performer/pf/pfPipe.h
vsPipe.o: /usr/include/Performer/pf/pfUpdatable.h /usr/include/Performer/pf.h
vsPipe.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsPipe.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsPipe.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsPipe.o: /usr/include/time.h /usr/include/endian.h
vsPipe.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsPipe.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsPipe.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsPipe.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
vsPipe.o: /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h
vsPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsPipe.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsPipe.o: /usr/include/getopt.h /usr/include/ulocks.h /usr/include/malloc.h
vsPipe.o: /usr/include/limits.h
vsPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsPipe.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsPipe.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsPipe.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsPipe.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsPipe.o: /usr/include/errno.h /usr/include/bits/errno.h
vsPipe.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsPipe.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsPipe.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsPipe.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h /usr/include/GL/glx.h
vsPipe.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsPipe.o: /usr/include/Performer/opengl.h /usr/include/Performer/linuxstubs.h
vsPipe.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsPipe.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsPipe.o: /usr/include/alloca.h /usr/include/string.h
vsPipe.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsPipe.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsPipe.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsPipe.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsPipe.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsPipe.o: /usr/include/Performer/pr/pfLinMath.h
vsPipe.o: /usr/include/Performer/pr/pfObject.h
vsPipe.o: /usr/include/Performer/pr/pfMemory.h
vsPipe.o: /usr/include/Performer/pr/pfType.h
vsPipe.o: /usr/include/Performer/pr/pfStruct.h
vsPipe.o: /usr/include/Performer/pfstats.h
vsPipe.o: /usr/include/Performer/pf/pfBuffer.h
vsPipe.o: /usr/include/Performer/pf/pfUpdate.h
vsPipe.o: /usr/include/Performer/pf/pfLists.h
vsPipe.o: /usr/include/Performer/pr/pfVideoChannel.h
vsPipe.o: /usr/include/Performer/pr/pfList.h
vsPipe.o: /usr/include/Performer/pr/pfCalligraphic.h
vsPipe.o: /usr/include/Performer/pr/pfStats.h
vsPipe.o: /usr/include/Performer/pr/pfDispList.h
vsPipe.o: /usr/include/Performer/pr/pfGeoState.h
vsPipe.o: /usr/include/Performer/pr/pfState.h
vsPipe.o: /usr/include/Performer/pr/pfByteBank.h
vsPipe.o: /usr/include/Performer/pr/pfGSetBank.h
vsPipe.o: ../../util/common/vsGlobals.h++ vsScreen.h++ vsWindow.h++
vsPipe.o: /usr/include/Performer/pf/pfPipeWindow.h
vsPipe.o: /usr/include/Performer/pr/pfWindow.h vsPane.h++
vsPipe.o: /usr/include/Performer/pf/pfChannel.h
vsPipe.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsPipe.o: /usr/include/Performer/pf/pfEarthSky.h
vsPipe.o: /usr/include/Performer/pf/pfLODState.h
vsPipe.o: /usr/include/Performer/pf/pfFrameStats.h
vsPipe.o: /usr/include/Performer/pf/pfScene.h
vsPipe.o: /usr/include/Performer/pf/pfGroup.h
vsPipe.o: /usr/include/Performer/pf/pfNode.h
vsPipe.o: /usr/include/Performer/pr/pfGeoMath.h
vsPipe.o: /usr/include/Performer/pr/pfGeoSet.h
vsPipe.o: /usr/include/Performer/pr/pfFlux.h
vsPipe.o: /usr/include/Performer/pr/pfEngine.h
vsPipe.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsPipe.o: /usr/include/Performer/pr/pfPassList.h
vsPipe.o: /usr/include/Performer/pr/pfFBState.h
vsPipe.o: ../../graphics/Performer/vsView.h++ ../../util/common/vsVector.h++
vsPipe.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsPipe.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsPipe.o: ../../graphics/Performer/vsComponent.h++
vsPipe.o: ../../util/common/vsGrowableArray.h++
vsPipe.o: ../../graphics/Performer/vsAttribute.h++
vsPipe.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsPipe.o: ../../util/common/vsObjectMap.h++
vsPipe.o: ../../graphics/Performer/vsNode.h++
vsPipe.o: ../../graphics/Performer/vsScene.h++
vsPipe.o: ../../graphics/Performer/vsLightAttribute.h++
vsPipe.o: /usr/include/Performer/pr/pfLight.h
vsPipe.o: /usr/include/Performer/pf/pfLightSource.h
vsScreen.o: vsScreen.h++ vsPipe.h++ /usr/include/Performer/pf/pfPipe.h
vsScreen.o: /usr/include/Performer/pf/pfUpdatable.h
vsScreen.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsScreen.o: /usr/include/sys/types.h /usr/include/features.h
vsScreen.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsScreen.o: /usr/include/bits/types.h
vsScreen.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsScreen.o: /usr/include/time.h /usr/include/endian.h
vsScreen.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsScreen.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsScreen.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsScreen.o: /usr/include/stdio.h /usr/include/libio.h
vsScreen.o: /usr/include/_G_config.h /usr/include/wchar.h
vsScreen.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsScreen.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsScreen.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsScreen.o: /usr/include/getopt.h /usr/include/ulocks.h /usr/include/malloc.h
vsScreen.o: /usr/include/limits.h
vsScreen.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsScreen.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsScreen.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsScreen.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsScreen.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsScreen.o: /usr/include/errno.h /usr/include/bits/errno.h
vsScreen.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsScreen.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsScreen.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsScreen.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsScreen.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsScreen.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsScreen.o: /usr/include/Performer/linuxstubs.h
vsScreen.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsScreen.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsScreen.o: /usr/include/alloca.h /usr/include/string.h
vsScreen.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsScreen.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsScreen.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsScreen.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsScreen.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsScreen.o: /usr/include/Performer/pr/pfLinMath.h
vsScreen.o: /usr/include/Performer/pr/pfObject.h
vsScreen.o: /usr/include/Performer/pr/pfMemory.h
vsScreen.o: /usr/include/Performer/pr/pfType.h
vsScreen.o: /usr/include/Performer/pr/pfStruct.h
vsScreen.o: /usr/include/Performer/pfstats.h
vsScreen.o: /usr/include/Performer/pf/pfBuffer.h
vsScreen.o: /usr/include/Performer/pf/pfUpdate.h
vsScreen.o: /usr/include/Performer/pf/pfLists.h
vsScreen.o: /usr/include/Performer/pr/pfVideoChannel.h
vsScreen.o: /usr/include/Performer/pr/pfList.h
vsScreen.o: /usr/include/Performer/pr/pfCalligraphic.h
vsScreen.o: /usr/include/Performer/pr/pfStats.h
vsScreen.o: /usr/include/Performer/pr/pfDispList.h
vsScreen.o: /usr/include/Performer/pr/pfGeoState.h
vsScreen.o: /usr/include/Performer/pr/pfState.h
vsScreen.o: /usr/include/Performer/pr/pfByteBank.h
vsScreen.o: /usr/include/Performer/pr/pfGSetBank.h
vsScreen.o: ../../util/common/vsGlobals.h++ vsWindow.h++
vsScreen.o: /usr/include/Performer/pf/pfPipeWindow.h
vsScreen.o: /usr/include/Performer/pr/pfWindow.h vsPane.h++
vsScreen.o: /usr/include/Performer/pf/pfChannel.h
vsScreen.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsScreen.o: /usr/include/Performer/pf/pfEarthSky.h
vsScreen.o: /usr/include/Performer/pf/pfLODState.h
vsScreen.o: /usr/include/Performer/pf/pfFrameStats.h
vsScreen.o: /usr/include/Performer/pf/pfScene.h
vsScreen.o: /usr/include/Performer/pf/pfGroup.h
vsScreen.o: /usr/include/Performer/pf/pfNode.h
vsScreen.o: /usr/include/Performer/pr/pfGeoMath.h
vsScreen.o: /usr/include/Performer/pr/pfGeoSet.h
vsScreen.o: /usr/include/Performer/pr/pfFlux.h
vsScreen.o: /usr/include/Performer/pr/pfEngine.h
vsScreen.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsScreen.o: /usr/include/Performer/pr/pfPassList.h
vsScreen.o: /usr/include/Performer/pr/pfFBState.h
vsScreen.o: ../../graphics/Performer/vsView.h++
vsScreen.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsScreen.o: ../../util/common/vsQuat.h++
vsScreen.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsScreen.o: ../../graphics/Performer/vsComponent.h++
vsScreen.o: ../../util/common/vsGrowableArray.h++
vsScreen.o: ../../graphics/Performer/vsAttribute.h++
vsScreen.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsScreen.o: ../../util/common/vsObjectMap.h++
vsScreen.o: ../../graphics/Performer/vsNode.h++
vsScreen.o: ../../graphics/Performer/vsScene.h++
vsScreen.o: ../../graphics/Performer/vsLightAttribute.h++
vsScreen.o: /usr/include/Performer/pr/pfLight.h
vsScreen.o: /usr/include/Performer/pf/pfLightSource.h
vsWindow.o: vsWindow.h++ /usr/include/X11/Xlib.h /usr/include/sys/types.h
vsWindow.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsWindow.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsWindow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWindow.o: /usr/include/time.h /usr/include/endian.h
vsWindow.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsWindow.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsWindow.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsWindow.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
vsWindow.o: /usr/include/X11/Xosdefs.h
vsWindow.o: /usr/include/Performer/pf/pfPipeWindow.h
vsWindow.o: /usr/include/Performer/pf/pfUpdatable.h
vsWindow.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsWindow.o: /usr/include/stdio.h /usr/include/libio.h
vsWindow.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWindow.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWindow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWindow.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsWindow.o: /usr/include/getopt.h /usr/include/ulocks.h /usr/include/malloc.h
vsWindow.o: /usr/include/limits.h
vsWindow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsWindow.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsWindow.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsWindow.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsWindow.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsWindow.o: /usr/include/errno.h /usr/include/bits/errno.h
vsWindow.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsWindow.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsWindow.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsWindow.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsWindow.o: /usr/include/Performer/linuxstubs.h
vsWindow.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsWindow.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsWindow.o: /usr/include/alloca.h /usr/include/string.h
vsWindow.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsWindow.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsWindow.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsWindow.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsWindow.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsWindow.o: /usr/include/Performer/pr/pfLinMath.h
vsWindow.o: /usr/include/Performer/pr/pfObject.h
vsWindow.o: /usr/include/Performer/pr/pfMemory.h
vsWindow.o: /usr/include/Performer/pr/pfType.h
vsWindow.o: /usr/include/Performer/pr/pfStruct.h
vsWindow.o: /usr/include/Performer/pfstats.h
vsWindow.o: /usr/include/Performer/pf/pfBuffer.h
vsWindow.o: /usr/include/Performer/pf/pfUpdate.h
vsWindow.o: /usr/include/Performer/pf/pfLists.h
vsWindow.o: /usr/include/Performer/pf/pfPipe.h
vsWindow.o: /usr/include/Performer/pr/pfVideoChannel.h
vsWindow.o: /usr/include/Performer/pr/pfList.h
vsWindow.o: /usr/include/Performer/pr/pfCalligraphic.h
vsWindow.o: /usr/include/Performer/pr/pfStats.h
vsWindow.o: /usr/include/Performer/pr/pfDispList.h
vsWindow.o: /usr/include/Performer/pr/pfGeoState.h
vsWindow.o: /usr/include/Performer/pr/pfState.h
vsWindow.o: /usr/include/Performer/pr/pfByteBank.h
vsWindow.o: /usr/include/Performer/pr/pfGSetBank.h
vsWindow.o: /usr/include/Performer/pr/pfWindow.h vsScreen.h++ vsPipe.h++
vsWindow.o: ../../util/common/vsGlobals.h++
vsWindow.o: ../../util/common/vsGrowableArray.h++ vsPane.h++
vsWindow.o: /usr/include/Performer/pf/pfChannel.h
vsWindow.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsWindow.o: /usr/include/Performer/pf/pfEarthSky.h
vsWindow.o: /usr/include/Performer/pf/pfLODState.h
vsWindow.o: /usr/include/Performer/pf/pfFrameStats.h
vsWindow.o: /usr/include/Performer/pf/pfScene.h
vsWindow.o: /usr/include/Performer/pf/pfGroup.h
vsWindow.o: /usr/include/Performer/pf/pfNode.h
vsWindow.o: /usr/include/Performer/pr/pfGeoMath.h
vsWindow.o: /usr/include/Performer/pr/pfGeoSet.h
vsWindow.o: /usr/include/Performer/pr/pfFlux.h
vsWindow.o: /usr/include/Performer/pr/pfEngine.h
vsWindow.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsWindow.o: /usr/include/Performer/pr/pfPassList.h
vsWindow.o: /usr/include/Performer/pr/pfFBState.h
vsWindow.o: ../../graphics/Performer/vsView.h++
vsWindow.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsWindow.o: ../../util/common/vsQuat.h++
vsWindow.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsWindow.o: ../../graphics/Performer/vsComponent.h++
vsWindow.o: ../../graphics/Performer/vsAttribute.h++
vsWindow.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsWindow.o: ../../util/common/vsObjectMap.h++
vsWindow.o: ../../graphics/Performer/vsNode.h++
vsWindow.o: ../../graphics/Performer/vsScene.h++
vsWindow.o: ../../graphics/Performer/vsLightAttribute.h++
vsWindow.o: /usr/include/Performer/pr/pfLight.h
vsWindow.o: /usr/include/Performer/pf/pfLightSource.h /usr/include/sys/time.h
vsWindow.o: /usr/include/Performer/image.h
