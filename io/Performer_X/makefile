#!/bin/make
# makefile for io/X_Performer

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../io/common \
	  -I../../io/$(WS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsWindowSystem.o

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

vsWindowSystem.o: vsWindowSystem.h++ /usr/include/X11/Xlib.h
vsWindowSystem.o: /usr/include/sys/types.h /usr/include/features.h
vsWindowSystem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWindowSystem.o: /usr/include/bits/types.h
vsWindowSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWindowSystem.o: /usr/include/time.h /usr/include/endian.h
vsWindowSystem.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsWindowSystem.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsWindowSystem.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsWindowSystem.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
vsWindowSystem.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsWindowSystem.o: /usr/include/X11/Xos.h /usr/include/string.h
vsWindowSystem.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsWindowSystem.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
vsWindowSystem.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
vsWindowSystem.o: /usr/include/bits/confname.h /usr/include/getopt.h
vsWindowSystem.o: /usr/include/sys/time.h /usr/include/X11/Xarch.h
vsWindowSystem.o: ../../util/common/vsObjectMap.h++
vsWindowSystem.o: ../../util/common/vsTreeMap.h++
vsWindowSystem.o: ../../util/common/vsGrowableArray.h++
vsWindowSystem.o: ../../io/common/vsInputSystem.h++
vsWindowSystem.o: ../../util/common/vsUpdatable.h++
vsWindowSystem.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsWindowSystem.o: /usr/include/libio.h /usr/include/_G_config.h
vsWindowSystem.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsWindowSystem.o: /usr/include/gconv.h
vsWindowSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWindowSystem.o: /usr/include/bits/stdio_lim.h
vsWindowSystem.o: /usr/include/bits/sys_errlist.h
vsWindowSystem.o: ../../graphics/Performer_X/vsWindow.h++
vsWindowSystem.o: /usr/include/Performer/pf/pfPipeWindow.h
vsWindowSystem.o: /usr/include/Performer/pf/pfUpdatable.h
vsWindowSystem.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsWindowSystem.o: /usr/include/ulocks.h /usr/include/malloc.h
vsWindowSystem.o: /usr/include/limits.h
vsWindowSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsWindowSystem.o: /usr/include/bits/posix1_lim.h
vsWindowSystem.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsWindowSystem.o: /usr/include/bits/posix2_lim.h
vsWindowSystem.o: /usr/include/bits/xopen_lim.h /usr/include/errno.h
vsWindowSystem.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsWindowSystem.o: /usr/include/asm/errno.h /usr/include/GL/gl.h
vsWindowSystem.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsWindowSystem.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsWindowSystem.o: /usr/include/Performer/linuxstubs.h
vsWindowSystem.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsWindowSystem.o: /usr/include/bits/waitflags.h
vsWindowSystem.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsWindowSystem.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsWindowSystem.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsWindowSystem.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsWindowSystem.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsWindowSystem.o: /usr/include/bits/mathcalls.h
vsWindowSystem.o: /usr/include/Performer/prstats.h
vsWindowSystem.o: /usr/include/Performer/pr/pfLinMath.h
vsWindowSystem.o: /usr/include/Performer/pr/pfObject.h
vsWindowSystem.o: /usr/include/Performer/pr/pfMemory.h
vsWindowSystem.o: /usr/include/Performer/pr/pfType.h
vsWindowSystem.o: /usr/include/Performer/pr/pfStruct.h
vsWindowSystem.o: /usr/include/Performer/pfstats.h
vsWindowSystem.o: /usr/include/Performer/pf/pfBuffer.h
vsWindowSystem.o: /usr/include/Performer/pf/pfUpdate.h
vsWindowSystem.o: /usr/include/Performer/pf/pfLists.h
vsWindowSystem.o: /usr/include/Performer/pf/pfPipe.h
vsWindowSystem.o: /usr/include/Performer/pr/pfVideoChannel.h
vsWindowSystem.o: /usr/include/Performer/pr/pfList.h
vsWindowSystem.o: /usr/include/Performer/pr/pfCalligraphic.h
vsWindowSystem.o: /usr/include/Performer/pr/pfStats.h
vsWindowSystem.o: /usr/include/Performer/pr/pfDispList.h
vsWindowSystem.o: /usr/include/Performer/pr/pfGeoState.h
vsWindowSystem.o: /usr/include/Performer/pr/pfState.h
vsWindowSystem.o: /usr/include/Performer/pr/pfByteBank.h
vsWindowSystem.o: /usr/include/Performer/pr/pfGSetBank.h
vsWindowSystem.o: /usr/include/Performer/pr/pfWindow.h
vsWindowSystem.o: ../../graphics/Performer_X/vsScreen.h++
vsWindowSystem.o: ../../graphics/Performer_X/vsPipe.h++
vsWindowSystem.o: ../../util/common/vsGlobals.h++
vsWindowSystem.o: ../../graphics/Performer_X/vsPane.h++
vsWindowSystem.o: /usr/include/Performer/pf/pfChannel.h
vsWindowSystem.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsWindowSystem.o: /usr/include/Performer/pf/pfEarthSky.h
vsWindowSystem.o: /usr/include/Performer/pf/pfLODState.h
vsWindowSystem.o: /usr/include/Performer/pf/pfFrameStats.h
vsWindowSystem.o: /usr/include/Performer/pf/pfScene.h
vsWindowSystem.o: /usr/include/Performer/pf/pfGroup.h
vsWindowSystem.o: /usr/include/Performer/pf/pfNode.h
vsWindowSystem.o: /usr/include/Performer/pr/pfGeoMath.h
vsWindowSystem.o: /usr/include/Performer/pr/pfGeoSet.h
vsWindowSystem.o: /usr/include/Performer/pr/pfFlux.h
vsWindowSystem.o: /usr/include/Performer/pr/pfEngine.h
vsWindowSystem.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsWindowSystem.o: /usr/include/Performer/pr/pfPassList.h
vsWindowSystem.o: /usr/include/Performer/pr/pfFBState.h
vsWindowSystem.o: ../../graphics/Performer/vsView.h++
vsWindowSystem.o: ../../util/common/vsVector.h++
vsWindowSystem.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsWindowSystem.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsWindowSystem.o: ../../graphics/Performer/vsComponent.h++
vsWindowSystem.o: ../../graphics/Performer/vsAttribute.h++
vsWindowSystem.o: ../../graphics/Performer/vsNode.h++
vsWindowSystem.o: ../../graphics/Performer/vsScene.h++
vsWindowSystem.o: ../../graphics/Performer/vsLightAttribute.h++
vsWindowSystem.o: /usr/include/Performer/pr/pfLight.h
vsWindowSystem.o: /usr/include/Performer/pf/pfLightSource.h
vsWindowSystem.o: ../../io/common/vsMouse.h++
vsWindowSystem.o: ../../io/common/vsInputDevice.h++
vsWindowSystem.o: ../../io/common/vsInputAxis.h++
vsWindowSystem.o: ../../io/common/vsInputButton.h++
vsWindowSystem.o: ../../io/common/vsKeyboard.h++ /usr/include/X11/keysym.h
vsWindowSystem.o: /usr/include/X11/keysymdef.h
