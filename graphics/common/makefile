#!/bin/make
# makefile for graphics/common

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsOptimizer.o \
          vsScenePrinter.o

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

vsOptimizer.o: /usr/include/stdio.h /usr/include/features.h
vsOptimizer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsOptimizer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsOptimizer.o: /usr/include/bits/types.h /usr/include/libio.h
vsOptimizer.o: /usr/include/_G_config.h /usr/include/wchar.h
vsOptimizer.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsOptimizer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsOptimizer.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsOptimizer.o: /usr/include/getopt.h vsOptimizer.h++
vsOptimizer.o: ../../graphics/Performer/vsAttribute.h++ /usr/include/stdlib.h
vsOptimizer.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsOptimizer.o: /usr/include/endian.h /usr/include/bits/endian.h
vsOptimizer.o: /usr/include/alloca.h /usr/include/Performer/pr/pfGeoState.h
vsOptimizer.o: /usr/include/Performer/pr/pfObject.h
vsOptimizer.o: /usr/include/Performer/pr/pfMemory.h /usr/include/sys/types.h
vsOptimizer.o: /usr/include/time.h /usr/include/sys/select.h
vsOptimizer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsOptimizer.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsOptimizer.o: /usr/include/string.h /usr/include/Performer/pr.h
vsOptimizer.o: /usr/include/ulocks.h /usr/include/malloc.h
vsOptimizer.o: /usr/include/limits.h
vsOptimizer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsOptimizer.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsOptimizer.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsOptimizer.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsOptimizer.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsOptimizer.o: /usr/include/errno.h /usr/include/bits/errno.h
vsOptimizer.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsOptimizer.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsOptimizer.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsOptimizer.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsOptimizer.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsOptimizer.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsOptimizer.o: /usr/include/Performer/linuxstubs.h
vsOptimizer.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsOptimizer.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsOptimizer.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsOptimizer.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsOptimizer.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsOptimizer.o: /usr/include/Performer/prstats.h
vsOptimizer.o: /usr/include/Performer/pr/pfType.h
vsOptimizer.o: ../../util/common/vsGlobals.h++ ../../util/common/vsObject.h++
vsOptimizer.o: ../../util/common/vsTreeMap.h++
vsOptimizer.o: ../../util/common/vsGrowableArray.h++
vsOptimizer.o: ../../util/common/vsObjectMap.h++
vsOptimizer.o: ../../graphics/Performer/vsGeometry.h++
vsOptimizer.o: /usr/include/Performer/pf/pfGeode.h
vsOptimizer.o: /usr/include/Performer/pf/pfNode.h
vsOptimizer.o: /usr/include/Performer/pf/pfUpdatable.h
vsOptimizer.o: /usr/include/Performer/pf.h
vsOptimizer.o: /usr/include/Performer/pr/pfLinMath.h
vsOptimizer.o: /usr/include/Performer/pr/pfStruct.h
vsOptimizer.o: /usr/include/Performer/pfstats.h
vsOptimizer.o: /usr/include/Performer/pf/pfBuffer.h
vsOptimizer.o: /usr/include/Performer/pf/pfUpdate.h
vsOptimizer.o: /usr/include/Performer/pf/pfLists.h
vsOptimizer.o: /usr/include/Performer/pf/pfFrameStats.h
vsOptimizer.o: /usr/include/Performer/pf/pfPipe.h
vsOptimizer.o: /usr/include/Performer/pr/pfVideoChannel.h
vsOptimizer.o: /usr/include/Performer/pr/pfList.h
vsOptimizer.o: /usr/include/Performer/pr/pfCalligraphic.h
vsOptimizer.o: /usr/include/Performer/pr/pfStats.h
vsOptimizer.o: /usr/include/Performer/pr/pfDispList.h
vsOptimizer.o: /usr/include/Performer/pr/pfState.h
vsOptimizer.o: /usr/include/Performer/pr/pfByteBank.h
vsOptimizer.o: /usr/include/Performer/pr/pfGSetBank.h
vsOptimizer.o: /usr/include/Performer/pr/pfGeoMath.h
vsOptimizer.o: /usr/include/Performer/pr/pfFlux.h
vsOptimizer.o: /usr/include/Performer/pr/pfEngine.h
vsOptimizer.o: /usr/include/Performer/pr/pfGeoSet.h
vsOptimizer.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsOptimizer.o: /usr/include/Performer/pr/pfPassList.h
vsOptimizer.o: /usr/include/Performer/pr/pfFBState.h
vsOptimizer.o: /usr/include/Performer/pr/pfWindow.h
vsOptimizer.o: ../../util/common/vsVector.h++
vsOptimizer.o: ../../graphics/Performer/vsNode.h++
vsOptimizer.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsOptimizer.o: ../../graphics/Performer/vsComponent.h++
vsOptimizer.o: /usr/include/Performer/pf/pfGroup.h
vsOptimizer.o: ../../graphics/Performer/vsStateAttribute.h++
vsOptimizer.o: ../../graphics/Performer/vsDecalAttribute.h++
vsOptimizer.o: /usr/include/Performer/pf/pfLayer.h
vsOptimizer.o: /usr/include/Performer/pf/pfChannel.h
vsOptimizer.o: /usr/include/Performer/pf/pfPipeWindow.h
vsOptimizer.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsOptimizer.o: /usr/include/Performer/pf/pfEarthSky.h
vsOptimizer.o: /usr/include/Performer/pf/pfLODState.h
vsOptimizer.o: /usr/include/Performer/pf/pfScene.h
vsOptimizer.o: ../../graphics/Performer/vsLODAttribute.h++
vsOptimizer.o: /usr/include/Performer/pf/pfLOD.h
vsOptimizer.o: /usr/include/Performer/pf/pfTraverser.h
vsOptimizer.o: /usr/include/Performer/pr/pfGenericMatrix.h
vsScenePrinter.o: vsScenePrinter.h++ /usr/include/stdio.h
vsScenePrinter.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsScenePrinter.o: /usr/include/gnu/stubs.h
vsScenePrinter.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsScenePrinter.o: /usr/include/bits/types.h /usr/include/libio.h
vsScenePrinter.o: /usr/include/_G_config.h /usr/include/wchar.h
vsScenePrinter.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsScenePrinter.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsScenePrinter.o: /usr/include/bits/stdio_lim.h
vsScenePrinter.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsScenePrinter.o: ../../graphics/Performer/vsNode.h++
vsScenePrinter.o: ../../util/common/vsObject.h++
vsScenePrinter.o: ../../util/common/vsTreeMap.h++
vsScenePrinter.o: ../../util/common/vsGrowableArray.h++
vsScenePrinter.o: ../../graphics/Performer/vsAttribute.h++
vsScenePrinter.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsScenePrinter.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsScenePrinter.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsScenePrinter.o: /usr/include/Performer/pr/pfGeoState.h
vsScenePrinter.o: /usr/include/Performer/pr/pfObject.h
vsScenePrinter.o: /usr/include/Performer/pr/pfMemory.h
vsScenePrinter.o: /usr/include/sys/types.h /usr/include/time.h
vsScenePrinter.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsScenePrinter.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsScenePrinter.o: /usr/include/sys/sysmacros.h /usr/include/string.h
vsScenePrinter.o: /usr/include/Performer/pr.h /usr/include/ulocks.h
vsScenePrinter.o: /usr/include/malloc.h /usr/include/limits.h
vsScenePrinter.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsScenePrinter.o: /usr/include/bits/posix1_lim.h
vsScenePrinter.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsScenePrinter.o: /usr/include/bits/posix2_lim.h
vsScenePrinter.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsScenePrinter.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsScenePrinter.o: /usr/include/errno.h /usr/include/bits/errno.h
vsScenePrinter.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsScenePrinter.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsScenePrinter.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsScenePrinter.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsScenePrinter.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsScenePrinter.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsScenePrinter.o: /usr/include/Performer/linuxstubs.h
vsScenePrinter.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsScenePrinter.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsScenePrinter.o: /usr/include/bits/wordsize.h
vsScenePrinter.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsScenePrinter.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsScenePrinter.o: /usr/include/bits/mathcalls.h
vsScenePrinter.o: /usr/include/Performer/prstats.h
vsScenePrinter.o: /usr/include/Performer/pr/pfType.h
vsScenePrinter.o: ../../util/common/vsGlobals.h++
vsScenePrinter.o: ../../util/common/vsObjectMap.h++
vsScenePrinter.o: ../../util/common/vsVector.h++
vsScenePrinter.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsScenePrinter.o: ../../graphics/Performer/vsGeometry.h++
vsScenePrinter.o: /usr/include/Performer/pf/pfGeode.h
vsScenePrinter.o: /usr/include/Performer/pf/pfNode.h
vsScenePrinter.o: /usr/include/Performer/pf/pfUpdatable.h
vsScenePrinter.o: /usr/include/Performer/pf.h
vsScenePrinter.o: /usr/include/Performer/pr/pfLinMath.h
vsScenePrinter.o: /usr/include/Performer/pr/pfStruct.h
vsScenePrinter.o: /usr/include/Performer/pfstats.h
vsScenePrinter.o: /usr/include/Performer/pf/pfBuffer.h
vsScenePrinter.o: /usr/include/Performer/pf/pfUpdate.h
vsScenePrinter.o: /usr/include/Performer/pf/pfLists.h
vsScenePrinter.o: /usr/include/Performer/pf/pfFrameStats.h
vsScenePrinter.o: /usr/include/Performer/pf/pfPipe.h
vsScenePrinter.o: /usr/include/Performer/pr/pfVideoChannel.h
vsScenePrinter.o: /usr/include/Performer/pr/pfList.h
vsScenePrinter.o: /usr/include/Performer/pr/pfCalligraphic.h
vsScenePrinter.o: /usr/include/Performer/pr/pfStats.h
vsScenePrinter.o: /usr/include/Performer/pr/pfDispList.h
vsScenePrinter.o: /usr/include/Performer/pr/pfState.h
vsScenePrinter.o: /usr/include/Performer/pr/pfByteBank.h
vsScenePrinter.o: /usr/include/Performer/pr/pfGSetBank.h
vsScenePrinter.o: /usr/include/Performer/pr/pfGeoMath.h
vsScenePrinter.o: /usr/include/Performer/pr/pfFlux.h
vsScenePrinter.o: /usr/include/Performer/pr/pfEngine.h
vsScenePrinter.o: /usr/include/Performer/pr/pfGeoSet.h
vsScenePrinter.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsScenePrinter.o: /usr/include/Performer/pr/pfPassList.h
vsScenePrinter.o: /usr/include/Performer/pr/pfFBState.h
vsScenePrinter.o: /usr/include/Performer/pr/pfWindow.h
vsScenePrinter.o: ../../graphics/Performer/vsComponent.h++
vsScenePrinter.o: /usr/include/Performer/pf/pfGroup.h
vsScenePrinter.o: ../../graphics/Performer/vsBackfaceAttribute.h++
vsScenePrinter.o: ../../graphics/Performer/vsStateAttribute.h++
vsScenePrinter.o: ../../graphics/Performer/vsMaterialAttribute.h++
vsScenePrinter.o: /usr/include/Performer/pr/pfMaterial.h
vsScenePrinter.o: ../../graphics/Performer/vsShadingAttribute.h++
vsScenePrinter.o: ../../graphics/Performer/vsTransparencyAttribute.h++
vsScenePrinter.o: ../../graphics/Performer/vsTransformAttribute.h++
vsScenePrinter.o: /usr/include/Performer/pf/pfDCS.h
vsScenePrinter.o: /usr/include/Performer/pf/pfSCS.h
vsScenePrinter.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsScenePrinter.o: /usr/include/Performer/pfdb/pfflt.h
vsScenePrinter.o: ../../graphics/Performer/vsTextureAttribute.h++
vsScenePrinter.o: /usr/include/Performer/pr/pfTexture.h
vsScenePrinter.o: /usr/include/Performer/image.h
