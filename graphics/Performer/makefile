#!/bin/make
# makefile for graphics/Performer

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../graphics/common \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/gcc-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsAttribute.o \
          vsBackfaceAttribute.o \
          vsBillboardAttribute.o \
          vsComponent.o \
          vsDatabaseLoader.o \
          vsDecalAttribute.o \
          vsDynamicGeometry.o \
          vsFogAttribute.o \
          vsGeometry.o \
          vsGraphicsState.o \
          vsIntersect.o \
          vsLODAttribute.o \
          vsLightAttribute.o \
          vsMaterialAttribute.o \
          vsNode.o \
          vsScene.o \
          vsSequenceAttribute.o \
          vsShadingAttribute.o \
          vsStateAttribute.o \
          vsSwitchAttribute.o \
          vsTextureAttribute.o \
          vsTransformAttribute.o \
          vsTransparencyAttribute.o \
          vsView.o \
          vsViewpointAttribute.o \
          vsWireframeAttribute.o \
          vsTextBuilder.o

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

vsAttribute.o: vsAttribute.h++ /usr/include/stdlib.h /usr/include/features.h
vsAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsAttribute.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsAttribute.o: /usr/include/endian.h /usr/include/bits/endian.h
vsAttribute.o: /usr/include/alloca.h /usr/include/Performer/pr/pfGeoState.h
vsAttribute.o: /usr/include/Performer/pr/pfObject.h
vsAttribute.o: /usr/include/Performer/pr/pfMemory.h /usr/include/sys/types.h
vsAttribute.o: /usr/include/bits/types.h /usr/include/time.h
vsAttribute.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsAttribute.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsAttribute.o: /usr/include/sys/sysmacros.h /usr/include/string.h
vsAttribute.o: /usr/include/Performer/pr.h /usr/include/stdio.h
vsAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsAttribute.o: /usr/include/gconv.h
vsAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsAttribute.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsAttribute.o: /usr/include/getopt.h /usr/include/ulocks.h
vsAttribute.o: /usr/include/malloc.h /usr/include/limits.h
vsAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsAttribute.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsAttribute.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsAttribute.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsAttribute.o: /usr/include/errno.h /usr/include/bits/errno.h
vsAttribute.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsAttribute.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsAttribute.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsAttribute.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsAttribute.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsAttribute.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsAttribute.o: /usr/include/Performer/linuxstubs.h
vsAttribute.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsAttribute.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsAttribute.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsAttribute.o: /usr/include/Performer/prstats.h
vsAttribute.o: /usr/include/Performer/pr/pfType.h
vsAttribute.o: ../../util/common/vsGlobals.h++ ../../util/common/vsObject.h++
vsAttribute.o: ../../util/common/vsTreeMap.h++
vsAttribute.o: ../../util/common/vsGrowableArray.h++
vsAttribute.o: ../../util/common/vsObjectMap.h++
vsBackfaceAttribute.o: vsBackfaceAttribute.h++ vsStateAttribute.h++
vsBackfaceAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsBackfaceAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsBackfaceAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsBackfaceAttribute.o: /usr/include/bits/mathdef.h
vsBackfaceAttribute.o: /usr/include/bits/mathcalls.h
vsBackfaceAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsBackfaceAttribute.o: /usr/include/stdlib.h
vsBackfaceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsBackfaceAttribute.o: /usr/include/bits/waitflags.h
vsBackfaceAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsBackfaceAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfObject.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsBackfaceAttribute.o: /usr/include/sys/types.h /usr/include/bits/types.h
vsBackfaceAttribute.o: /usr/include/time.h /usr/include/sys/select.h
vsBackfaceAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsBackfaceAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsBackfaceAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsBackfaceAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsBackfaceAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsBackfaceAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsBackfaceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsBackfaceAttribute.o: /usr/include/bits/stdio_lim.h
vsBackfaceAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsBackfaceAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsBackfaceAttribute.o: /usr/include/limits.h
vsBackfaceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsBackfaceAttribute.o: /usr/include/bits/posix1_lim.h
vsBackfaceAttribute.o: /usr/include/bits/local_lim.h
vsBackfaceAttribute.o: /usr/include/linux/limits.h
vsBackfaceAttribute.o: /usr/include/bits/posix2_lim.h
vsBackfaceAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsBackfaceAttribute.o: /usr/include/bits/posix_opt.h
vsBackfaceAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsBackfaceAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsBackfaceAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsBackfaceAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsBackfaceAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsBackfaceAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsBackfaceAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsBackfaceAttribute.o: /usr/include/Performer/opengl.h
vsBackfaceAttribute.o: /usr/include/Performer/linuxstubs.h
vsBackfaceAttribute.o: /usr/include/Performer/image.h
vsBackfaceAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsBackfaceAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsBackfaceAttribute.o: /usr/include/Performer/prmath.h
vsBackfaceAttribute.o: /usr/include/Performer/prstats.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfType.h
vsBackfaceAttribute.o: ../../util/common/vsObject.h++
vsBackfaceAttribute.o: ../../util/common/vsTreeMap.h++
vsBackfaceAttribute.o: ../../util/common/vsObjectMap.h++
vsBackfaceAttribute.o: /usr/include/Performer/pf.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsBackfaceAttribute.o: /usr/include/Performer/pfstats.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfLight.h vsNode.h++
vsBackfaceAttribute.o: ../../util/common/vsVector.h++
vsBackfaceAttribute.o: ../../util/common/vsMatrix.h++
vsBackfaceAttribute.o: ../../util/common/vsQuat.h++ vsGraphicsState.h++
vsBackfaceAttribute.o: vsFogAttribute.h++ /usr/include/Performer/pr/pfFog.h
vsBackfaceAttribute.o: vsMaterialAttribute.h++
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsBackfaceAttribute.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfList.h
vsBackfaceAttribute.o: /usr/include/Performer/image.h
vsBackfaceAttribute.o: vsTransparencyAttribute.h++ vsWireframeAttribute.h++
vsBackfaceAttribute.o: vsLightAttribute.h++
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfNode.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfLists.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsBackfaceAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfStats.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfState.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsBackfaceAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsBillboardAttribute.o: vsBillboardAttribute.h++
vsBillboardAttribute.o: /usr/include/Performer/pf/pfBillboard.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfGeode.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfNode.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsBillboardAttribute.o: /usr/include/Performer/pf.h
vsBillboardAttribute.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsBillboardAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsBillboardAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsBillboardAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsBillboardAttribute.o: /usr/include/time.h /usr/include/endian.h
vsBillboardAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsBillboardAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsBillboardAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsBillboardAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsBillboardAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsBillboardAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsBillboardAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsBillboardAttribute.o: /usr/include/bits/stdio_lim.h
vsBillboardAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsBillboardAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsBillboardAttribute.o: /usr/include/limits.h
vsBillboardAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsBillboardAttribute.o: /usr/include/bits/posix1_lim.h
vsBillboardAttribute.o: /usr/include/bits/local_lim.h
vsBillboardAttribute.o: /usr/include/linux/limits.h
vsBillboardAttribute.o: /usr/include/bits/posix2_lim.h
vsBillboardAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsBillboardAttribute.o: /usr/include/bits/posix_opt.h
vsBillboardAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsBillboardAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsBillboardAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsBillboardAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsBillboardAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsBillboardAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsBillboardAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsBillboardAttribute.o: /usr/include/Performer/opengl.h
vsBillboardAttribute.o: /usr/include/Performer/linuxstubs.h
vsBillboardAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsBillboardAttribute.o: /usr/include/bits/waitflags.h
vsBillboardAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsBillboardAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsBillboardAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsBillboardAttribute.o: /usr/include/bits/wordsize.h
vsBillboardAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsBillboardAttribute.o: /usr/include/bits/huge_val.h
vsBillboardAttribute.o: /usr/include/bits/mathdef.h
vsBillboardAttribute.o: /usr/include/bits/mathcalls.h
vsBillboardAttribute.o: /usr/include/Performer/prstats.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfObject.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfType.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsBillboardAttribute.o: /usr/include/Performer/pfstats.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfLists.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfList.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfStats.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfState.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfFlux.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfEngine.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfCuller.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfTraverser.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfChannel.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfPipeWindow.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfWindow.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfEarthSky.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfLODState.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfScene.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfGeoSet.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfPassList.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfFBState.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfGenericMatrix.h
vsBillboardAttribute.o: /usr/include/Performer/pr/pfSprite.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfDCS.h
vsBillboardAttribute.o: /usr/include/Performer/pf/pfSCS.h
vsBillboardAttribute.o: ../../util/common/vsMatrix.h++
vsBillboardAttribute.o: ../../util/common/vsGlobals.h++
vsBillboardAttribute.o: ../../util/common/vsVector.h++
vsBillboardAttribute.o: ../../util/common/vsQuat.h++ vsAttribute.h++
vsBillboardAttribute.o: ../../util/common/vsObject.h++
vsBillboardAttribute.o: ../../util/common/vsTreeMap.h++
vsBillboardAttribute.o: ../../util/common/vsGrowableArray.h++
vsBillboardAttribute.o: ../../util/common/vsObjectMap.h++ vsComponent.h++
vsBillboardAttribute.o: vsNode.h++
vsComponent.o: vsComponent.h++ /usr/include/Performer/pf/pfGroup.h
vsComponent.o: /usr/include/Performer/pf/pfNode.h
vsComponent.o: /usr/include/Performer/pf/pfUpdatable.h
vsComponent.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsComponent.o: /usr/include/sys/types.h /usr/include/features.h
vsComponent.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsComponent.o: /usr/include/bits/types.h
vsComponent.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsComponent.o: /usr/include/time.h /usr/include/endian.h
vsComponent.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsComponent.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsComponent.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsComponent.o: /usr/include/stdio.h /usr/include/libio.h
vsComponent.o: /usr/include/_G_config.h /usr/include/wchar.h
vsComponent.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsComponent.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsComponent.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsComponent.o: /usr/include/getopt.h /usr/include/ulocks.h
vsComponent.o: /usr/include/malloc.h /usr/include/limits.h
vsComponent.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsComponent.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsComponent.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsComponent.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsComponent.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsComponent.o: /usr/include/errno.h /usr/include/bits/errno.h
vsComponent.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsComponent.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsComponent.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsComponent.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsComponent.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsComponent.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsComponent.o: /usr/include/Performer/linuxstubs.h
vsComponent.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsComponent.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsComponent.o: /usr/include/alloca.h /usr/include/string.h
vsComponent.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsComponent.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsComponent.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsComponent.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsComponent.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsComponent.o: /usr/include/Performer/pr/pfLinMath.h
vsComponent.o: /usr/include/Performer/pr/pfObject.h
vsComponent.o: /usr/include/Performer/pr/pfMemory.h
vsComponent.o: /usr/include/Performer/pr/pfType.h
vsComponent.o: /usr/include/Performer/pr/pfStruct.h
vsComponent.o: /usr/include/Performer/pfstats.h
vsComponent.o: /usr/include/Performer/pf/pfBuffer.h
vsComponent.o: /usr/include/Performer/pf/pfUpdate.h
vsComponent.o: /usr/include/Performer/pf/pfLists.h
vsComponent.o: /usr/include/Performer/pf/pfFrameStats.h
vsComponent.o: /usr/include/Performer/pf/pfPipe.h
vsComponent.o: /usr/include/Performer/pr/pfVideoChannel.h
vsComponent.o: /usr/include/Performer/pr/pfList.h
vsComponent.o: /usr/include/Performer/pr/pfCalligraphic.h
vsComponent.o: /usr/include/Performer/pr/pfStats.h
vsComponent.o: /usr/include/Performer/pr/pfDispList.h
vsComponent.o: /usr/include/Performer/pr/pfGeoState.h
vsComponent.o: /usr/include/Performer/pr/pfState.h
vsComponent.o: /usr/include/Performer/pr/pfByteBank.h
vsComponent.o: /usr/include/Performer/pr/pfGSetBank.h
vsComponent.o: /usr/include/Performer/pr/pfGeoMath.h
vsComponent.o: ../../util/common/vsGrowableArray.h++
vsComponent.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsComponent.o: vsAttribute.h++ ../../util/common/vsObject.h++
vsComponent.o: ../../util/common/vsTreeMap.h++
vsComponent.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsComponent.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsComponent.o: vsTransformAttribute.h++ /usr/include/Performer/pf/pfDCS.h
vsComponent.o: /usr/include/Performer/pf/pfSCS.h vsDatabaseLoader.h++
vsComponent.o: /usr/include/Performer/pfdb/pfflt.h vsGeometry.h++
vsComponent.o: /usr/include/Performer/pf/pfGeode.h
vsComponent.o: /usr/include/Performer/pr/pfFlux.h
vsComponent.o: /usr/include/Performer/pr/pfEngine.h
vsComponent.o: /usr/include/Performer/pr/pfGeoSet.h
vsComponent.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsComponent.o: /usr/include/Performer/pr/pfPassList.h
vsComponent.o: /usr/include/Performer/pr/pfFBState.h
vsComponent.o: /usr/include/Performer/pr/pfWindow.h vsSwitchAttribute.h++
vsComponent.o: /usr/include/Performer/pf/pfSwitch.h vsSequenceAttribute.h++
vsComponent.o: /usr/include/Performer/pf/pfSequence.h vsLODAttribute.h++
vsComponent.o: /usr/include/Performer/pf/pfLOD.h
vsComponent.o: /usr/include/Performer/pf/pfLODState.h
vsComponent.o: /usr/include/Performer/pf/pfTraverser.h
vsComponent.o: /usr/include/Performer/pf/pfChannel.h
vsComponent.o: /usr/include/Performer/pf/pfPipeWindow.h
vsComponent.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsComponent.o: /usr/include/Performer/pf/pfEarthSky.h
vsComponent.o: /usr/include/Performer/pf/pfScene.h
vsComponent.o: /usr/include/Performer/pr/pfGenericMatrix.h
vsComponent.o: vsDecalAttribute.h++ /usr/include/Performer/pf/pfLayer.h
vsComponent.o: vsBillboardAttribute.h++
vsComponent.o: /usr/include/Performer/pf/pfBillboard.h
vsComponent.o: /usr/include/Performer/pf/pfCuller.h
vsComponent.o: /usr/include/Performer/pr/pfSprite.h vsDynamicGeometry.h++
vsDatabaseLoader.o: /usr/include/stdio.h /usr/include/features.h
vsDatabaseLoader.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsDatabaseLoader.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDatabaseLoader.o: /usr/include/bits/types.h /usr/include/libio.h
vsDatabaseLoader.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDatabaseLoader.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDatabaseLoader.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDatabaseLoader.o: /usr/include/bits/stdio_lim.h
vsDatabaseLoader.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDatabaseLoader.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsDatabaseLoader.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsDatabaseLoader.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsDatabaseLoader.o: /usr/include/string.h /usr/include/Performer/pfdu.h
vsDatabaseLoader.o: /usr/include/dlfcn.h /usr/include/bits/dlfcn.h
vsDatabaseLoader.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsDatabaseLoader.o: /usr/include/sys/types.h /usr/include/time.h
vsDatabaseLoader.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsDatabaseLoader.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsDatabaseLoader.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsDatabaseLoader.o: /usr/include/malloc.h /usr/include/limits.h
vsDatabaseLoader.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsDatabaseLoader.o: /usr/include/bits/posix1_lim.h
vsDatabaseLoader.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsDatabaseLoader.o: /usr/include/bits/posix2_lim.h
vsDatabaseLoader.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsDatabaseLoader.o: /usr/include/bits/posix_opt.h
vsDatabaseLoader.o: /usr/include/bits/confname.h /usr/include/errno.h
vsDatabaseLoader.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsDatabaseLoader.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsDatabaseLoader.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsDatabaseLoader.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsDatabaseLoader.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsDatabaseLoader.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsDatabaseLoader.o: /usr/include/Performer/opengl.h
vsDatabaseLoader.o: /usr/include/Performer/linuxstubs.h
vsDatabaseLoader.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsDatabaseLoader.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsDatabaseLoader.o: /usr/include/bits/wordsize.h
vsDatabaseLoader.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsDatabaseLoader.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsDatabaseLoader.o: /usr/include/bits/mathcalls.h
vsDatabaseLoader.o: /usr/include/Performer/prstats.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfLinMath.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfObject.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfMemory.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfType.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfStruct.h
vsDatabaseLoader.o: /usr/include/Performer/pfstats.h
vsDatabaseLoader.o: /usr/include/Performer/pfutil.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfGeoMath.h
vsDatabaseLoader.o: /usr/include/Performer/objfnt.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfLOD.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfGroup.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfNode.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfUpdatable.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfBuffer.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfUpdate.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfLists.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfFrameStats.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfPipe.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfVideoChannel.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfList.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfCalligraphic.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfStats.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfDispList.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfGeoState.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfState.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfByteBank.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfGSetBank.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfLODState.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfFlux.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfEngine.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfTraverser.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfChannel.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfPipeWindow.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfWindow.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfEarthSky.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfScene.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfGeoSet.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfPassList.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfFBState.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfGenericMatrix.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfSequence.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfSwitch.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfDCS.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfSCS.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfBillboard.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfGeode.h
vsDatabaseLoader.o: /usr/include/Performer/pf/pfCuller.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfSprite.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfFog.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfMaterial.h
vsDatabaseLoader.o: /usr/include/Performer/pr/pfTexture.h
vsDatabaseLoader.o: /usr/include/Performer/image.h vsDatabaseLoader.h++
vsDatabaseLoader.o: /usr/include/Performer/pfdb/pfflt.h vsComponent.h++
vsDatabaseLoader.o: ../../util/common/vsGrowableArray.h++
vsDatabaseLoader.o: ../../util/common/vsVector.h++
vsDatabaseLoader.o: ../../util/common/vsGlobals.h++ vsAttribute.h++
vsDatabaseLoader.o: ../../util/common/vsObject.h++
vsDatabaseLoader.o: ../../util/common/vsTreeMap.h++
vsDatabaseLoader.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsDatabaseLoader.o: ../../util/common/vsMatrix.h++
vsDatabaseLoader.o: ../../util/common/vsQuat.h++ vsGeometry.h++
vsDatabaseLoader.o: vsDecalAttribute.h++ /usr/include/Performer/pf/pfLayer.h
vsDatabaseLoader.o: vsLODAttribute.h++ vsSequenceAttribute.h++
vsDatabaseLoader.o: vsSwitchAttribute.h++ vsTransformAttribute.h++
vsDatabaseLoader.o: vsBillboardAttribute.h++ vsFogAttribute.h++
vsDatabaseLoader.o: vsStateAttribute.h++ vsMaterialAttribute.h++
vsDatabaseLoader.o: vsTextureAttribute.h++ vsBackfaceAttribute.h++
vsDatabaseLoader.o: vsTransparencyAttribute.h++ vsWireframeAttribute.h++
vsDatabaseLoader.o: vsShadingAttribute.h++
vsDatabaseLoader.o: ../../graphics/common/vsOptimizer.h++
vsDecalAttribute.o: vsDecalAttribute.h++ /usr/include/Performer/pf/pfLayer.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfNode.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsDecalAttribute.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsDecalAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsDecalAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsDecalAttribute.o: /usr/include/bits/types.h
vsDecalAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDecalAttribute.o: /usr/include/time.h /usr/include/endian.h
vsDecalAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsDecalAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsDecalAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsDecalAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsDecalAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDecalAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDecalAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDecalAttribute.o: /usr/include/bits/stdio_lim.h
vsDecalAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDecalAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsDecalAttribute.o: /usr/include/limits.h
vsDecalAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsDecalAttribute.o: /usr/include/bits/posix1_lim.h
vsDecalAttribute.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsDecalAttribute.o: /usr/include/bits/posix2_lim.h
vsDecalAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsDecalAttribute.o: /usr/include/bits/posix_opt.h
vsDecalAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsDecalAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsDecalAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsDecalAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsDecalAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsDecalAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsDecalAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsDecalAttribute.o: /usr/include/Performer/opengl.h
vsDecalAttribute.o: /usr/include/Performer/linuxstubs.h
vsDecalAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsDecalAttribute.o: /usr/include/bits/waitflags.h
vsDecalAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsDecalAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsDecalAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsDecalAttribute.o: /usr/include/bits/wordsize.h
vsDecalAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsDecalAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsDecalAttribute.o: /usr/include/bits/mathcalls.h
vsDecalAttribute.o: /usr/include/Performer/prstats.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfObject.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfType.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsDecalAttribute.o: /usr/include/Performer/pfstats.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfLists.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfList.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfStats.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfState.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfChannel.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfPipeWindow.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfWindow.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfEarthSky.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfLODState.h
vsDecalAttribute.o: /usr/include/Performer/pf/pfScene.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfGeoSet.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfFlux.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfEngine.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfPassList.h
vsDecalAttribute.o: /usr/include/Performer/pr/pfFBState.h vsAttribute.h++
vsDecalAttribute.o: ../../util/common/vsGlobals.h++
vsDecalAttribute.o: ../../util/common/vsObject.h++
vsDecalAttribute.o: ../../util/common/vsTreeMap.h++
vsDecalAttribute.o: ../../util/common/vsGrowableArray.h++
vsDecalAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsDecalAttribute.o: ../../util/common/vsVector.h++
vsDecalAttribute.o: ../../util/common/vsMatrix.h++
vsDecalAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsDynamicGeometry.o: vsDynamicGeometry.h++ /usr/include/Performer/pr/pfFlux.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfMemory.h
vsDynamicGeometry.o: /usr/include/sys/types.h /usr/include/features.h
vsDynamicGeometry.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsDynamicGeometry.o: /usr/include/bits/types.h
vsDynamicGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDynamicGeometry.o: /usr/include/time.h /usr/include/endian.h
vsDynamicGeometry.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsDynamicGeometry.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsDynamicGeometry.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsDynamicGeometry.o: /usr/include/string.h /usr/include/Performer/pr.h
vsDynamicGeometry.o: /usr/include/stdio.h /usr/include/libio.h
vsDynamicGeometry.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDynamicGeometry.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDynamicGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDynamicGeometry.o: /usr/include/bits/stdio_lim.h
vsDynamicGeometry.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDynamicGeometry.o: /usr/include/ulocks.h /usr/include/malloc.h
vsDynamicGeometry.o: /usr/include/limits.h
vsDynamicGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsDynamicGeometry.o: /usr/include/bits/posix1_lim.h
vsDynamicGeometry.o: /usr/include/bits/local_lim.h
vsDynamicGeometry.o: /usr/include/linux/limits.h
vsDynamicGeometry.o: /usr/include/bits/posix2_lim.h
vsDynamicGeometry.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsDynamicGeometry.o: /usr/include/bits/posix_opt.h
vsDynamicGeometry.o: /usr/include/bits/confname.h /usr/include/errno.h
vsDynamicGeometry.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsDynamicGeometry.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsDynamicGeometry.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsDynamicGeometry.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsDynamicGeometry.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsDynamicGeometry.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsDynamicGeometry.o: /usr/include/Performer/opengl.h
vsDynamicGeometry.o: /usr/include/Performer/linuxstubs.h
vsDynamicGeometry.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsDynamicGeometry.o: /usr/include/bits/waitflags.h
vsDynamicGeometry.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsDynamicGeometry.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsDynamicGeometry.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsDynamicGeometry.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsDynamicGeometry.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsDynamicGeometry.o: /usr/include/bits/mathcalls.h
vsDynamicGeometry.o: /usr/include/Performer/prstats.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfType.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfList.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfObject.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfState.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfGeoState.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfLinMath.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfStruct.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfEngine.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfGeode.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfNode.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfUpdatable.h
vsDynamicGeometry.o: /usr/include/Performer/pf.h
vsDynamicGeometry.o: /usr/include/Performer/pfstats.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfBuffer.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfUpdate.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfLists.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfFrameStats.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfPipe.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfVideoChannel.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfCalligraphic.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfStats.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfDispList.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfByteBank.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfGSetBank.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfGeoMath.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfGeoSet.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfPassList.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfFBState.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfWindow.h
vsDynamicGeometry.o: ../../util/common/vsVector.h++
vsDynamicGeometry.o: ../../util/common/vsGlobals.h++ vsAttribute.h++
vsDynamicGeometry.o: ../../util/common/vsObject.h++
vsDynamicGeometry.o: ../../util/common/vsTreeMap.h++
vsDynamicGeometry.o: ../../util/common/vsGrowableArray.h++
vsDynamicGeometry.o: ../../util/common/vsObjectMap.h++ vsGeometry.h++
vsDynamicGeometry.o: vsNode.h++ ../../util/common/vsMatrix.h++
vsDynamicGeometry.o: ../../util/common/vsQuat.h++
vsDynamicGeometry.o: /usr/include/Performer/pf/pfSCS.h
vsDynamicGeometry.o: /usr/include/Performer/pf/pfGroup.h
vsDynamicGeometry.o: /usr/include/Performer/pr/pfLight.h vsComponent.h++
vsDynamicGeometry.o: vsShadingAttribute.h++ vsStateAttribute.h++
vsDynamicGeometry.o: vsGraphicsState.h++ vsBackfaceAttribute.h++
vsDynamicGeometry.o: vsFogAttribute.h++ /usr/include/Performer/pr/pfFog.h
vsDynamicGeometry.o: vsMaterialAttribute.h++
vsDynamicGeometry.o: /usr/include/Performer/pr/pfMaterial.h
vsDynamicGeometry.o: vsTextureAttribute.h++
vsDynamicGeometry.o: /usr/include/Performer/pr/pfTexture.h
vsDynamicGeometry.o: /usr/include/Performer/image.h
vsDynamicGeometry.o: vsTransparencyAttribute.h++ vsWireframeAttribute.h++
vsDynamicGeometry.o: vsLightAttribute.h++
vsDynamicGeometry.o: /usr/include/Performer/pf/pfLightSource.h
vsFogAttribute.o: vsFogAttribute.h++ /usr/include/Performer/pr/pfFog.h
vsFogAttribute.o: /usr/include/Performer/pr/pfObject.h
vsFogAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsFogAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsFogAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsFogAttribute.o: /usr/include/bits/types.h
vsFogAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsFogAttribute.o: /usr/include/time.h /usr/include/endian.h
vsFogAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsFogAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsFogAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsFogAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsFogAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsFogAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsFogAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsFogAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsFogAttribute.o: /usr/include/bits/stdio_lim.h
vsFogAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsFogAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsFogAttribute.o: /usr/include/limits.h
vsFogAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsFogAttribute.o: /usr/include/bits/posix1_lim.h
vsFogAttribute.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsFogAttribute.o: /usr/include/bits/posix2_lim.h
vsFogAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsFogAttribute.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsFogAttribute.o: /usr/include/errno.h /usr/include/bits/errno.h
vsFogAttribute.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsFogAttribute.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsFogAttribute.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsFogAttribute.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsFogAttribute.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsFogAttribute.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsFogAttribute.o: /usr/include/Performer/linuxstubs.h
vsFogAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsFogAttribute.o: /usr/include/bits/waitflags.h
vsFogAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsFogAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsFogAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsFogAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsFogAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsFogAttribute.o: /usr/include/bits/mathcalls.h
vsFogAttribute.o: /usr/include/Performer/prstats.h
vsFogAttribute.o: /usr/include/Performer/pr/pfType.h
vsFogAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsFogAttribute.o: /usr/include/Performer/pr/pfStruct.h vsStateAttribute.h++
vsFogAttribute.o: ../../util/common/vsGlobals.h++
vsFogAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsFogAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsFogAttribute.o: ../../util/common/vsObject.h++
vsFogAttribute.o: ../../util/common/vsTreeMap.h++
vsFogAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsFogAttribute.o: ../../util/common/vsVector.h++
vsFogAttribute.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsFogAttribute.o: vsGraphicsState.h++ vsBackfaceAttribute.h++
vsFogAttribute.o: vsMaterialAttribute.h++
vsFogAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsFogAttribute.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsFogAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsFogAttribute.o: /usr/include/Performer/pr/pfList.h
vsFogAttribute.o: /usr/include/Performer/image.h vsTransparencyAttribute.h++
vsFogAttribute.o: vsWireframeAttribute.h++ vsLightAttribute.h++
vsFogAttribute.o: /usr/include/Performer/pr/pfLight.h
vsFogAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsFogAttribute.o: /usr/include/Performer/pf/pfNode.h
vsFogAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsFogAttribute.o: /usr/include/Performer/pf.h
vsFogAttribute.o: /usr/include/Performer/pfstats.h
vsFogAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsFogAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsFogAttribute.o: /usr/include/Performer/pf/pfLists.h
vsFogAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsFogAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsFogAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsFogAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsFogAttribute.o: /usr/include/Performer/pr/pfStats.h
vsFogAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsFogAttribute.o: /usr/include/Performer/pr/pfState.h
vsFogAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsFogAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsFogAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsGeometry.o: vsGeometry.h++ /usr/include/Performer/pf/pfGeode.h
vsGeometry.o: /usr/include/Performer/pf/pfNode.h
vsGeometry.o: /usr/include/Performer/pf/pfUpdatable.h
vsGeometry.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsGeometry.o: /usr/include/sys/types.h /usr/include/features.h
vsGeometry.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsGeometry.o: /usr/include/bits/types.h
vsGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsGeometry.o: /usr/include/time.h /usr/include/endian.h
vsGeometry.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsGeometry.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsGeometry.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsGeometry.o: /usr/include/stdio.h /usr/include/libio.h
vsGeometry.o: /usr/include/_G_config.h /usr/include/wchar.h
vsGeometry.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsGeometry.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsGeometry.o: /usr/include/getopt.h /usr/include/ulocks.h
vsGeometry.o: /usr/include/malloc.h /usr/include/limits.h
vsGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsGeometry.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsGeometry.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsGeometry.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsGeometry.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsGeometry.o: /usr/include/errno.h /usr/include/bits/errno.h
vsGeometry.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsGeometry.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsGeometry.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsGeometry.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsGeometry.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsGeometry.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsGeometry.o: /usr/include/Performer/linuxstubs.h
vsGeometry.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsGeometry.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsGeometry.o: /usr/include/alloca.h /usr/include/string.h
vsGeometry.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsGeometry.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsGeometry.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsGeometry.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsGeometry.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsGeometry.o: /usr/include/Performer/pr/pfLinMath.h
vsGeometry.o: /usr/include/Performer/pr/pfObject.h
vsGeometry.o: /usr/include/Performer/pr/pfMemory.h
vsGeometry.o: /usr/include/Performer/pr/pfType.h
vsGeometry.o: /usr/include/Performer/pr/pfStruct.h
vsGeometry.o: /usr/include/Performer/pfstats.h
vsGeometry.o: /usr/include/Performer/pf/pfBuffer.h
vsGeometry.o: /usr/include/Performer/pf/pfUpdate.h
vsGeometry.o: /usr/include/Performer/pf/pfLists.h
vsGeometry.o: /usr/include/Performer/pf/pfFrameStats.h
vsGeometry.o: /usr/include/Performer/pf/pfPipe.h
vsGeometry.o: /usr/include/Performer/pr/pfVideoChannel.h
vsGeometry.o: /usr/include/Performer/pr/pfList.h
vsGeometry.o: /usr/include/Performer/pr/pfCalligraphic.h
vsGeometry.o: /usr/include/Performer/pr/pfStats.h
vsGeometry.o: /usr/include/Performer/pr/pfDispList.h
vsGeometry.o: /usr/include/Performer/pr/pfGeoState.h
vsGeometry.o: /usr/include/Performer/pr/pfState.h
vsGeometry.o: /usr/include/Performer/pr/pfByteBank.h
vsGeometry.o: /usr/include/Performer/pr/pfGSetBank.h
vsGeometry.o: /usr/include/Performer/pr/pfGeoMath.h
vsGeometry.o: /usr/include/Performer/pr/pfFlux.h
vsGeometry.o: /usr/include/Performer/pr/pfEngine.h
vsGeometry.o: /usr/include/Performer/pr/pfGeoSet.h
vsGeometry.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsGeometry.o: /usr/include/Performer/pr/pfPassList.h
vsGeometry.o: /usr/include/Performer/pr/pfFBState.h
vsGeometry.o: /usr/include/Performer/pr/pfWindow.h
vsGeometry.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsGeometry.o: vsAttribute.h++ ../../util/common/vsObject.h++
vsGeometry.o: ../../util/common/vsTreeMap.h++
vsGeometry.o: ../../util/common/vsGrowableArray.h++
vsGeometry.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsGeometry.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsGeometry.o: /usr/include/Performer/pf/pfSCS.h
vsGeometry.o: /usr/include/Performer/pf/pfGroup.h
vsGeometry.o: /usr/include/Performer/pr/pfLight.h vsComponent.h++
vsGeometry.o: vsBackfaceAttribute.h++ vsStateAttribute.h++ vsFogAttribute.h++
vsGeometry.o: /usr/include/Performer/pr/pfFog.h vsMaterialAttribute.h++
vsGeometry.o: /usr/include/Performer/pr/pfMaterial.h vsShadingAttribute.h++
vsGeometry.o: vsTextureAttribute.h++ /usr/include/Performer/pr/pfTexture.h
vsGeometry.o: /usr/include/Performer/image.h vsTransparencyAttribute.h++
vsGeometry.o: vsWireframeAttribute.h++ vsGraphicsState.h++
vsGeometry.o: vsLightAttribute.h++ /usr/include/Performer/pf/pfLightSource.h
vsGraphicsState.o: vsGraphicsState.h++ vsBackfaceAttribute.h++
vsGraphicsState.o: vsStateAttribute.h++ ../../util/common/vsGlobals.h++
vsGraphicsState.o: /usr/include/math.h /usr/include/features.h
vsGraphicsState.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsGraphicsState.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsGraphicsState.o: /usr/include/bits/mathcalls.h
vsGraphicsState.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsGraphicsState.o: /usr/include/stdlib.h
vsGraphicsState.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsGraphicsState.o: /usr/include/bits/waitflags.h
vsGraphicsState.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsGraphicsState.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsGraphicsState.o: /usr/include/Performer/pr/pfGeoState.h
vsGraphicsState.o: /usr/include/Performer/pr/pfObject.h
vsGraphicsState.o: /usr/include/Performer/pr/pfMemory.h
vsGraphicsState.o: /usr/include/sys/types.h /usr/include/bits/types.h
vsGraphicsState.o: /usr/include/time.h /usr/include/sys/select.h
vsGraphicsState.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsGraphicsState.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsGraphicsState.o: /usr/include/string.h /usr/include/Performer/pr.h
vsGraphicsState.o: /usr/include/stdio.h /usr/include/libio.h
vsGraphicsState.o: /usr/include/_G_config.h /usr/include/wchar.h
vsGraphicsState.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsGraphicsState.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsGraphicsState.o: /usr/include/bits/stdio_lim.h
vsGraphicsState.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsGraphicsState.o: /usr/include/ulocks.h /usr/include/malloc.h
vsGraphicsState.o: /usr/include/limits.h
vsGraphicsState.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsGraphicsState.o: /usr/include/bits/posix1_lim.h
vsGraphicsState.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsGraphicsState.o: /usr/include/bits/posix2_lim.h
vsGraphicsState.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsGraphicsState.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsGraphicsState.o: /usr/include/errno.h /usr/include/bits/errno.h
vsGraphicsState.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsGraphicsState.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsGraphicsState.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsGraphicsState.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsGraphicsState.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsGraphicsState.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsGraphicsState.o: /usr/include/Performer/linuxstubs.h
vsGraphicsState.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsGraphicsState.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsGraphicsState.o: /usr/include/bits/wordsize.h
vsGraphicsState.o: /usr/include/Performer/prmath.h
vsGraphicsState.o: /usr/include/Performer/prstats.h
vsGraphicsState.o: /usr/include/Performer/pr/pfType.h
vsGraphicsState.o: ../../util/common/vsObject.h++
vsGraphicsState.o: ../../util/common/vsTreeMap.h++
vsGraphicsState.o: ../../util/common/vsObjectMap.h++ vsFogAttribute.h++
vsGraphicsState.o: /usr/include/Performer/pr/pfFog.h
vsGraphicsState.o: /usr/include/Performer/pr/pfLinMath.h
vsGraphicsState.o: /usr/include/Performer/pr/pfStruct.h
vsGraphicsState.o: vsMaterialAttribute.h++
vsGraphicsState.o: /usr/include/Performer/pr/pfMaterial.h
vsGraphicsState.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsGraphicsState.o: /usr/include/Performer/pr/pfTexture.h
vsGraphicsState.o: /usr/include/Performer/pr/pfList.h
vsGraphicsState.o: /usr/include/Performer/image.h vsTransparencyAttribute.h++
vsGraphicsState.o: vsWireframeAttribute.h++ vsLightAttribute.h++
vsGraphicsState.o: /usr/include/Performer/pr/pfLight.h
vsGraphicsState.o: /usr/include/Performer/pf/pfLightSource.h
vsGraphicsState.o: /usr/include/Performer/pf/pfNode.h
vsGraphicsState.o: /usr/include/Performer/pf/pfUpdatable.h
vsGraphicsState.o: /usr/include/Performer/pf.h
vsGraphicsState.o: /usr/include/Performer/pfstats.h
vsGraphicsState.o: /usr/include/Performer/pf/pfBuffer.h
vsGraphicsState.o: /usr/include/Performer/pf/pfUpdate.h
vsGraphicsState.o: /usr/include/Performer/pf/pfLists.h
vsGraphicsState.o: /usr/include/Performer/pf/pfFrameStats.h
vsGraphicsState.o: /usr/include/Performer/pf/pfPipe.h
vsGraphicsState.o: /usr/include/Performer/pr/pfVideoChannel.h
vsGraphicsState.o: /usr/include/Performer/pr/pfCalligraphic.h
vsGraphicsState.o: /usr/include/Performer/pr/pfStats.h
vsGraphicsState.o: /usr/include/Performer/pr/pfDispList.h
vsGraphicsState.o: /usr/include/Performer/pr/pfState.h
vsGraphicsState.o: /usr/include/Performer/pr/pfByteBank.h
vsGraphicsState.o: /usr/include/Performer/pr/pfGSetBank.h
vsGraphicsState.o: /usr/include/Performer/pr/pfGeoMath.h vsNode.h++
vsGraphicsState.o: ../../util/common/vsVector.h++
vsGraphicsState.o: ../../util/common/vsMatrix.h++
vsGraphicsState.o: ../../util/common/vsQuat.h++
vsIntersect.o: vsIntersect.h++ /usr/include/Performer/pr/pfGeoSet.h
vsIntersect.o: /usr/include/Performer/pr/pfGeoMath.h
vsIntersect.o: /usr/include/Performer/pr/pfObject.h
vsIntersect.o: /usr/include/Performer/pr/pfMemory.h /usr/include/sys/types.h
vsIntersect.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsIntersect.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsIntersect.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsIntersect.o: /usr/include/time.h /usr/include/endian.h
vsIntersect.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsIntersect.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsIntersect.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsIntersect.o: /usr/include/string.h /usr/include/Performer/pr.h
vsIntersect.o: /usr/include/stdio.h /usr/include/libio.h
vsIntersect.o: /usr/include/_G_config.h /usr/include/wchar.h
vsIntersect.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsIntersect.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsIntersect.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsIntersect.o: /usr/include/getopt.h /usr/include/ulocks.h
vsIntersect.o: /usr/include/malloc.h /usr/include/limits.h
vsIntersect.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsIntersect.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsIntersect.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsIntersect.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsIntersect.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsIntersect.o: /usr/include/errno.h /usr/include/bits/errno.h
vsIntersect.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsIntersect.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsIntersect.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsIntersect.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsIntersect.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsIntersect.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsIntersect.o: /usr/include/Performer/linuxstubs.h
vsIntersect.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsIntersect.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsIntersect.o: /usr/include/alloca.h /usr/include/sys/prctl.h
vsIntersect.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsIntersect.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsIntersect.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsIntersect.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsIntersect.o: /usr/include/Performer/prstats.h
vsIntersect.o: /usr/include/Performer/pr/pfType.h
vsIntersect.o: /usr/include/Performer/pr/pfLinMath.h
vsIntersect.o: /usr/include/Performer/pr/pfStruct.h
vsIntersect.o: /usr/include/Performer/pr/pfStats.h
vsIntersect.o: /usr/include/Performer/pr/pfDispList.h
vsIntersect.o: /usr/include/Performer/pr/pfGeoState.h
vsIntersect.o: /usr/include/Performer/pr/pfState.h
vsIntersect.o: /usr/include/Performer/pr/pfList.h
vsIntersect.o: /usr/include/Performer/pr/pfByteBank.h
vsIntersect.o: /usr/include/Performer/pr/pfGSetBank.h
vsIntersect.o: /usr/include/Performer/pr/pfVideoChannel.h
vsIntersect.o: /usr/include/Performer/pr/pfCalligraphic.h
vsIntersect.o: /usr/include/Performer/pr/pfFlux.h
vsIntersect.o: /usr/include/Performer/pr/pfEngine.h
vsIntersect.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsIntersect.o: /usr/include/Performer/pr/pfPassList.h
vsIntersect.o: /usr/include/Performer/pr/pfFBState.h
vsIntersect.o: /usr/include/Performer/pr/pfWindow.h
vsIntersect.o: /usr/include/Performer/pf/pfNode.h
vsIntersect.o: /usr/include/Performer/pf/pfUpdatable.h
vsIntersect.o: /usr/include/Performer/pf.h /usr/include/Performer/pfstats.h
vsIntersect.o: /usr/include/Performer/pf/pfBuffer.h
vsIntersect.o: /usr/include/Performer/pf/pfUpdate.h
vsIntersect.o: /usr/include/Performer/pf/pfLists.h
vsIntersect.o: /usr/include/Performer/pf/pfFrameStats.h
vsIntersect.o: /usr/include/Performer/pf/pfPipe.h
vsIntersect.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsIntersect.o: vsComponent.h++ /usr/include/Performer/pf/pfGroup.h
vsIntersect.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsIntersect.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsIntersect.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsIntersect.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsIntersect.o: vsGeometry.h++ /usr/include/Performer/pf/pfGeode.h
vsIntersect.o: ../../graphics/Performer_X/vsPane.h++
vsIntersect.o: /usr/include/Performer/pf/pfChannel.h
vsIntersect.o: /usr/include/Performer/pf/pfPipeWindow.h
vsIntersect.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsIntersect.o: /usr/include/Performer/pf/pfEarthSky.h
vsIntersect.o: /usr/include/Performer/pf/pfLODState.h
vsIntersect.o: /usr/include/Performer/pf/pfScene.h
vsIntersect.o: ../../graphics/Performer_X/vsWindow.h++
vsIntersect.o: ../../graphics/Performer_X/vsScreen.h++
vsIntersect.o: ../../graphics/Performer_X/vsPipe.h++ vsView.h++
vsIntersect.o: vsViewpointAttribute.h++ vsScene.h++ vsLightAttribute.h++
vsIntersect.o: /usr/include/Performer/pr/pfLight.h
vsIntersect.o: /usr/include/Performer/pf/pfLightSource.h
vsIntersect.o: /usr/include/Performer/pf/pfTraverser.h
vsIntersect.o: /usr/include/Performer/pr/pfGenericMatrix.h
vsIntersect.o: /usr/include/Performer/pf/pfSCS.h vsDynamicGeometry.h++
vsLODAttribute.o: vsLODAttribute.h++ /usr/include/Performer/pf/pfLOD.h
vsLODAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsLODAttribute.o: /usr/include/Performer/pf/pfNode.h
vsLODAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsLODAttribute.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsLODAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsLODAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsLODAttribute.o: /usr/include/bits/types.h
vsLODAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsLODAttribute.o: /usr/include/time.h /usr/include/endian.h
vsLODAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsLODAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsLODAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsLODAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsLODAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsLODAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsLODAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsLODAttribute.o: /usr/include/bits/stdio_lim.h
vsLODAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsLODAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsLODAttribute.o: /usr/include/limits.h
vsLODAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsLODAttribute.o: /usr/include/bits/posix1_lim.h
vsLODAttribute.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsLODAttribute.o: /usr/include/bits/posix2_lim.h
vsLODAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsLODAttribute.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsLODAttribute.o: /usr/include/errno.h /usr/include/bits/errno.h
vsLODAttribute.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsLODAttribute.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsLODAttribute.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsLODAttribute.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsLODAttribute.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsLODAttribute.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsLODAttribute.o: /usr/include/Performer/linuxstubs.h
vsLODAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsLODAttribute.o: /usr/include/bits/waitflags.h
vsLODAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsLODAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsLODAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsLODAttribute.o: /usr/include/bits/wordsize.h
vsLODAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsLODAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsLODAttribute.o: /usr/include/bits/mathcalls.h
vsLODAttribute.o: /usr/include/Performer/prstats.h
vsLODAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsLODAttribute.o: /usr/include/Performer/pr/pfObject.h
vsLODAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsLODAttribute.o: /usr/include/Performer/pr/pfType.h
vsLODAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsLODAttribute.o: /usr/include/Performer/pfstats.h
vsLODAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsLODAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsLODAttribute.o: /usr/include/Performer/pf/pfLists.h
vsLODAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsLODAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsLODAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsLODAttribute.o: /usr/include/Performer/pr/pfList.h
vsLODAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsLODAttribute.o: /usr/include/Performer/pr/pfStats.h
vsLODAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsLODAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsLODAttribute.o: /usr/include/Performer/pr/pfState.h
vsLODAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsLODAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsLODAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsLODAttribute.o: /usr/include/Performer/pf/pfLODState.h
vsLODAttribute.o: /usr/include/Performer/pr/pfFlux.h
vsLODAttribute.o: /usr/include/Performer/pr/pfEngine.h
vsLODAttribute.o: /usr/include/Performer/pf/pfTraverser.h
vsLODAttribute.o: /usr/include/Performer/pf/pfChannel.h
vsLODAttribute.o: /usr/include/Performer/pf/pfPipeWindow.h
vsLODAttribute.o: /usr/include/Performer/pr/pfWindow.h
vsLODAttribute.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsLODAttribute.o: /usr/include/Performer/pf/pfEarthSky.h
vsLODAttribute.o: /usr/include/Performer/pf/pfScene.h
vsLODAttribute.o: /usr/include/Performer/pr/pfGeoSet.h
vsLODAttribute.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsLODAttribute.o: /usr/include/Performer/pr/pfPassList.h
vsLODAttribute.o: /usr/include/Performer/pr/pfFBState.h
vsLODAttribute.o: /usr/include/Performer/pr/pfGenericMatrix.h vsAttribute.h++
vsLODAttribute.o: ../../util/common/vsGlobals.h++
vsLODAttribute.o: ../../util/common/vsObject.h++
vsLODAttribute.o: ../../util/common/vsTreeMap.h++
vsLODAttribute.o: ../../util/common/vsGrowableArray.h++
vsLODAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsLODAttribute.o: ../../util/common/vsVector.h++
vsLODAttribute.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsLODAttribute.o: vsComponent.h++
vsLightAttribute.o: vsLightAttribute.h++ /usr/include/Performer/pr/pfLight.h
vsLightAttribute.o: /usr/include/Performer/pr/pfObject.h
vsLightAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsLightAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsLightAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsLightAttribute.o: /usr/include/bits/types.h
vsLightAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsLightAttribute.o: /usr/include/time.h /usr/include/endian.h
vsLightAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsLightAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsLightAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsLightAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsLightAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsLightAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsLightAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsLightAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsLightAttribute.o: /usr/include/bits/stdio_lim.h
vsLightAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsLightAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsLightAttribute.o: /usr/include/limits.h
vsLightAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsLightAttribute.o: /usr/include/bits/posix1_lim.h
vsLightAttribute.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsLightAttribute.o: /usr/include/bits/posix2_lim.h
vsLightAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsLightAttribute.o: /usr/include/bits/posix_opt.h
vsLightAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsLightAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsLightAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsLightAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsLightAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsLightAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsLightAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsLightAttribute.o: /usr/include/Performer/opengl.h
vsLightAttribute.o: /usr/include/Performer/linuxstubs.h
vsLightAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsLightAttribute.o: /usr/include/bits/waitflags.h
vsLightAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsLightAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsLightAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsLightAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsLightAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsLightAttribute.o: /usr/include/bits/mathcalls.h
vsLightAttribute.o: /usr/include/Performer/prstats.h
vsLightAttribute.o: /usr/include/Performer/pr/pfType.h
vsLightAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsLightAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsLightAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsLightAttribute.o: /usr/include/Performer/pf/pfNode.h
vsLightAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsLightAttribute.o: /usr/include/Performer/pf.h
vsLightAttribute.o: /usr/include/Performer/pfstats.h
vsLightAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsLightAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsLightAttribute.o: /usr/include/Performer/pf/pfLists.h
vsLightAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsLightAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsLightAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsLightAttribute.o: /usr/include/Performer/pr/pfList.h
vsLightAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsLightAttribute.o: /usr/include/Performer/pr/pfStats.h
vsLightAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsLightAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsLightAttribute.o: /usr/include/Performer/pr/pfState.h
vsLightAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsLightAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsLightAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsAttribute.h++
vsLightAttribute.o: ../../util/common/vsGlobals.h++
vsLightAttribute.o: ../../util/common/vsObject.h++
vsLightAttribute.o: ../../util/common/vsTreeMap.h++
vsLightAttribute.o: ../../util/common/vsGrowableArray.h++
vsLightAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsLightAttribute.o: ../../util/common/vsVector.h++
vsLightAttribute.o: ../../util/common/vsMatrix.h++
vsLightAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsLightAttribute.o: /usr/include/Performer/pf/pfGroup.h vsScene.h++
vsLightAttribute.o: /usr/include/Performer/pf/pfScene.h
vsLightAttribute.o: /usr/include/Performer/pr/pfGeoSet.h
vsLightAttribute.o: /usr/include/Performer/pr/pfFlux.h
vsLightAttribute.o: /usr/include/Performer/pr/pfEngine.h
vsLightAttribute.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsLightAttribute.o: /usr/include/Performer/pr/pfPassList.h
vsLightAttribute.o: /usr/include/Performer/pr/pfFBState.h
vsLightAttribute.o: /usr/include/Performer/pr/pfWindow.h vsGraphicsState.h++
vsLightAttribute.o: vsBackfaceAttribute.h++ vsStateAttribute.h++
vsLightAttribute.o: vsFogAttribute.h++ /usr/include/Performer/pr/pfFog.h
vsLightAttribute.o: vsMaterialAttribute.h++
vsLightAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsLightAttribute.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsLightAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsLightAttribute.o: /usr/include/Performer/image.h
vsLightAttribute.o: vsTransparencyAttribute.h++ vsWireframeAttribute.h++
vsMaterialAttribute.o: vsMaterialAttribute.h++
vsMaterialAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfObject.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsMaterialAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsMaterialAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsMaterialAttribute.o: /usr/include/bits/types.h
vsMaterialAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsMaterialAttribute.o: /usr/include/time.h /usr/include/endian.h
vsMaterialAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsMaterialAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsMaterialAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsMaterialAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsMaterialAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsMaterialAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsMaterialAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsMaterialAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsMaterialAttribute.o: /usr/include/bits/stdio_lim.h
vsMaterialAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsMaterialAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsMaterialAttribute.o: /usr/include/limits.h
vsMaterialAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsMaterialAttribute.o: /usr/include/bits/posix1_lim.h
vsMaterialAttribute.o: /usr/include/bits/local_lim.h
vsMaterialAttribute.o: /usr/include/linux/limits.h
vsMaterialAttribute.o: /usr/include/bits/posix2_lim.h
vsMaterialAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsMaterialAttribute.o: /usr/include/bits/posix_opt.h
vsMaterialAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsMaterialAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsMaterialAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsMaterialAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsMaterialAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsMaterialAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsMaterialAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsMaterialAttribute.o: /usr/include/Performer/opengl.h
vsMaterialAttribute.o: /usr/include/Performer/linuxstubs.h
vsMaterialAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsMaterialAttribute.o: /usr/include/bits/waitflags.h
vsMaterialAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsMaterialAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsMaterialAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsMaterialAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsMaterialAttribute.o: /usr/include/bits/huge_val.h
vsMaterialAttribute.o: /usr/include/bits/mathdef.h
vsMaterialAttribute.o: /usr/include/bits/mathcalls.h
vsMaterialAttribute.o: /usr/include/Performer/prstats.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfType.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsMaterialAttribute.o: vsStateAttribute.h++ ../../util/common/vsGlobals.h++
vsMaterialAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsMaterialAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsMaterialAttribute.o: ../../util/common/vsObject.h++
vsMaterialAttribute.o: ../../util/common/vsTreeMap.h++
vsMaterialAttribute.o: ../../util/common/vsObjectMap.h++ vsGraphicsState.h++
vsMaterialAttribute.o: vsBackfaceAttribute.h++ vsFogAttribute.h++
vsMaterialAttribute.o: /usr/include/Performer/pr/pfFog.h
vsMaterialAttribute.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsMaterialAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfList.h
vsMaterialAttribute.o: /usr/include/Performer/image.h
vsMaterialAttribute.o: vsTransparencyAttribute.h++ vsWireframeAttribute.h++
vsMaterialAttribute.o: vsLightAttribute.h++
vsMaterialAttribute.o: /usr/include/Performer/pr/pfLight.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfNode.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsMaterialAttribute.o: /usr/include/Performer/pf.h
vsMaterialAttribute.o: /usr/include/Performer/pfstats.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfLists.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsMaterialAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfStats.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfState.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsMaterialAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsNode.h++
vsMaterialAttribute.o: ../../util/common/vsVector.h++
vsMaterialAttribute.o: ../../util/common/vsMatrix.h++
vsMaterialAttribute.o: ../../util/common/vsQuat.h++
vsNode.o: /usr/include/stdio.h /usr/include/features.h
vsNode.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsNode.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsNode.o: /usr/include/bits/types.h /usr/include/libio.h
vsNode.o: /usr/include/_G_config.h /usr/include/wchar.h
vsNode.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsNode.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsNode.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsNode.o: /usr/include/getopt.h vsNode.h++ ../../util/common/vsObject.h++
vsNode.o: ../../util/common/vsTreeMap.h++
vsNode.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsNode.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsNode.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsNode.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsNode.o: /usr/include/Performer/pr/pfGeoState.h
vsNode.o: /usr/include/Performer/pr/pfObject.h
vsNode.o: /usr/include/Performer/pr/pfMemory.h /usr/include/sys/types.h
vsNode.o: /usr/include/time.h /usr/include/sys/select.h
vsNode.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsNode.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsNode.o: /usr/include/string.h /usr/include/Performer/pr.h
vsNode.o: /usr/include/ulocks.h /usr/include/malloc.h /usr/include/limits.h
vsNode.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsNode.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsNode.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsNode.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsNode.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsNode.o: /usr/include/errno.h /usr/include/bits/errno.h
vsNode.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsNode.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsNode.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsNode.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h /usr/include/GL/glx.h
vsNode.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsNode.o: /usr/include/Performer/opengl.h /usr/include/Performer/linuxstubs.h
vsNode.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsNode.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsNode.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsNode.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsNode.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsNode.o: /usr/include/Performer/prstats.h /usr/include/Performer/pr/pfType.h
vsNode.o: ../../util/common/vsGlobals.h++ ../../util/common/vsObjectMap.h++
vsNode.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsNode.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsNode.o: /usr/include/Performer/pf/pfGroup.h
vsNode.o: /usr/include/Performer/pf/pfNode.h
vsNode.o: /usr/include/Performer/pf/pfUpdatable.h /usr/include/Performer/pf.h
vsNode.o: /usr/include/Performer/pr/pfLinMath.h
vsNode.o: /usr/include/Performer/pr/pfStruct.h
vsNode.o: /usr/include/Performer/pfstats.h
vsNode.o: /usr/include/Performer/pf/pfBuffer.h
vsNode.o: /usr/include/Performer/pf/pfUpdate.h
vsNode.o: /usr/include/Performer/pf/pfLists.h
vsNode.o: /usr/include/Performer/pf/pfFrameStats.h
vsNode.o: /usr/include/Performer/pf/pfPipe.h
vsNode.o: /usr/include/Performer/pr/pfVideoChannel.h
vsNode.o: /usr/include/Performer/pr/pfList.h
vsNode.o: /usr/include/Performer/pr/pfCalligraphic.h
vsNode.o: /usr/include/Performer/pr/pfStats.h
vsNode.o: /usr/include/Performer/pr/pfDispList.h
vsNode.o: /usr/include/Performer/pr/pfState.h
vsNode.o: /usr/include/Performer/pr/pfByteBank.h
vsNode.o: /usr/include/Performer/pr/pfGSetBank.h
vsNode.o: /usr/include/Performer/pr/pfGeoMath.h vsBackfaceAttribute.h++
vsNode.o: vsStateAttribute.h++ vsFogAttribute.h++
vsNode.o: /usr/include/Performer/pr/pfFog.h vsMaterialAttribute.h++
vsNode.o: /usr/include/Performer/pr/pfMaterial.h vsTextureAttribute.h++
vsNode.o: /usr/include/Performer/pr/pfTexture.h
vsNode.o: /usr/include/Performer/image.h vsTransparencyAttribute.h++
vsNode.o: vsShadingAttribute.h++ vsWireframeAttribute.h++
vsScene.o: vsScene.h++ /usr/include/Performer/pf/pfScene.h
vsScene.o: /usr/include/Performer/pf/pfGroup.h
vsScene.o: /usr/include/Performer/pf/pfNode.h
vsScene.o: /usr/include/Performer/pf/pfUpdatable.h
vsScene.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsScene.o: /usr/include/sys/types.h /usr/include/features.h
vsScene.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsScene.o: /usr/include/bits/types.h
vsScene.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsScene.o: /usr/include/time.h /usr/include/endian.h
vsScene.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsScene.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsScene.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsScene.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
vsScene.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsScene.o: /usr/include/gconv.h
vsScene.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsScene.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsScene.o: /usr/include/getopt.h /usr/include/ulocks.h /usr/include/malloc.h
vsScene.o: /usr/include/limits.h
vsScene.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsScene.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsScene.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsScene.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsScene.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsScene.o: /usr/include/errno.h /usr/include/bits/errno.h
vsScene.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsScene.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsScene.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsScene.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsScene.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsScene.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsScene.o: /usr/include/Performer/linuxstubs.h /usr/include/Performer/image.h
vsScene.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsScene.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsScene.o: /usr/include/string.h /usr/include/sys/prctl.h
vsScene.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsScene.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsScene.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsScene.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsScene.o: /usr/include/Performer/prstats.h
vsScene.o: /usr/include/Performer/pr/pfLinMath.h
vsScene.o: /usr/include/Performer/pr/pfObject.h
vsScene.o: /usr/include/Performer/pr/pfMemory.h
vsScene.o: /usr/include/Performer/pr/pfType.h
vsScene.o: /usr/include/Performer/pr/pfStruct.h
vsScene.o: /usr/include/Performer/pfstats.h
vsScene.o: /usr/include/Performer/pf/pfBuffer.h
vsScene.o: /usr/include/Performer/pf/pfUpdate.h
vsScene.o: /usr/include/Performer/pf/pfLists.h
vsScene.o: /usr/include/Performer/pf/pfFrameStats.h
vsScene.o: /usr/include/Performer/pf/pfPipe.h
vsScene.o: /usr/include/Performer/pr/pfVideoChannel.h
vsScene.o: /usr/include/Performer/pr/pfList.h
vsScene.o: /usr/include/Performer/pr/pfCalligraphic.h
vsScene.o: /usr/include/Performer/pr/pfStats.h
vsScene.o: /usr/include/Performer/pr/pfDispList.h
vsScene.o: /usr/include/Performer/pr/pfGeoState.h
vsScene.o: /usr/include/Performer/pr/pfState.h
vsScene.o: /usr/include/Performer/pr/pfByteBank.h
vsScene.o: /usr/include/Performer/pr/pfGSetBank.h
vsScene.o: /usr/include/Performer/pr/pfGeoMath.h
vsScene.o: /usr/include/Performer/pr/pfGeoSet.h
vsScene.o: /usr/include/Performer/pr/pfFlux.h
vsScene.o: /usr/include/Performer/pr/pfEngine.h
vsScene.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsScene.o: /usr/include/Performer/pr/pfPassList.h
vsScene.o: /usr/include/Performer/pr/pfFBState.h
vsScene.o: /usr/include/Performer/pr/pfWindow.h vsNode.h++
vsScene.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsScene.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsScene.o: ../../util/common/vsGlobals.h++ ../../util/common/vsObjectMap.h++
vsScene.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsScene.o: ../../util/common/vsQuat.h++ vsLightAttribute.h++
vsScene.o: /usr/include/Performer/pr/pfLight.h
vsScene.o: /usr/include/Performer/pf/pfLightSource.h vsComponent.h++
vsScene.o: vsGeometry.h++ /usr/include/Performer/pf/pfGeode.h
vsScene.o: vsDynamicGeometry.h++
vsSequenceAttribute.o: vsSequenceAttribute.h++
vsSequenceAttribute.o: /usr/include/Performer/pf/pfSequence.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfNode.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsSequenceAttribute.o: /usr/include/Performer/pf.h
vsSequenceAttribute.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsSequenceAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsSequenceAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsSequenceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSequenceAttribute.o: /usr/include/time.h /usr/include/endian.h
vsSequenceAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsSequenceAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsSequenceAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsSequenceAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsSequenceAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSequenceAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSequenceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSequenceAttribute.o: /usr/include/bits/stdio_lim.h
vsSequenceAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSequenceAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsSequenceAttribute.o: /usr/include/limits.h
vsSequenceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSequenceAttribute.o: /usr/include/bits/posix1_lim.h
vsSequenceAttribute.o: /usr/include/bits/local_lim.h
vsSequenceAttribute.o: /usr/include/linux/limits.h
vsSequenceAttribute.o: /usr/include/bits/posix2_lim.h
vsSequenceAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsSequenceAttribute.o: /usr/include/bits/posix_opt.h
vsSequenceAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsSequenceAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsSequenceAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsSequenceAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsSequenceAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsSequenceAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsSequenceAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsSequenceAttribute.o: /usr/include/Performer/opengl.h
vsSequenceAttribute.o: /usr/include/Performer/linuxstubs.h
vsSequenceAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsSequenceAttribute.o: /usr/include/bits/waitflags.h
vsSequenceAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsSequenceAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsSequenceAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsSequenceAttribute.o: /usr/include/bits/wordsize.h
vsSequenceAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsSequenceAttribute.o: /usr/include/bits/huge_val.h
vsSequenceAttribute.o: /usr/include/bits/mathdef.h
vsSequenceAttribute.o: /usr/include/bits/mathcalls.h
vsSequenceAttribute.o: /usr/include/Performer/prstats.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfObject.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfType.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsSequenceAttribute.o: /usr/include/Performer/pfstats.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfLists.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsSequenceAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfList.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfStats.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfState.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsSequenceAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsAttribute.h++
vsSequenceAttribute.o: ../../util/common/vsGlobals.h++
vsSequenceAttribute.o: ../../util/common/vsObject.h++
vsSequenceAttribute.o: ../../util/common/vsTreeMap.h++
vsSequenceAttribute.o: ../../util/common/vsGrowableArray.h++
vsSequenceAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsSequenceAttribute.o: ../../util/common/vsVector.h++
vsSequenceAttribute.o: ../../util/common/vsMatrix.h++
vsSequenceAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsShadingAttribute.o: vsShadingAttribute.h++ vsStateAttribute.h++
vsShadingAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsShadingAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsShadingAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsShadingAttribute.o: /usr/include/bits/mathdef.h
vsShadingAttribute.o: /usr/include/bits/mathcalls.h
vsShadingAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsShadingAttribute.o: /usr/include/stdlib.h
vsShadingAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsShadingAttribute.o: /usr/include/bits/waitflags.h
vsShadingAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsShadingAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfObject.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsShadingAttribute.o: /usr/include/sys/types.h /usr/include/bits/types.h
vsShadingAttribute.o: /usr/include/time.h /usr/include/sys/select.h
vsShadingAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsShadingAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsShadingAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsShadingAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsShadingAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsShadingAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsShadingAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsShadingAttribute.o: /usr/include/bits/stdio_lim.h
vsShadingAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsShadingAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsShadingAttribute.o: /usr/include/limits.h
vsShadingAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsShadingAttribute.o: /usr/include/bits/posix1_lim.h
vsShadingAttribute.o: /usr/include/bits/local_lim.h
vsShadingAttribute.o: /usr/include/linux/limits.h
vsShadingAttribute.o: /usr/include/bits/posix2_lim.h
vsShadingAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsShadingAttribute.o: /usr/include/bits/posix_opt.h
vsShadingAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsShadingAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsShadingAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsShadingAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsShadingAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsShadingAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsShadingAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsShadingAttribute.o: /usr/include/Performer/opengl.h
vsShadingAttribute.o: /usr/include/Performer/linuxstubs.h
vsShadingAttribute.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsShadingAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsShadingAttribute.o: /usr/include/bits/wordsize.h
vsShadingAttribute.o: /usr/include/Performer/prmath.h
vsShadingAttribute.o: /usr/include/Performer/prstats.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfType.h
vsShadingAttribute.o: ../../util/common/vsObject.h++
vsShadingAttribute.o: ../../util/common/vsTreeMap.h++
vsShadingAttribute.o: ../../util/common/vsObjectMap.h++ vsGraphicsState.h++
vsShadingAttribute.o: vsBackfaceAttribute.h++ vsFogAttribute.h++
vsShadingAttribute.o: /usr/include/Performer/pr/pfFog.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsShadingAttribute.o: vsMaterialAttribute.h++
vsShadingAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsShadingAttribute.o: vsTextureAttribute.h++
vsShadingAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfList.h
vsShadingAttribute.o: /usr/include/Performer/image.h
vsShadingAttribute.o: vsTransparencyAttribute.h++ vsWireframeAttribute.h++
vsShadingAttribute.o: vsLightAttribute.h++
vsShadingAttribute.o: /usr/include/Performer/pr/pfLight.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfNode.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsShadingAttribute.o: /usr/include/Performer/pf.h
vsShadingAttribute.o: /usr/include/Performer/pfstats.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfLists.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsShadingAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfStats.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfState.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsShadingAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsNode.h++
vsShadingAttribute.o: ../../util/common/vsVector.h++
vsShadingAttribute.o: ../../util/common/vsMatrix.h++
vsShadingAttribute.o: ../../util/common/vsQuat.h++
vsStateAttribute.o: vsNode.h++ ../../util/common/vsObject.h++
vsStateAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsStateAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsStateAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsStateAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsStateAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsStateAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsStateAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsStateAttribute.o: /usr/include/bits/stdio_lim.h
vsStateAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsStateAttribute.o: ../../util/common/vsTreeMap.h++
vsStateAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsStateAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsStateAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsStateAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsStateAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsStateAttribute.o: /usr/include/Performer/pr/pfObject.h
vsStateAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsStateAttribute.o: /usr/include/sys/types.h /usr/include/time.h
vsStateAttribute.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsStateAttribute.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsStateAttribute.o: /usr/include/sys/sysmacros.h /usr/include/string.h
vsStateAttribute.o: /usr/include/Performer/pr.h /usr/include/ulocks.h
vsStateAttribute.o: /usr/include/malloc.h /usr/include/limits.h
vsStateAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsStateAttribute.o: /usr/include/bits/posix1_lim.h
vsStateAttribute.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsStateAttribute.o: /usr/include/bits/posix2_lim.h
vsStateAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsStateAttribute.o: /usr/include/bits/posix_opt.h
vsStateAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsStateAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsStateAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsStateAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsStateAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsStateAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsStateAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsStateAttribute.o: /usr/include/Performer/opengl.h
vsStateAttribute.o: /usr/include/Performer/linuxstubs.h
vsStateAttribute.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsStateAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsStateAttribute.o: /usr/include/bits/wordsize.h
vsStateAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsStateAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsStateAttribute.o: /usr/include/bits/mathcalls.h
vsStateAttribute.o: /usr/include/Performer/prstats.h
vsStateAttribute.o: /usr/include/Performer/pr/pfType.h
vsStateAttribute.o: ../../util/common/vsGlobals.h++
vsStateAttribute.o: ../../util/common/vsObjectMap.h++
vsStateAttribute.o: ../../util/common/vsVector.h++
vsStateAttribute.o: ../../util/common/vsMatrix.h++
vsStateAttribute.o: ../../util/common/vsQuat.h++ vsStateAttribute.h++
vsSwitchAttribute.o: vsSwitchAttribute.h++
vsSwitchAttribute.o: /usr/include/Performer/pf/pfSwitch.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfNode.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsSwitchAttribute.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsSwitchAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsSwitchAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSwitchAttribute.o: /usr/include/bits/types.h
vsSwitchAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSwitchAttribute.o: /usr/include/time.h /usr/include/endian.h
vsSwitchAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsSwitchAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsSwitchAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsSwitchAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsSwitchAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSwitchAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSwitchAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSwitchAttribute.o: /usr/include/bits/stdio_lim.h
vsSwitchAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSwitchAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsSwitchAttribute.o: /usr/include/limits.h
vsSwitchAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSwitchAttribute.o: /usr/include/bits/posix1_lim.h
vsSwitchAttribute.o: /usr/include/bits/local_lim.h
vsSwitchAttribute.o: /usr/include/linux/limits.h
vsSwitchAttribute.o: /usr/include/bits/posix2_lim.h
vsSwitchAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsSwitchAttribute.o: /usr/include/bits/posix_opt.h
vsSwitchAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsSwitchAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsSwitchAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsSwitchAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsSwitchAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsSwitchAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsSwitchAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsSwitchAttribute.o: /usr/include/Performer/opengl.h
vsSwitchAttribute.o: /usr/include/Performer/linuxstubs.h
vsSwitchAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsSwitchAttribute.o: /usr/include/bits/waitflags.h
vsSwitchAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsSwitchAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsSwitchAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsSwitchAttribute.o: /usr/include/bits/wordsize.h
vsSwitchAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsSwitchAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsSwitchAttribute.o: /usr/include/bits/mathcalls.h
vsSwitchAttribute.o: /usr/include/Performer/prstats.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfObject.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfType.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsSwitchAttribute.o: /usr/include/Performer/pfstats.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfLists.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsSwitchAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfList.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfStats.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfState.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfFlux.h
vsSwitchAttribute.o: /usr/include/Performer/pr/pfEngine.h vsAttribute.h++
vsSwitchAttribute.o: ../../util/common/vsGlobals.h++
vsSwitchAttribute.o: ../../util/common/vsObject.h++
vsSwitchAttribute.o: ../../util/common/vsTreeMap.h++
vsSwitchAttribute.o: ../../util/common/vsGrowableArray.h++
vsSwitchAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsSwitchAttribute.o: ../../util/common/vsVector.h++
vsSwitchAttribute.o: ../../util/common/vsMatrix.h++
vsSwitchAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsTextureAttribute.o: vsTextureAttribute.h++
vsTextureAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfObject.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsTextureAttribute.o: /usr/include/sys/types.h /usr/include/features.h
vsTextureAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTextureAttribute.o: /usr/include/bits/types.h
vsTextureAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTextureAttribute.o: /usr/include/time.h /usr/include/endian.h
vsTextureAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsTextureAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsTextureAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsTextureAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsTextureAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsTextureAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTextureAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTextureAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTextureAttribute.o: /usr/include/bits/stdio_lim.h
vsTextureAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTextureAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsTextureAttribute.o: /usr/include/limits.h
vsTextureAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTextureAttribute.o: /usr/include/bits/posix1_lim.h
vsTextureAttribute.o: /usr/include/bits/local_lim.h
vsTextureAttribute.o: /usr/include/linux/limits.h
vsTextureAttribute.o: /usr/include/bits/posix2_lim.h
vsTextureAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsTextureAttribute.o: /usr/include/bits/posix_opt.h
vsTextureAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsTextureAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsTextureAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsTextureAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsTextureAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsTextureAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsTextureAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsTextureAttribute.o: /usr/include/Performer/opengl.h
vsTextureAttribute.o: /usr/include/Performer/linuxstubs.h
vsTextureAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsTextureAttribute.o: /usr/include/bits/waitflags.h
vsTextureAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsTextureAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsTextureAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsTextureAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsTextureAttribute.o: /usr/include/bits/huge_val.h
vsTextureAttribute.o: /usr/include/bits/mathdef.h
vsTextureAttribute.o: /usr/include/bits/mathcalls.h
vsTextureAttribute.o: /usr/include/Performer/prstats.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfType.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfList.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsTextureAttribute.o: /usr/include/Performer/image.h vsStateAttribute.h++
vsTextureAttribute.o: ../../util/common/vsGlobals.h++
vsTextureAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsTextureAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsTextureAttribute.o: ../../util/common/vsObject.h++
vsTextureAttribute.o: ../../util/common/vsTreeMap.h++
vsTextureAttribute.o: ../../util/common/vsObjectMap.h++ vsGraphicsState.h++
vsTextureAttribute.o: vsBackfaceAttribute.h++ vsFogAttribute.h++
vsTextureAttribute.o: /usr/include/Performer/pr/pfFog.h
vsTextureAttribute.o: vsMaterialAttribute.h++
vsTextureAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsTextureAttribute.o: vsShadingAttribute.h++ vsTransparencyAttribute.h++
vsTextureAttribute.o: vsWireframeAttribute.h++ vsLightAttribute.h++
vsTextureAttribute.o: /usr/include/Performer/pr/pfLight.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfNode.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsTextureAttribute.o: /usr/include/Performer/pf.h
vsTextureAttribute.o: /usr/include/Performer/pfstats.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfLists.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsTextureAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfStats.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfState.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsTextureAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsNode.h++
vsTextureAttribute.o: ../../util/common/vsVector.h++
vsTextureAttribute.o: ../../util/common/vsMatrix.h++
vsTextureAttribute.o: ../../util/common/vsQuat.h++
vsTransformAttribute.o: vsTransformAttribute.h++
vsTransformAttribute.o: /usr/include/Performer/pf/pfDCS.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfSCS.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfNode.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsTransformAttribute.o: /usr/include/Performer/pf.h
vsTransformAttribute.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsTransformAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsTransformAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsTransformAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTransformAttribute.o: /usr/include/time.h /usr/include/endian.h
vsTransformAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsTransformAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsTransformAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsTransformAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsTransformAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTransformAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTransformAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTransformAttribute.o: /usr/include/bits/stdio_lim.h
vsTransformAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTransformAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsTransformAttribute.o: /usr/include/limits.h
vsTransformAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTransformAttribute.o: /usr/include/bits/posix1_lim.h
vsTransformAttribute.o: /usr/include/bits/local_lim.h
vsTransformAttribute.o: /usr/include/linux/limits.h
vsTransformAttribute.o: /usr/include/bits/posix2_lim.h
vsTransformAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsTransformAttribute.o: /usr/include/bits/posix_opt.h
vsTransformAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsTransformAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsTransformAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsTransformAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsTransformAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsTransformAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsTransformAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsTransformAttribute.o: /usr/include/Performer/opengl.h
vsTransformAttribute.o: /usr/include/Performer/linuxstubs.h
vsTransformAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsTransformAttribute.o: /usr/include/bits/waitflags.h
vsTransformAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsTransformAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsTransformAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsTransformAttribute.o: /usr/include/bits/wordsize.h
vsTransformAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsTransformAttribute.o: /usr/include/bits/huge_val.h
vsTransformAttribute.o: /usr/include/bits/mathdef.h
vsTransformAttribute.o: /usr/include/bits/mathcalls.h
vsTransformAttribute.o: /usr/include/Performer/prstats.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfObject.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfType.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsTransformAttribute.o: /usr/include/Performer/pfstats.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfLists.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsTransformAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfList.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfStats.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfState.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsTransformAttribute.o: ../../util/common/vsMatrix.h++
vsTransformAttribute.o: ../../util/common/vsGlobals.h++
vsTransformAttribute.o: ../../util/common/vsVector.h++
vsTransformAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsTransformAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsTransformAttribute.o: ../../util/common/vsObject.h++
vsTransformAttribute.o: ../../util/common/vsTreeMap.h++
vsTransformAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsTransformAttribute.o: vsDatabaseLoader.h++
vsTransformAttribute.o: /usr/include/Performer/pfdb/pfflt.h vsGeometry.h++
vsTransformAttribute.o: /usr/include/Performer/pf/pfGeode.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfFlux.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfEngine.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfGeoSet.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfPassList.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfFBState.h
vsTransformAttribute.o: /usr/include/Performer/pr/pfWindow.h
vsTransparencyAttribute.o: vsTransparencyAttribute.h++ vsStateAttribute.h++
vsTransparencyAttribute.o: ../../util/common/vsGlobals.h++
vsTransparencyAttribute.o: /usr/include/math.h /usr/include/features.h
vsTransparencyAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTransparencyAttribute.o: /usr/include/bits/huge_val.h
vsTransparencyAttribute.o: /usr/include/bits/mathdef.h
vsTransparencyAttribute.o: /usr/include/bits/mathcalls.h
vsTransparencyAttribute.o: ../../util/common/vsGrowableArray.h++
vsTransparencyAttribute.o: vsAttribute.h++ /usr/include/stdlib.h
vsTransparencyAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTransparencyAttribute.o: /usr/include/bits/waitflags.h
vsTransparencyAttribute.o: /usr/include/bits/waitstatus.h
vsTransparencyAttribute.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTransparencyAttribute.o: /usr/include/alloca.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfObject.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsTransparencyAttribute.o: /usr/include/sys/types.h /usr/include/bits/types.h
vsTransparencyAttribute.o: /usr/include/time.h /usr/include/sys/select.h
vsTransparencyAttribute.o: /usr/include/bits/select.h
vsTransparencyAttribute.o: /usr/include/bits/sigset.h
vsTransparencyAttribute.o: /usr/include/bits/time.h
vsTransparencyAttribute.o: /usr/include/sys/sysmacros.h /usr/include/string.h
vsTransparencyAttribute.o: /usr/include/Performer/pr.h /usr/include/stdio.h
vsTransparencyAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsTransparencyAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsTransparencyAttribute.o: /usr/include/gconv.h
vsTransparencyAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTransparencyAttribute.o: /usr/include/bits/stdio_lim.h
vsTransparencyAttribute.o: /usr/include/bits/sys_errlist.h
vsTransparencyAttribute.o: /usr/include/getopt.h /usr/include/ulocks.h
vsTransparencyAttribute.o: /usr/include/malloc.h /usr/include/limits.h
vsTransparencyAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTransparencyAttribute.o: /usr/include/bits/posix1_lim.h
vsTransparencyAttribute.o: /usr/include/bits/local_lim.h
vsTransparencyAttribute.o: /usr/include/linux/limits.h
vsTransparencyAttribute.o: /usr/include/bits/posix2_lim.h
vsTransparencyAttribute.o: /usr/include/bits/xopen_lim.h
vsTransparencyAttribute.o: /usr/include/unistd.h
vsTransparencyAttribute.o: /usr/include/bits/posix_opt.h
vsTransparencyAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsTransparencyAttribute.o: /usr/include/bits/errno.h
vsTransparencyAttribute.o: /usr/include/linux/errno.h
vsTransparencyAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsTransparencyAttribute.o: /usr/include/X11/Xlib.h
vsTransparencyAttribute.o: /usr/include/X11/Xfuncproto.h
vsTransparencyAttribute.o: /usr/include/X11/Xosdefs.h
vsTransparencyAttribute.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsTransparencyAttribute.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsTransparencyAttribute.o: /usr/include/GL/glu.h
vsTransparencyAttribute.o: /usr/include/Performer/opengl.h
vsTransparencyAttribute.o: /usr/include/Performer/linuxstubs.h
vsTransparencyAttribute.o: /usr/include/Performer/image.h
vsTransparencyAttribute.o: /usr/include/sys/prctl.h
vsTransparencyAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsTransparencyAttribute.o: /usr/include/bits/wordsize.h
vsTransparencyAttribute.o: /usr/include/Performer/prmath.h
vsTransparencyAttribute.o: /usr/include/Performer/prstats.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfType.h
vsTransparencyAttribute.o: ../../util/common/vsObject.h++
vsTransparencyAttribute.o: ../../util/common/vsTreeMap.h++
vsTransparencyAttribute.o: ../../util/common/vsObjectMap.h++
vsTransparencyAttribute.o: vsGraphicsState.h++ vsBackfaceAttribute.h++
vsTransparencyAttribute.o: vsFogAttribute.h++
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfFog.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsTransparencyAttribute.o: vsMaterialAttribute.h++
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsTransparencyAttribute.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfList.h
vsTransparencyAttribute.o: /usr/include/Performer/image.h
vsTransparencyAttribute.o: vsWireframeAttribute.h++ vsLightAttribute.h++
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfLight.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfNode.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsTransparencyAttribute.o: /usr/include/Performer/pf.h
vsTransparencyAttribute.o: /usr/include/Performer/pfstats.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfLists.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsTransparencyAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfStats.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfState.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsTransparencyAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsNode.h++
vsTransparencyAttribute.o: ../../util/common/vsVector.h++
vsTransparencyAttribute.o: ../../util/common/vsMatrix.h++
vsTransparencyAttribute.o: ../../util/common/vsQuat.h++
vsView.o: vsView.h++ /usr/include/math.h /usr/include/features.h
vsView.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsView.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsView.o: /usr/include/bits/mathcalls.h ../../util/common/vsVector.h++
vsView.o: /usr/include/stdio.h
vsView.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsView.o: /usr/include/bits/types.h /usr/include/libio.h
vsView.o: /usr/include/_G_config.h /usr/include/wchar.h
vsView.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsView.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsView.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsView.o: /usr/include/getopt.h ../../util/common/vsGlobals.h++
vsView.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsView.o: vsViewpointAttribute.h++ vsComponent.h++
vsView.o: /usr/include/Performer/pf/pfGroup.h
vsView.o: /usr/include/Performer/pf/pfNode.h
vsView.o: /usr/include/Performer/pf/pfUpdatable.h /usr/include/Performer/pf.h
vsView.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsView.o: /usr/include/time.h /usr/include/endian.h
vsView.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsView.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsView.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsView.o: /usr/include/ulocks.h /usr/include/malloc.h /usr/include/limits.h
vsView.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsView.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsView.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsView.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsView.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsView.o: /usr/include/errno.h /usr/include/bits/errno.h
vsView.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsView.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsView.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsView.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h /usr/include/GL/glx.h
vsView.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsView.o: /usr/include/Performer/opengl.h /usr/include/Performer/linuxstubs.h
vsView.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsView.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsView.o: /usr/include/alloca.h /usr/include/string.h
vsView.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsView.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsView.o: /usr/include/Performer/prmath.h /usr/include/Performer/prstats.h
vsView.o: /usr/include/Performer/pr/pfLinMath.h
vsView.o: /usr/include/Performer/pr/pfObject.h
vsView.o: /usr/include/Performer/pr/pfMemory.h
vsView.o: /usr/include/Performer/pr/pfType.h
vsView.o: /usr/include/Performer/pr/pfStruct.h
vsView.o: /usr/include/Performer/pfstats.h
vsView.o: /usr/include/Performer/pf/pfBuffer.h
vsView.o: /usr/include/Performer/pf/pfUpdate.h
vsView.o: /usr/include/Performer/pf/pfLists.h
vsView.o: /usr/include/Performer/pf/pfFrameStats.h
vsView.o: /usr/include/Performer/pf/pfPipe.h
vsView.o: /usr/include/Performer/pr/pfVideoChannel.h
vsView.o: /usr/include/Performer/pr/pfList.h
vsView.o: /usr/include/Performer/pr/pfCalligraphic.h
vsView.o: /usr/include/Performer/pr/pfStats.h
vsView.o: /usr/include/Performer/pr/pfDispList.h
vsView.o: /usr/include/Performer/pr/pfGeoState.h
vsView.o: /usr/include/Performer/pr/pfState.h
vsView.o: /usr/include/Performer/pr/pfByteBank.h
vsView.o: /usr/include/Performer/pr/pfGSetBank.h
vsView.o: /usr/include/Performer/pr/pfGeoMath.h
vsView.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsView.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsView.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsViewpointAttribute.o: vsViewpointAttribute.h++ vsComponent.h++
vsViewpointAttribute.o: /usr/include/Performer/pf/pfGroup.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfNode.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsViewpointAttribute.o: /usr/include/Performer/pf.h
vsViewpointAttribute.o: /usr/include/Performer/pr.h /usr/include/sys/types.h
vsViewpointAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsViewpointAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
vsViewpointAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsViewpointAttribute.o: /usr/include/time.h /usr/include/endian.h
vsViewpointAttribute.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsViewpointAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsViewpointAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsViewpointAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsViewpointAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsViewpointAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsViewpointAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsViewpointAttribute.o: /usr/include/bits/stdio_lim.h
vsViewpointAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsViewpointAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsViewpointAttribute.o: /usr/include/limits.h
vsViewpointAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsViewpointAttribute.o: /usr/include/bits/posix1_lim.h
vsViewpointAttribute.o: /usr/include/bits/local_lim.h
vsViewpointAttribute.o: /usr/include/linux/limits.h
vsViewpointAttribute.o: /usr/include/bits/posix2_lim.h
vsViewpointAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsViewpointAttribute.o: /usr/include/bits/posix_opt.h
vsViewpointAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsViewpointAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsViewpointAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsViewpointAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsViewpointAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsViewpointAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsViewpointAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsViewpointAttribute.o: /usr/include/Performer/opengl.h
vsViewpointAttribute.o: /usr/include/Performer/linuxstubs.h
vsViewpointAttribute.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsViewpointAttribute.o: /usr/include/bits/waitflags.h
vsViewpointAttribute.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsViewpointAttribute.o: /usr/include/string.h /usr/include/sys/prctl.h
vsViewpointAttribute.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsViewpointAttribute.o: /usr/include/bits/wordsize.h
vsViewpointAttribute.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsViewpointAttribute.o: /usr/include/bits/huge_val.h
vsViewpointAttribute.o: /usr/include/bits/mathdef.h
vsViewpointAttribute.o: /usr/include/bits/mathcalls.h
vsViewpointAttribute.o: /usr/include/Performer/prstats.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfObject.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfType.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsViewpointAttribute.o: /usr/include/Performer/pfstats.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfLists.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsViewpointAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfList.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfStats.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfState.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsViewpointAttribute.o: /usr/include/Performer/pr/pfGeoMath.h
vsViewpointAttribute.o: ../../util/common/vsGrowableArray.h++
vsViewpointAttribute.o: ../../util/common/vsVector.h++
vsViewpointAttribute.o: ../../util/common/vsGlobals.h++ vsAttribute.h++
vsViewpointAttribute.o: ../../util/common/vsObject.h++
vsViewpointAttribute.o: ../../util/common/vsTreeMap.h++
vsViewpointAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsViewpointAttribute.o: ../../util/common/vsMatrix.h++
vsViewpointAttribute.o: ../../util/common/vsQuat.h++ vsView.h++
vsWireframeAttribute.o: vsWireframeAttribute.h++ vsStateAttribute.h++
vsWireframeAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWireframeAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsWireframeAttribute.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsWireframeAttribute.o: /usr/include/bits/mathdef.h
vsWireframeAttribute.o: /usr/include/bits/mathcalls.h
vsWireframeAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsWireframeAttribute.o: /usr/include/stdlib.h
vsWireframeAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWireframeAttribute.o: /usr/include/bits/waitflags.h
vsWireframeAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsWireframeAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfGeoState.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfObject.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfMemory.h
vsWireframeAttribute.o: /usr/include/sys/types.h /usr/include/bits/types.h
vsWireframeAttribute.o: /usr/include/time.h /usr/include/sys/select.h
vsWireframeAttribute.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsWireframeAttribute.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsWireframeAttribute.o: /usr/include/string.h /usr/include/Performer/pr.h
vsWireframeAttribute.o: /usr/include/stdio.h /usr/include/libio.h
vsWireframeAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWireframeAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWireframeAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWireframeAttribute.o: /usr/include/bits/stdio_lim.h
vsWireframeAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWireframeAttribute.o: /usr/include/ulocks.h /usr/include/malloc.h
vsWireframeAttribute.o: /usr/include/limits.h
vsWireframeAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsWireframeAttribute.o: /usr/include/bits/posix1_lim.h
vsWireframeAttribute.o: /usr/include/bits/local_lim.h
vsWireframeAttribute.o: /usr/include/linux/limits.h
vsWireframeAttribute.o: /usr/include/bits/posix2_lim.h
vsWireframeAttribute.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsWireframeAttribute.o: /usr/include/bits/posix_opt.h
vsWireframeAttribute.o: /usr/include/bits/confname.h /usr/include/errno.h
vsWireframeAttribute.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsWireframeAttribute.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsWireframeAttribute.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsWireframeAttribute.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsWireframeAttribute.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsWireframeAttribute.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsWireframeAttribute.o: /usr/include/Performer/opengl.h
vsWireframeAttribute.o: /usr/include/Performer/linuxstubs.h
vsWireframeAttribute.o: /usr/include/Performer/image.h
vsWireframeAttribute.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsWireframeAttribute.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsWireframeAttribute.o: /usr/include/Performer/prmath.h
vsWireframeAttribute.o: /usr/include/Performer/prstats.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfType.h
vsWireframeAttribute.o: ../../util/common/vsObject.h++
vsWireframeAttribute.o: ../../util/common/vsTreeMap.h++
vsWireframeAttribute.o: ../../util/common/vsObjectMap.h++ vsGraphicsState.h++
vsWireframeAttribute.o: vsBackfaceAttribute.h++ vsFogAttribute.h++
vsWireframeAttribute.o: /usr/include/Performer/pr/pfFog.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfLinMath.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfStruct.h
vsWireframeAttribute.o: vsMaterialAttribute.h++
vsWireframeAttribute.o: /usr/include/Performer/pr/pfMaterial.h
vsWireframeAttribute.o: vsShadingAttribute.h++ vsTextureAttribute.h++
vsWireframeAttribute.o: /usr/include/Performer/pr/pfTexture.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfList.h
vsWireframeAttribute.o: /usr/include/Performer/image.h
vsWireframeAttribute.o: vsTransparencyAttribute.h++ vsLightAttribute.h++
vsWireframeAttribute.o: /usr/include/Performer/pr/pfLight.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfLightSource.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfNode.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfUpdatable.h
vsWireframeAttribute.o: /usr/include/Performer/pf.h
vsWireframeAttribute.o: /usr/include/Performer/pfstats.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfBuffer.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfUpdate.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfLists.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfFrameStats.h
vsWireframeAttribute.o: /usr/include/Performer/pf/pfPipe.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfVideoChannel.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfCalligraphic.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfStats.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfDispList.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfState.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfByteBank.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfGSetBank.h
vsWireframeAttribute.o: /usr/include/Performer/pr/pfGeoMath.h vsNode.h++
vsWireframeAttribute.o: ../../util/common/vsVector.h++
vsWireframeAttribute.o: ../../util/common/vsMatrix.h++
vsWireframeAttribute.o: ../../util/common/vsQuat.h++
vsTextBuilder.o: vsTextBuilder.h++ /usr/include/Performer/pf/pfText.h
vsTextBuilder.o: /usr/include/Performer/pf/pfNode.h
vsTextBuilder.o: /usr/include/Performer/pf/pfUpdatable.h
vsTextBuilder.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsTextBuilder.o: /usr/include/sys/types.h /usr/include/features.h
vsTextBuilder.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTextBuilder.o: /usr/include/bits/types.h
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTextBuilder.o: /usr/include/time.h /usr/include/endian.h
vsTextBuilder.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsTextBuilder.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsTextBuilder.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsTextBuilder.o: /usr/include/stdio.h /usr/include/libio.h
vsTextBuilder.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTextBuilder.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTextBuilder.o: /usr/include/bits/stdio_lim.h
vsTextBuilder.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTextBuilder.o: /usr/include/ulocks.h /usr/include/malloc.h
vsTextBuilder.o: /usr/include/limits.h
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTextBuilder.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsTextBuilder.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsTextBuilder.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsTextBuilder.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsTextBuilder.o: /usr/include/errno.h /usr/include/bits/errno.h
vsTextBuilder.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsTextBuilder.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsTextBuilder.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsTextBuilder.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsTextBuilder.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsTextBuilder.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsTextBuilder.o: /usr/include/Performer/linuxstubs.h
vsTextBuilder.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsTextBuilder.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsTextBuilder.o: /usr/include/alloca.h /usr/include/string.h
vsTextBuilder.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsTextBuilder.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsTextBuilder.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsTextBuilder.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTextBuilder.o: /usr/include/bits/mathcalls.h
vsTextBuilder.o: /usr/include/Performer/prstats.h
vsTextBuilder.o: /usr/include/Performer/pr/pfLinMath.h
vsTextBuilder.o: /usr/include/Performer/pr/pfObject.h
vsTextBuilder.o: /usr/include/Performer/pr/pfMemory.h
vsTextBuilder.o: /usr/include/Performer/pr/pfType.h
vsTextBuilder.o: /usr/include/Performer/pr/pfStruct.h
vsTextBuilder.o: /usr/include/Performer/pfstats.h
vsTextBuilder.o: /usr/include/Performer/pf/pfBuffer.h
vsTextBuilder.o: /usr/include/Performer/pf/pfUpdate.h
vsTextBuilder.o: /usr/include/Performer/pf/pfLists.h
vsTextBuilder.o: /usr/include/Performer/pf/pfFrameStats.h
vsTextBuilder.o: /usr/include/Performer/pf/pfPipe.h
vsTextBuilder.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTextBuilder.o: /usr/include/Performer/pr/pfList.h
vsTextBuilder.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTextBuilder.o: /usr/include/Performer/pr/pfStats.h
vsTextBuilder.o: /usr/include/Performer/pr/pfDispList.h
vsTextBuilder.o: /usr/include/Performer/pr/pfGeoState.h
vsTextBuilder.o: /usr/include/Performer/pr/pfState.h
vsTextBuilder.o: /usr/include/Performer/pr/pfByteBank.h
vsTextBuilder.o: /usr/include/Performer/pr/pfGSetBank.h
vsTextBuilder.o: /usr/include/Performer/pr/pfGeoMath.h
vsTextBuilder.o: /usr/include/Performer/pr/pfString.h
vsTextBuilder.o: /usr/include/Performer/pfdu.h /usr/include/dlfcn.h
vsTextBuilder.o: /usr/include/bits/dlfcn.h /usr/include/Performer/pfutil.h
vsTextBuilder.o: /usr/include/Performer/objfnt.h
vsTextBuilder.o: ../../util/common/vsVector.h++
vsTextBuilder.o: ../../util/common/vsGlobals.h++
vsTextBuilder.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsTextBuilder.o: vsComponent.h++ /usr/include/Performer/pf/pfGroup.h
vsTextBuilder.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsTextBuilder.o: ../../util/common/vsObject.h++
vsTextBuilder.o: ../../util/common/vsTreeMap.h++
vsTextBuilder.o: ../../util/common/vsObjectMap.h++ vsNode.h++ vsGeometry.h++
vsTextBuilder.o: /usr/include/Performer/pf/pfGeode.h
vsTextBuilder.o: /usr/include/Performer/pr/pfFlux.h
vsTextBuilder.o: /usr/include/Performer/pr/pfEngine.h
vsTextBuilder.o: /usr/include/Performer/pr/pfGeoSet.h
vsTextBuilder.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsTextBuilder.o: /usr/include/Performer/pr/pfPassList.h
vsTextBuilder.o: /usr/include/Performer/pr/pfFBState.h
vsTextBuilder.o: /usr/include/Performer/pr/pfWindow.h
vsTextBuilder.o: vsTransformAttribute.h++ /usr/include/Performer/pf/pfDCS.h
vsTextBuilder.o: /usr/include/Performer/pf/pfSCS.h vsDatabaseLoader.h++
vsTextBuilder.o: /usr/include/Performer/pfdb/pfflt.h
