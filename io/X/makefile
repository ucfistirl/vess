#!/bin/make
# makefile for io/X

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../io/common \
	  -I../../io/$(SG)_$(WS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsWSSpaceball.o

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

vsWSSpaceball.o: /usr/include/stdio.h /usr/include/features.h
vsWSSpaceball.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWSSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWSSpaceball.o: /usr/include/bits/types.h /usr/include/libio.h
vsWSSpaceball.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWSSpaceball.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWSSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWSSpaceball.o: /usr/include/bits/stdio_lim.h
vsWSSpaceball.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWSSpaceball.o: vsWSSpaceball.h++ ../../io/common/vsInputSystem.h++
vsWSSpaceball.o: ../../util/common/vsUpdatable.h++
vsWSSpaceball.o: ../../util/common/vsObject.h++
vsWSSpaceball.o: ../../util/common/vsTreeMap.h++
vsWSSpaceball.o: ../../util/common/vsGrowableArray.h++
vsWSSpaceball.o: ../../io/common/vsSpaceball.h++
vsWSSpaceball.o: ../../io/common/vs6DInputDevice.h++
vsWSSpaceball.o: ../../util/common/vsVector.h++
vsWSSpaceball.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWSSpaceball.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsWSSpaceball.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsWSSpaceball.o: ../../util/common/vsQuat.h++
vsWSSpaceball.o: ../../io/common/vsInputDevice.h++
vsWSSpaceball.o: ../../io/common/vsInputAxis.h++
vsWSSpaceball.o: ../../io/common/vsInputButton.h++
vsWSSpaceball.o: ../../io/Performer_X/vsWindowSystem.h++
vsWSSpaceball.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
vsWSSpaceball.o: /usr/include/time.h /usr/include/endian.h
vsWSSpaceball.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsWSSpaceball.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsWSSpaceball.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsWSSpaceball.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
vsWSSpaceball.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsWSSpaceball.o: /usr/include/X11/Xos.h /usr/include/string.h
vsWSSpaceball.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsWSSpaceball.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
vsWSSpaceball.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
vsWSSpaceball.o: /usr/include/bits/confname.h /usr/include/sys/time.h
vsWSSpaceball.o: /usr/include/X11/Xarch.h ../../util/common/vsObjectMap.h++
vsWSSpaceball.o: ../../graphics/Performer_X/vsWindow.h++
vsWSSpaceball.o: /usr/include/Performer/pf/pfPipeWindow.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfUpdatable.h
vsWSSpaceball.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsWSSpaceball.o: /usr/include/ulocks.h /usr/include/malloc.h
vsWSSpaceball.o: /usr/include/limits.h
vsWSSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsWSSpaceball.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsWSSpaceball.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsWSSpaceball.o: /usr/include/bits/xopen_lim.h /usr/include/errno.h
vsWSSpaceball.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsWSSpaceball.o: /usr/include/asm/errno.h /usr/include/GL/gl.h
vsWSSpaceball.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsWSSpaceball.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsWSSpaceball.o: /usr/include/Performer/linuxstubs.h
vsWSSpaceball.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsWSSpaceball.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsWSSpaceball.o: /usr/include/alloca.h /usr/include/sys/prctl.h
vsWSSpaceball.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsWSSpaceball.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsWSSpaceball.o: /usr/include/Performer/prstats.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfLinMath.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfObject.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfMemory.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfType.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfStruct.h
vsWSSpaceball.o: /usr/include/Performer/pfstats.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfBuffer.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfUpdate.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfLists.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfPipe.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfVideoChannel.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfList.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfCalligraphic.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfStats.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfDispList.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfGeoState.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfState.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfByteBank.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfGSetBank.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfWindow.h
vsWSSpaceball.o: ../../graphics/Performer_X/vsScreen.h++
vsWSSpaceball.o: ../../graphics/Performer_X/vsPipe.h++
vsWSSpaceball.o: ../../graphics/Performer_X/vsPane.h++
vsWSSpaceball.o: /usr/include/Performer/pf/pfChannel.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfEarthSky.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfLODState.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfFrameStats.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfScene.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfGroup.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfNode.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfGeoMath.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfGeoSet.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfFlux.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfEngine.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfPassList.h
vsWSSpaceball.o: /usr/include/Performer/pr/pfFBState.h
vsWSSpaceball.o: ../../graphics/Performer/vsView.h++
vsWSSpaceball.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsWSSpaceball.o: ../../graphics/Performer/vsComponent.h++
vsWSSpaceball.o: ../../graphics/Performer/vsAttribute.h++
vsWSSpaceball.o: ../../graphics/Performer/vsNode.h++
vsWSSpaceball.o: ../../graphics/Performer/vsScene.h++
vsWSSpaceball.o: ../../graphics/Performer/vsLightAttribute.h++
vsWSSpaceball.o: /usr/include/Performer/pr/pfLight.h
vsWSSpaceball.o: /usr/include/Performer/pf/pfLightSource.h
vsWSSpaceball.o: ../../io/common/vsMouse.h++ ../../io/common/vsKeyboard.h++
vsWSSpaceball.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
vsWSSpaceball.o: /usr/include/X11/extensions/XI.h
vsWSSpaceball.o: /usr/include/X11/extensions/XInput.h
