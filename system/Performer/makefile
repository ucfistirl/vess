#!/bin/make
# makefile for system/Performer

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../avatar/common \
	  -I../../motion/common \
	  -I../../input/common \
	  -I../../input/$(WS) \
	  -I../../input/$(SG)_$(WS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsSystem.o

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

vsSystem.o: vsSystem.h++ ../../graphics/Performer_X/vsPipe.h++
vsSystem.o: /usr/include/Performer/pf/pfPipe.h
vsSystem.o: /usr/include/Performer/pf/pfUpdatable.h
vsSystem.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsSystem.o: /usr/include/sys/types.h /usr/include/features.h
vsSystem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSystem.o: /usr/include/bits/types.h
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSystem.o: /usr/include/time.h /usr/include/endian.h
vsSystem.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsSystem.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsSystem.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsSystem.o: /usr/include/stdio.h /usr/include/libio.h
vsSystem.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSystem.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSystem.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsSystem.o: /usr/include/getopt.h /usr/include/ulocks.h /usr/include/malloc.h
vsSystem.o: /usr/include/limits.h
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSystem.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsSystem.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsSystem.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsSystem.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsSystem.o: /usr/include/errno.h /usr/include/bits/errno.h
vsSystem.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsSystem.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsSystem.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsSystem.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsSystem.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsSystem.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsSystem.o: /usr/include/Performer/linuxstubs.h
vsSystem.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsSystem.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsSystem.o: /usr/include/alloca.h /usr/include/string.h
vsSystem.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsSystem.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsSystem.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsSystem.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsSystem.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsSystem.o: /usr/include/Performer/pr/pfLinMath.h
vsSystem.o: /usr/include/Performer/pr/pfObject.h
vsSystem.o: /usr/include/Performer/pr/pfMemory.h
vsSystem.o: /usr/include/Performer/pr/pfType.h
vsSystem.o: /usr/include/Performer/pr/pfStruct.h
vsSystem.o: /usr/include/Performer/pfstats.h
vsSystem.o: /usr/include/Performer/pf/pfBuffer.h
vsSystem.o: /usr/include/Performer/pf/pfUpdate.h
vsSystem.o: /usr/include/Performer/pf/pfLists.h
vsSystem.o: /usr/include/Performer/pr/pfVideoChannel.h
vsSystem.o: /usr/include/Performer/pr/pfList.h
vsSystem.o: /usr/include/Performer/pr/pfCalligraphic.h
vsSystem.o: /usr/include/Performer/pr/pfStats.h
vsSystem.o: /usr/include/Performer/pr/pfDispList.h
vsSystem.o: /usr/include/Performer/pr/pfGeoState.h
vsSystem.o: /usr/include/Performer/pr/pfState.h
vsSystem.o: /usr/include/Performer/pr/pfByteBank.h
vsSystem.o: /usr/include/Performer/pr/pfGSetBank.h
vsSystem.o: ../../util/common/vsGlobals.h++
vsSystem.o: ../../graphics/Performer_X/vsScreen.h++
vsSystem.o: ../../graphics/Performer_X/vsWindow.h++
vsSystem.o: /usr/include/Performer/pf/pfPipeWindow.h
vsSystem.o: /usr/include/Performer/pr/pfWindow.h
vsSystem.o: ../../graphics/Performer_X/vsPane.h++
vsSystem.o: /usr/include/Performer/pf/pfChannel.h
vsSystem.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsSystem.o: /usr/include/Performer/pf/pfEarthSky.h
vsSystem.o: /usr/include/Performer/pf/pfLODState.h
vsSystem.o: /usr/include/Performer/pf/pfFrameStats.h
vsSystem.o: /usr/include/Performer/pf/pfScene.h
vsSystem.o: /usr/include/Performer/pf/pfGroup.h
vsSystem.o: /usr/include/Performer/pf/pfNode.h
vsSystem.o: /usr/include/Performer/pr/pfGeoMath.h
vsSystem.o: /usr/include/Performer/pr/pfGeoSet.h
vsSystem.o: /usr/include/Performer/pr/pfFlux.h
vsSystem.o: /usr/include/Performer/pr/pfEngine.h
vsSystem.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsSystem.o: /usr/include/Performer/pr/pfPassList.h
vsSystem.o: /usr/include/Performer/pr/pfFBState.h
vsSystem.o: ../../graphics/Performer/vsView.h++
vsSystem.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsSystem.o: ../../util/common/vsQuat.h++
vsSystem.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsSystem.o: ../../graphics/Performer/vsComponent.h++
vsSystem.o: ../../util/common/vsGrowableArray.h++
vsSystem.o: ../../graphics/Performer/vsAttribute.h++
vsSystem.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsSystem.o: ../../util/common/vsObjectMap.h++
vsSystem.o: ../../graphics/Performer/vsNode.h++
vsSystem.o: ../../graphics/Performer/vsScene.h++
vsSystem.o: ../../graphics/Performer/vsLightAttribute.h++
vsSystem.o: /usr/include/Performer/pr/pfLight.h
vsSystem.o: /usr/include/Performer/pf/pfLightSource.h
vsSystem.o: ../../util/common/vsTimer.h++ /usr/include/sys/time.h
vsSystem.o: /usr/include/Performer/pfutil.h /usr/include/Performer/pfdu.h
vsSystem.o: /usr/include/dlfcn.h /usr/include/bits/dlfcn.h
vsSystem.o: /usr/include/Performer/objfnt.h
vsSystem.o: ../../graphics/Performer/vsGeometry.h++
vsSystem.o: /usr/include/Performer/pf/pfGeode.h
vsSystem.o: ../../graphics/common/vsOptimizer.h++
vsSystem.o: ../../graphics/Performer/vsGraphicsState.h++
vsSystem.o: ../../graphics/Performer/vsBackfaceAttribute.h++
vsSystem.o: ../../graphics/Performer/vsStateAttribute.h++
vsSystem.o: ../../graphics/Performer/vsFogAttribute.h++
vsSystem.o: /usr/include/Performer/pr/pfFog.h
vsSystem.o: ../../graphics/Performer/vsMaterialAttribute.h++
vsSystem.o: /usr/include/Performer/pr/pfMaterial.h
vsSystem.o: ../../graphics/Performer/vsShadingAttribute.h++
vsSystem.o: ../../graphics/Performer/vsTextureAttribute.h++
vsSystem.o: /usr/include/Performer/pr/pfTexture.h
vsSystem.o: /usr/include/Performer/image.h
vsSystem.o: ../../graphics/Performer/vsTransparencyAttribute.h++
vsSystem.o: ../../graphics/Performer/vsWireframeAttribute.h++
vsSystem.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsSystem.o: /usr/include/Performer/pfdb/pfflt.h
