#!/bin/make
# makefile for motion/common

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../system/$(SG) \
	  -I../../io/common \
	  -I../../io/$(WS) \
	  -I../../io/$(SG)_$(WS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
          -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
	  vs3TrackerArm.o \
          vsAxisRotation.o \
	  vsCollision.o \
          vsDifferentialTrackedOrientation.o \
	  vsDrivingMotion.o \
	  vsFlyingMotion.o \
	  vsKinematics.o \
	  vsMotionModel.o \
	  vsSphericalMotion.o \
	  vsTerrainFollow.o \
	  vsTrackballMotion.o \
	  vsTrackedMotion.o \
	  vsWalkArticulation.o \
	  vsWalkInPlace.o

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

vs3TrackerArm.o: vs3TrackerArm.h++ vsMotionModel.h++
vs3TrackerArm.o: ../../util/common/vsUpdatable.h++
vs3TrackerArm.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vs3TrackerArm.o: /usr/include/features.h /usr/include/sys/cdefs.h
vs3TrackerArm.o: /usr/include/gnu/stubs.h
vs3TrackerArm.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vs3TrackerArm.o: /usr/include/bits/types.h /usr/include/libio.h
vs3TrackerArm.o: /usr/include/_G_config.h /usr/include/wchar.h
vs3TrackerArm.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vs3TrackerArm.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vs3TrackerArm.o: /usr/include/bits/stdio_lim.h
vs3TrackerArm.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vs3TrackerArm.o: ../../util/common/vsTreeMap.h++
vs3TrackerArm.o: ../../util/common/vsGrowableArray.h++ vsKinematics.h++
vs3TrackerArm.o: ../../graphics/Performer/vsComponent.h++
vs3TrackerArm.o: /usr/include/Performer/pf/pfGroup.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfNode.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfUpdatable.h
vs3TrackerArm.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vs3TrackerArm.o: /usr/include/sys/types.h /usr/include/time.h
vs3TrackerArm.o: /usr/include/endian.h /usr/include/bits/endian.h
vs3TrackerArm.o: /usr/include/sys/select.h /usr/include/bits/select.h
vs3TrackerArm.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vs3TrackerArm.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vs3TrackerArm.o: /usr/include/malloc.h /usr/include/limits.h
vs3TrackerArm.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vs3TrackerArm.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vs3TrackerArm.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vs3TrackerArm.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vs3TrackerArm.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vs3TrackerArm.o: /usr/include/errno.h /usr/include/bits/errno.h
vs3TrackerArm.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vs3TrackerArm.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vs3TrackerArm.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vs3TrackerArm.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vs3TrackerArm.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vs3TrackerArm.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vs3TrackerArm.o: /usr/include/Performer/linuxstubs.h
vs3TrackerArm.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vs3TrackerArm.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vs3TrackerArm.o: /usr/include/alloca.h /usr/include/string.h
vs3TrackerArm.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vs3TrackerArm.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vs3TrackerArm.o: /usr/include/Performer/prmath.h /usr/include/math.h
vs3TrackerArm.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vs3TrackerArm.o: /usr/include/bits/mathcalls.h
vs3TrackerArm.o: /usr/include/Performer/prstats.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfLinMath.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfObject.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfMemory.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfType.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfStruct.h
vs3TrackerArm.o: /usr/include/Performer/pfstats.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfBuffer.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfUpdate.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfLists.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfFrameStats.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfPipe.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfVideoChannel.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfList.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfCalligraphic.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfStats.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfDispList.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfGeoState.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfState.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfByteBank.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfGSetBank.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfGeoMath.h
vs3TrackerArm.o: ../../util/common/vsVector.h++
vs3TrackerArm.o: ../../util/common/vsGlobals.h++
vs3TrackerArm.o: ../../graphics/Performer/vsAttribute.h++
vs3TrackerArm.o: ../../util/common/vsObjectMap.h++
vs3TrackerArm.o: ../../graphics/Performer/vsNode.h++
vs3TrackerArm.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vs3TrackerArm.o: ../../graphics/Performer/vsTransformAttribute.h++
vs3TrackerArm.o: /usr/include/Performer/pf/pfDCS.h
vs3TrackerArm.o: /usr/include/Performer/pf/pfSCS.h
vs3TrackerArm.o: ../../graphics/Performer/vsDatabaseLoader.h++
vs3TrackerArm.o: /usr/include/Performer/pfdb/pfflt.h
vs3TrackerArm.o: ../../graphics/Performer/vsGeometry.h++
vs3TrackerArm.o: /usr/include/Performer/pf/pfGeode.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfFlux.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfEngine.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfGeoSet.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfCycleBuffer.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfPassList.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfFBState.h
vs3TrackerArm.o: /usr/include/Performer/pr/pfWindow.h
vs3TrackerArm.o: ../../io/common/vsMotionTracker.h++
vs3TrackerArm.o: ../../io/common/vs6DInputDevice.h++
vs3TrackerArm.o: ../../io/common/vsInputDevice.h++
vs3TrackerArm.o: ../../io/common/vsInputAxis.h++
vs3TrackerArm.o: ../../io/common/vsInputButton.h++
vsAxisRotation.o: vsAxisRotation.h++ vsMotionModel.h++
vsAxisRotation.o: ../../util/common/vsUpdatable.h++
vsAxisRotation.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsAxisRotation.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsAxisRotation.o: /usr/include/gnu/stubs.h
vsAxisRotation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsAxisRotation.o: /usr/include/bits/types.h /usr/include/libio.h
vsAxisRotation.o: /usr/include/_G_config.h /usr/include/wchar.h
vsAxisRotation.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsAxisRotation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsAxisRotation.o: /usr/include/bits/stdio_lim.h
vsAxisRotation.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsAxisRotation.o: ../../util/common/vsTreeMap.h++
vsAxisRotation.o: ../../util/common/vsGrowableArray.h++ vsKinematics.h++
vsAxisRotation.o: ../../graphics/Performer/vsComponent.h++
vsAxisRotation.o: /usr/include/Performer/pf/pfGroup.h
vsAxisRotation.o: /usr/include/Performer/pf/pfNode.h
vsAxisRotation.o: /usr/include/Performer/pf/pfUpdatable.h
vsAxisRotation.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsAxisRotation.o: /usr/include/sys/types.h /usr/include/time.h
vsAxisRotation.o: /usr/include/endian.h /usr/include/bits/endian.h
vsAxisRotation.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsAxisRotation.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsAxisRotation.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsAxisRotation.o: /usr/include/malloc.h /usr/include/limits.h
vsAxisRotation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsAxisRotation.o: /usr/include/bits/posix1_lim.h
vsAxisRotation.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsAxisRotation.o: /usr/include/bits/posix2_lim.h
vsAxisRotation.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsAxisRotation.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsAxisRotation.o: /usr/include/errno.h /usr/include/bits/errno.h
vsAxisRotation.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsAxisRotation.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsAxisRotation.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsAxisRotation.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsAxisRotation.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsAxisRotation.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsAxisRotation.o: /usr/include/Performer/linuxstubs.h
vsAxisRotation.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsAxisRotation.o: /usr/include/bits/waitflags.h
vsAxisRotation.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsAxisRotation.o: /usr/include/string.h /usr/include/sys/prctl.h
vsAxisRotation.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsAxisRotation.o: /usr/include/bits/wordsize.h
vsAxisRotation.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsAxisRotation.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsAxisRotation.o: /usr/include/bits/mathcalls.h
vsAxisRotation.o: /usr/include/Performer/prstats.h
vsAxisRotation.o: /usr/include/Performer/pr/pfLinMath.h
vsAxisRotation.o: /usr/include/Performer/pr/pfObject.h
vsAxisRotation.o: /usr/include/Performer/pr/pfMemory.h
vsAxisRotation.o: /usr/include/Performer/pr/pfType.h
vsAxisRotation.o: /usr/include/Performer/pr/pfStruct.h
vsAxisRotation.o: /usr/include/Performer/pfstats.h
vsAxisRotation.o: /usr/include/Performer/pf/pfBuffer.h
vsAxisRotation.o: /usr/include/Performer/pf/pfUpdate.h
vsAxisRotation.o: /usr/include/Performer/pf/pfLists.h
vsAxisRotation.o: /usr/include/Performer/pf/pfFrameStats.h
vsAxisRotation.o: /usr/include/Performer/pf/pfPipe.h
vsAxisRotation.o: /usr/include/Performer/pr/pfVideoChannel.h
vsAxisRotation.o: /usr/include/Performer/pr/pfList.h
vsAxisRotation.o: /usr/include/Performer/pr/pfCalligraphic.h
vsAxisRotation.o: /usr/include/Performer/pr/pfStats.h
vsAxisRotation.o: /usr/include/Performer/pr/pfDispList.h
vsAxisRotation.o: /usr/include/Performer/pr/pfGeoState.h
vsAxisRotation.o: /usr/include/Performer/pr/pfState.h
vsAxisRotation.o: /usr/include/Performer/pr/pfByteBank.h
vsAxisRotation.o: /usr/include/Performer/pr/pfGSetBank.h
vsAxisRotation.o: /usr/include/Performer/pr/pfGeoMath.h
vsAxisRotation.o: ../../util/common/vsVector.h++
vsAxisRotation.o: ../../util/common/vsGlobals.h++
vsAxisRotation.o: ../../graphics/Performer/vsAttribute.h++
vsAxisRotation.o: ../../util/common/vsObjectMap.h++
vsAxisRotation.o: ../../graphics/Performer/vsNode.h++
vsAxisRotation.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsAxisRotation.o: ../../graphics/Performer/vsTransformAttribute.h++
vsAxisRotation.o: /usr/include/Performer/pf/pfDCS.h
vsAxisRotation.o: /usr/include/Performer/pf/pfSCS.h
vsAxisRotation.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsAxisRotation.o: /usr/include/Performer/pfdb/pfflt.h
vsAxisRotation.o: ../../graphics/Performer/vsGeometry.h++
vsAxisRotation.o: /usr/include/Performer/pf/pfGeode.h
vsAxisRotation.o: /usr/include/Performer/pr/pfFlux.h
vsAxisRotation.o: /usr/include/Performer/pr/pfEngine.h
vsAxisRotation.o: /usr/include/Performer/pr/pfGeoSet.h
vsAxisRotation.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsAxisRotation.o: /usr/include/Performer/pr/pfPassList.h
vsAxisRotation.o: /usr/include/Performer/pr/pfFBState.h
vsAxisRotation.o: /usr/include/Performer/pr/pfWindow.h
vsAxisRotation.o: ../../io/common/vsInputAxis.h++
vsAxisRotation.o: ../../io/common/vsInputButton.h++
vsCollision.o: /usr/include/stdio.h /usr/include/features.h
vsCollision.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsCollision.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsCollision.o: /usr/include/bits/types.h /usr/include/libio.h
vsCollision.o: /usr/include/_G_config.h /usr/include/wchar.h
vsCollision.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsCollision.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsCollision.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsCollision.o: /usr/include/getopt.h vsCollision.h++ vsMotionModel.h++
vsCollision.o: ../../util/common/vsUpdatable.h++
vsCollision.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsCollision.o: ../../util/common/vsGrowableArray.h++ vsKinematics.h++
vsCollision.o: ../../graphics/Performer/vsComponent.h++
vsCollision.o: /usr/include/Performer/pf/pfGroup.h
vsCollision.o: /usr/include/Performer/pf/pfNode.h
vsCollision.o: /usr/include/Performer/pf/pfUpdatable.h
vsCollision.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsCollision.o: /usr/include/sys/types.h /usr/include/time.h
vsCollision.o: /usr/include/endian.h /usr/include/bits/endian.h
vsCollision.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsCollision.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsCollision.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsCollision.o: /usr/include/malloc.h /usr/include/limits.h
vsCollision.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsCollision.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsCollision.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsCollision.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsCollision.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsCollision.o: /usr/include/errno.h /usr/include/bits/errno.h
vsCollision.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsCollision.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsCollision.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsCollision.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsCollision.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsCollision.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsCollision.o: /usr/include/Performer/linuxstubs.h
vsCollision.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsCollision.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsCollision.o: /usr/include/alloca.h /usr/include/string.h
vsCollision.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsCollision.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsCollision.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsCollision.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsCollision.o: /usr/include/bits/mathcalls.h /usr/include/Performer/prstats.h
vsCollision.o: /usr/include/Performer/pr/pfLinMath.h
vsCollision.o: /usr/include/Performer/pr/pfObject.h
vsCollision.o: /usr/include/Performer/pr/pfMemory.h
vsCollision.o: /usr/include/Performer/pr/pfType.h
vsCollision.o: /usr/include/Performer/pr/pfStruct.h
vsCollision.o: /usr/include/Performer/pfstats.h
vsCollision.o: /usr/include/Performer/pf/pfBuffer.h
vsCollision.o: /usr/include/Performer/pf/pfUpdate.h
vsCollision.o: /usr/include/Performer/pf/pfLists.h
vsCollision.o: /usr/include/Performer/pf/pfFrameStats.h
vsCollision.o: /usr/include/Performer/pf/pfPipe.h
vsCollision.o: /usr/include/Performer/pr/pfVideoChannel.h
vsCollision.o: /usr/include/Performer/pr/pfList.h
vsCollision.o: /usr/include/Performer/pr/pfCalligraphic.h
vsCollision.o: /usr/include/Performer/pr/pfStats.h
vsCollision.o: /usr/include/Performer/pr/pfDispList.h
vsCollision.o: /usr/include/Performer/pr/pfGeoState.h
vsCollision.o: /usr/include/Performer/pr/pfState.h
vsCollision.o: /usr/include/Performer/pr/pfByteBank.h
vsCollision.o: /usr/include/Performer/pr/pfGSetBank.h
vsCollision.o: /usr/include/Performer/pr/pfGeoMath.h
vsCollision.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsCollision.o: ../../graphics/Performer/vsAttribute.h++
vsCollision.o: ../../util/common/vsObjectMap.h++
vsCollision.o: ../../graphics/Performer/vsNode.h++
vsCollision.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsCollision.o: ../../graphics/Performer/vsTransformAttribute.h++
vsCollision.o: /usr/include/Performer/pf/pfDCS.h
vsCollision.o: /usr/include/Performer/pf/pfSCS.h
vsCollision.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsCollision.o: /usr/include/Performer/pfdb/pfflt.h
vsCollision.o: ../../graphics/Performer/vsGeometry.h++
vsCollision.o: /usr/include/Performer/pf/pfGeode.h
vsCollision.o: /usr/include/Performer/pr/pfFlux.h
vsCollision.o: /usr/include/Performer/pr/pfEngine.h
vsCollision.o: /usr/include/Performer/pr/pfGeoSet.h
vsCollision.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsCollision.o: /usr/include/Performer/pr/pfPassList.h
vsCollision.o: /usr/include/Performer/pr/pfFBState.h
vsCollision.o: /usr/include/Performer/pr/pfWindow.h
vsCollision.o: ../../graphics/Performer/vsIntersect.h++
vsCollision.o: ../../graphics/Performer_X/vsPane.h++
vsCollision.o: /usr/include/Performer/pf/pfChannel.h
vsCollision.o: /usr/include/Performer/pf/pfPipeWindow.h
vsCollision.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsCollision.o: /usr/include/Performer/pf/pfEarthSky.h
vsCollision.o: /usr/include/Performer/pf/pfLODState.h
vsCollision.o: /usr/include/Performer/pf/pfScene.h
vsCollision.o: ../../graphics/Performer_X/vsWindow.h++
vsCollision.o: ../../graphics/Performer_X/vsScreen.h++
vsCollision.o: ../../graphics/Performer_X/vsPipe.h++
vsCollision.o: ../../graphics/Performer/vsView.h++
vsCollision.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsCollision.o: ../../graphics/Performer/vsScene.h++
vsCollision.o: ../../graphics/Performer/vsLightAttribute.h++
vsCollision.o: /usr/include/Performer/pr/pfLight.h
vsCollision.o: /usr/include/Performer/pf/pfLightSource.h
vsDifferentialTrackedOrientation.o: vsDifferentialTrackedOrientation.h++
vsDifferentialTrackedOrientation.o: vsMotionModel.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsUpdatable.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsObject.h++
vsDifferentialTrackedOrientation.o: /usr/include/stdio.h
vsDifferentialTrackedOrientation.o: /usr/include/features.h
vsDifferentialTrackedOrientation.o: /usr/include/sys/cdefs.h
vsDifferentialTrackedOrientation.o: /usr/include/gnu/stubs.h
vsDifferentialTrackedOrientation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/types.h
vsDifferentialTrackedOrientation.o: /usr/include/libio.h
vsDifferentialTrackedOrientation.o: /usr/include/_G_config.h
vsDifferentialTrackedOrientation.o: /usr/include/wchar.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/wchar.h
vsDifferentialTrackedOrientation.o: /usr/include/gconv.h
vsDifferentialTrackedOrientation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/stdio_lim.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/sys_errlist.h
vsDifferentialTrackedOrientation.o: /usr/include/getopt.h
vsDifferentialTrackedOrientation.o: ../../util/common/vsTreeMap.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsGrowableArray.h++
vsDifferentialTrackedOrientation.o: ../../io/common/vsMotionTracker.h++
vsDifferentialTrackedOrientation.o: ../../io/common/vs6DInputDevice.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsVector.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsGlobals.h++
vsDifferentialTrackedOrientation.o: /usr/include/math.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/huge_val.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/mathdef.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/mathcalls.h
vsDifferentialTrackedOrientation.o: ../../util/common/vsMatrix.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsQuat.h++
vsDifferentialTrackedOrientation.o: ../../io/common/vsInputDevice.h++
vsDifferentialTrackedOrientation.o: ../../io/common/vsInputAxis.h++
vsDifferentialTrackedOrientation.o: ../../io/common/vsInputButton.h++
vsDifferentialTrackedOrientation.o: vsKinematics.h++
vsDifferentialTrackedOrientation.o: ../../graphics/Performer/vsComponent.h++
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfGroup.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfNode.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfUpdatable.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr.h
vsDifferentialTrackedOrientation.o: /usr/include/sys/types.h
vsDifferentialTrackedOrientation.o: /usr/include/time.h /usr/include/endian.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/endian.h
vsDifferentialTrackedOrientation.o: /usr/include/sys/select.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/select.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/sigset.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/time.h
vsDifferentialTrackedOrientation.o: /usr/include/sys/sysmacros.h
vsDifferentialTrackedOrientation.o: /usr/include/ulocks.h
vsDifferentialTrackedOrientation.o: /usr/include/malloc.h
vsDifferentialTrackedOrientation.o: /usr/include/limits.h
vsDifferentialTrackedOrientation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/posix1_lim.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/local_lim.h
vsDifferentialTrackedOrientation.o: /usr/include/linux/limits.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/posix2_lim.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/xopen_lim.h
vsDifferentialTrackedOrientation.o: /usr/include/unistd.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/posix_opt.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/confname.h
vsDifferentialTrackedOrientation.o: /usr/include/errno.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/errno.h
vsDifferentialTrackedOrientation.o: /usr/include/linux/errno.h
vsDifferentialTrackedOrientation.o: /usr/include/asm/errno.h
vsDifferentialTrackedOrientation.o: /usr/include/X11/X.h
vsDifferentialTrackedOrientation.o: /usr/include/X11/Xlib.h
vsDifferentialTrackedOrientation.o: /usr/include/X11/Xfuncproto.h
vsDifferentialTrackedOrientation.o: /usr/include/X11/Xosdefs.h
vsDifferentialTrackedOrientation.o: /usr/include/X11/Xutil.h
vsDifferentialTrackedOrientation.o: /usr/include/GL/gl.h
vsDifferentialTrackedOrientation.o: /usr/include/GL/glx.h
vsDifferentialTrackedOrientation.o: /usr/include/GL/glxtokens.h
vsDifferentialTrackedOrientation.o: /usr/include/GL/glu.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/opengl.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/linuxstubs.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/image.h
vsDifferentialTrackedOrientation.o: /usr/include/stdlib.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/waitflags.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/waitstatus.h
vsDifferentialTrackedOrientation.o: /usr/include/alloca.h
vsDifferentialTrackedOrientation.o: /usr/include/string.h
vsDifferentialTrackedOrientation.o: /usr/include/sys/prctl.h
vsDifferentialTrackedOrientation.o: /usr/include/linux/prctl.h
vsDifferentialTrackedOrientation.o: /usr/include/stdint.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/wordsize.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/prmath.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/prstats.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfLinMath.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfObject.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfMemory.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfType.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfStruct.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pfstats.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfBuffer.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfUpdate.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfLists.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfFrameStats.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfPipe.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfVideoChannel.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfList.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfCalligraphic.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfStats.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfDispList.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfGeoState.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfState.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfByteBank.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfGSetBank.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfGeoMath.h
vsDifferentialTrackedOrientation.o: ../../graphics/Performer/vsAttribute.h++
vsDifferentialTrackedOrientation.o: ../../util/common/vsObjectMap.h++
vsDifferentialTrackedOrientation.o: ../../graphics/Performer/vsNode.h++
vsDifferentialTrackedOrientation.o: ../../graphics/Performer/vsTransformAttribute.h++
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfDCS.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfSCS.h
vsDifferentialTrackedOrientation.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pfdb/pfflt.h
vsDifferentialTrackedOrientation.o: ../../graphics/Performer/vsGeometry.h++
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pf/pfGeode.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfFlux.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfEngine.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfGeoSet.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfPassList.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfFBState.h
vsDifferentialTrackedOrientation.o: /usr/include/Performer/pr/pfWindow.h
vsDrivingMotion.o: vsDrivingMotion.h++ vsMotionModel.h++
vsDrivingMotion.o: ../../util/common/vsUpdatable.h++
vsDrivingMotion.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsDrivingMotion.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsDrivingMotion.o: /usr/include/gnu/stubs.h
vsDrivingMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDrivingMotion.o: /usr/include/bits/types.h /usr/include/libio.h
vsDrivingMotion.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDrivingMotion.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDrivingMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDrivingMotion.o: /usr/include/bits/stdio_lim.h
vsDrivingMotion.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDrivingMotion.o: ../../util/common/vsTreeMap.h++
vsDrivingMotion.o: ../../util/common/vsGrowableArray.h++ vsKinematics.h++
vsDrivingMotion.o: ../../graphics/Performer/vsComponent.h++
vsDrivingMotion.o: /usr/include/Performer/pf/pfGroup.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfNode.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfUpdatable.h
vsDrivingMotion.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsDrivingMotion.o: /usr/include/sys/types.h /usr/include/time.h
vsDrivingMotion.o: /usr/include/endian.h /usr/include/bits/endian.h
vsDrivingMotion.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsDrivingMotion.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsDrivingMotion.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsDrivingMotion.o: /usr/include/malloc.h /usr/include/limits.h
vsDrivingMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsDrivingMotion.o: /usr/include/bits/posix1_lim.h
vsDrivingMotion.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsDrivingMotion.o: /usr/include/bits/posix2_lim.h
vsDrivingMotion.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsDrivingMotion.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsDrivingMotion.o: /usr/include/errno.h /usr/include/bits/errno.h
vsDrivingMotion.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsDrivingMotion.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsDrivingMotion.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsDrivingMotion.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsDrivingMotion.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsDrivingMotion.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsDrivingMotion.o: /usr/include/Performer/linuxstubs.h
vsDrivingMotion.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsDrivingMotion.o: /usr/include/bits/waitflags.h
vsDrivingMotion.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsDrivingMotion.o: /usr/include/string.h /usr/include/sys/prctl.h
vsDrivingMotion.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsDrivingMotion.o: /usr/include/bits/wordsize.h
vsDrivingMotion.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsDrivingMotion.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsDrivingMotion.o: /usr/include/bits/mathcalls.h
vsDrivingMotion.o: /usr/include/Performer/prstats.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfLinMath.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfObject.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfMemory.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfType.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfStruct.h
vsDrivingMotion.o: /usr/include/Performer/pfstats.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfBuffer.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfUpdate.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfLists.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfFrameStats.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfPipe.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfVideoChannel.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfList.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfCalligraphic.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfStats.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfDispList.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfGeoState.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfState.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfByteBank.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfGSetBank.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfGeoMath.h
vsDrivingMotion.o: ../../util/common/vsVector.h++
vsDrivingMotion.o: ../../util/common/vsGlobals.h++
vsDrivingMotion.o: ../../graphics/Performer/vsAttribute.h++
vsDrivingMotion.o: ../../util/common/vsObjectMap.h++
vsDrivingMotion.o: ../../graphics/Performer/vsNode.h++
vsDrivingMotion.o: ../../util/common/vsMatrix.h++
vsDrivingMotion.o: ../../util/common/vsQuat.h++
vsDrivingMotion.o: ../../graphics/Performer/vsTransformAttribute.h++
vsDrivingMotion.o: /usr/include/Performer/pf/pfDCS.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfSCS.h
vsDrivingMotion.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsDrivingMotion.o: /usr/include/Performer/pfdb/pfflt.h
vsDrivingMotion.o: ../../graphics/Performer/vsGeometry.h++
vsDrivingMotion.o: /usr/include/Performer/pf/pfGeode.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfFlux.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfEngine.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfGeoSet.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfPassList.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfFBState.h
vsDrivingMotion.o: /usr/include/Performer/pr/pfWindow.h
vsDrivingMotion.o: ../../io/common/vsMouse.h++
vsDrivingMotion.o: ../../io/common/vsInputDevice.h++
vsDrivingMotion.o: ../../io/common/vsInputAxis.h++
vsDrivingMotion.o: ../../io/common/vsInputButton.h++
vsDrivingMotion.o: ../../system/Performer/vsSystem.h++
vsDrivingMotion.o: ../../graphics/Performer_X/vsPipe.h++
vsDrivingMotion.o: ../../graphics/Performer_X/vsScreen.h++
vsDrivingMotion.o: ../../graphics/Performer_X/vsWindow.h++
vsDrivingMotion.o: /usr/include/Performer/pf/pfPipeWindow.h
vsDrivingMotion.o: ../../graphics/Performer_X/vsPane.h++
vsDrivingMotion.o: /usr/include/Performer/pf/pfChannel.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfEarthSky.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfLODState.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfScene.h
vsDrivingMotion.o: ../../graphics/Performer/vsView.h++
vsDrivingMotion.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsDrivingMotion.o: ../../graphics/Performer/vsScene.h++
vsDrivingMotion.o: ../../graphics/Performer/vsLightAttribute.h++
vsDrivingMotion.o: /usr/include/Performer/pr/pfLight.h
vsDrivingMotion.o: /usr/include/Performer/pf/pfLightSource.h
vsDrivingMotion.o: ../../util/common/vsTimer.h++
vsFlyingMotion.o: vsFlyingMotion.h++ vsMotionModel.h++
vsFlyingMotion.o: ../../util/common/vsUpdatable.h++
vsFlyingMotion.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsFlyingMotion.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsFlyingMotion.o: /usr/include/gnu/stubs.h
vsFlyingMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsFlyingMotion.o: /usr/include/bits/types.h /usr/include/libio.h
vsFlyingMotion.o: /usr/include/_G_config.h /usr/include/wchar.h
vsFlyingMotion.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsFlyingMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsFlyingMotion.o: /usr/include/bits/stdio_lim.h
vsFlyingMotion.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsFlyingMotion.o: ../../util/common/vsTreeMap.h++
vsFlyingMotion.o: ../../util/common/vsGrowableArray.h++
vsFlyingMotion.o: ../../io/common/vsMouse.h++
vsFlyingMotion.o: ../../io/common/vsInputDevice.h++
vsFlyingMotion.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsFlyingMotion.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsFlyingMotion.o: /usr/include/bits/mathcalls.h
vsFlyingMotion.o: ../../io/common/vsInputAxis.h++
vsFlyingMotion.o: ../../io/common/vsInputButton.h++ vsKinematics.h++
vsFlyingMotion.o: ../../graphics/Performer/vsComponent.h++
vsFlyingMotion.o: /usr/include/Performer/pf/pfGroup.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfNode.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfUpdatable.h
vsFlyingMotion.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsFlyingMotion.o: /usr/include/sys/types.h /usr/include/time.h
vsFlyingMotion.o: /usr/include/endian.h /usr/include/bits/endian.h
vsFlyingMotion.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsFlyingMotion.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsFlyingMotion.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsFlyingMotion.o: /usr/include/malloc.h /usr/include/limits.h
vsFlyingMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsFlyingMotion.o: /usr/include/bits/posix1_lim.h
vsFlyingMotion.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsFlyingMotion.o: /usr/include/bits/posix2_lim.h
vsFlyingMotion.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsFlyingMotion.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsFlyingMotion.o: /usr/include/errno.h /usr/include/bits/errno.h
vsFlyingMotion.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsFlyingMotion.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsFlyingMotion.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsFlyingMotion.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsFlyingMotion.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsFlyingMotion.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsFlyingMotion.o: /usr/include/Performer/linuxstubs.h
vsFlyingMotion.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsFlyingMotion.o: /usr/include/bits/waitflags.h
vsFlyingMotion.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsFlyingMotion.o: /usr/include/string.h /usr/include/sys/prctl.h
vsFlyingMotion.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsFlyingMotion.o: /usr/include/bits/wordsize.h
vsFlyingMotion.o: /usr/include/Performer/prmath.h
vsFlyingMotion.o: /usr/include/Performer/prstats.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfLinMath.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfObject.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfMemory.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfType.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfStruct.h
vsFlyingMotion.o: /usr/include/Performer/pfstats.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfBuffer.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfUpdate.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfLists.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfFrameStats.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfPipe.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfVideoChannel.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfList.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfCalligraphic.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfStats.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfDispList.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfGeoState.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfState.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfByteBank.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfGSetBank.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfGeoMath.h
vsFlyingMotion.o: ../../util/common/vsVector.h++
vsFlyingMotion.o: ../../graphics/Performer/vsAttribute.h++
vsFlyingMotion.o: ../../util/common/vsObjectMap.h++
vsFlyingMotion.o: ../../graphics/Performer/vsNode.h++
vsFlyingMotion.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsFlyingMotion.o: ../../graphics/Performer/vsTransformAttribute.h++
vsFlyingMotion.o: /usr/include/Performer/pf/pfDCS.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfSCS.h
vsFlyingMotion.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsFlyingMotion.o: /usr/include/Performer/pfdb/pfflt.h
vsFlyingMotion.o: ../../graphics/Performer/vsGeometry.h++
vsFlyingMotion.o: /usr/include/Performer/pf/pfGeode.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfFlux.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfEngine.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfGeoSet.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfPassList.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfFBState.h
vsFlyingMotion.o: /usr/include/Performer/pr/pfWindow.h
vsFlyingMotion.o: ../../system/Performer/vsSystem.h++
vsFlyingMotion.o: ../../graphics/Performer_X/vsPipe.h++
vsFlyingMotion.o: ../../graphics/Performer_X/vsScreen.h++
vsFlyingMotion.o: ../../graphics/Performer_X/vsWindow.h++
vsFlyingMotion.o: /usr/include/Performer/pf/pfPipeWindow.h
vsFlyingMotion.o: ../../graphics/Performer_X/vsPane.h++
vsFlyingMotion.o: /usr/include/Performer/pf/pfChannel.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfEarthSky.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfLODState.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfScene.h
vsFlyingMotion.o: ../../graphics/Performer/vsView.h++
vsFlyingMotion.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsFlyingMotion.o: ../../graphics/Performer/vsScene.h++
vsFlyingMotion.o: ../../graphics/Performer/vsLightAttribute.h++
vsFlyingMotion.o: /usr/include/Performer/pr/pfLight.h
vsFlyingMotion.o: /usr/include/Performer/pf/pfLightSource.h
vsFlyingMotion.o: ../../util/common/vsTimer.h++
vsKinematics.o: /usr/include/stdio.h /usr/include/features.h
vsKinematics.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsKinematics.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsKinematics.o: /usr/include/bits/types.h /usr/include/libio.h
vsKinematics.o: /usr/include/_G_config.h /usr/include/wchar.h
vsKinematics.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsKinematics.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsKinematics.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsKinematics.o: /usr/include/getopt.h ../../system/Performer/vsSystem.h++
vsKinematics.o: ../../graphics/Performer_X/vsPipe.h++
vsKinematics.o: /usr/include/Performer/pf/pfPipe.h
vsKinematics.o: /usr/include/Performer/pf/pfUpdatable.h
vsKinematics.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsKinematics.o: /usr/include/sys/types.h /usr/include/time.h
vsKinematics.o: /usr/include/endian.h /usr/include/bits/endian.h
vsKinematics.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsKinematics.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsKinematics.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsKinematics.o: /usr/include/malloc.h /usr/include/limits.h
vsKinematics.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsKinematics.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsKinematics.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsKinematics.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsKinematics.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsKinematics.o: /usr/include/errno.h /usr/include/bits/errno.h
vsKinematics.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsKinematics.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsKinematics.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsKinematics.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsKinematics.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsKinematics.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsKinematics.o: /usr/include/Performer/linuxstubs.h
vsKinematics.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsKinematics.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsKinematics.o: /usr/include/alloca.h /usr/include/string.h
vsKinematics.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsKinematics.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsKinematics.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsKinematics.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsKinematics.o: /usr/include/bits/mathcalls.h
vsKinematics.o: /usr/include/Performer/prstats.h
vsKinematics.o: /usr/include/Performer/pr/pfLinMath.h
vsKinematics.o: /usr/include/Performer/pr/pfObject.h
vsKinematics.o: /usr/include/Performer/pr/pfMemory.h
vsKinematics.o: /usr/include/Performer/pr/pfType.h
vsKinematics.o: /usr/include/Performer/pr/pfStruct.h
vsKinematics.o: /usr/include/Performer/pfstats.h
vsKinematics.o: /usr/include/Performer/pf/pfBuffer.h
vsKinematics.o: /usr/include/Performer/pf/pfUpdate.h
vsKinematics.o: /usr/include/Performer/pf/pfLists.h
vsKinematics.o: /usr/include/Performer/pr/pfVideoChannel.h
vsKinematics.o: /usr/include/Performer/pr/pfList.h
vsKinematics.o: /usr/include/Performer/pr/pfCalligraphic.h
vsKinematics.o: /usr/include/Performer/pr/pfStats.h
vsKinematics.o: /usr/include/Performer/pr/pfDispList.h
vsKinematics.o: /usr/include/Performer/pr/pfGeoState.h
vsKinematics.o: /usr/include/Performer/pr/pfState.h
vsKinematics.o: /usr/include/Performer/pr/pfByteBank.h
vsKinematics.o: /usr/include/Performer/pr/pfGSetBank.h
vsKinematics.o: ../../util/common/vsGlobals.h++
vsKinematics.o: ../../graphics/Performer_X/vsScreen.h++
vsKinematics.o: ../../graphics/Performer_X/vsWindow.h++
vsKinematics.o: /usr/include/Performer/pf/pfPipeWindow.h
vsKinematics.o: /usr/include/Performer/pr/pfWindow.h
vsKinematics.o: ../../graphics/Performer_X/vsPane.h++
vsKinematics.o: /usr/include/Performer/pf/pfChannel.h
vsKinematics.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsKinematics.o: /usr/include/Performer/pf/pfEarthSky.h
vsKinematics.o: /usr/include/Performer/pf/pfLODState.h
vsKinematics.o: /usr/include/Performer/pf/pfFrameStats.h
vsKinematics.o: /usr/include/Performer/pf/pfScene.h
vsKinematics.o: /usr/include/Performer/pf/pfGroup.h
vsKinematics.o: /usr/include/Performer/pf/pfNode.h
vsKinematics.o: /usr/include/Performer/pr/pfGeoMath.h
vsKinematics.o: /usr/include/Performer/pr/pfGeoSet.h
vsKinematics.o: /usr/include/Performer/pr/pfFlux.h
vsKinematics.o: /usr/include/Performer/pr/pfEngine.h
vsKinematics.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsKinematics.o: /usr/include/Performer/pr/pfPassList.h
vsKinematics.o: /usr/include/Performer/pr/pfFBState.h
vsKinematics.o: ../../graphics/Performer/vsView.h++
vsKinematics.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsKinematics.o: ../../util/common/vsQuat.h++
vsKinematics.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsKinematics.o: ../../graphics/Performer/vsComponent.h++
vsKinematics.o: ../../util/common/vsGrowableArray.h++
vsKinematics.o: ../../graphics/Performer/vsAttribute.h++
vsKinematics.o: ../../util/common/vsObject.h++
vsKinematics.o: ../../util/common/vsTreeMap.h++
vsKinematics.o: ../../util/common/vsObjectMap.h++
vsKinematics.o: ../../graphics/Performer/vsNode.h++
vsKinematics.o: ../../graphics/Performer/vsScene.h++
vsKinematics.o: ../../graphics/Performer/vsLightAttribute.h++
vsKinematics.o: /usr/include/Performer/pr/pfLight.h
vsKinematics.o: /usr/include/Performer/pf/pfLightSource.h
vsKinematics.o: ../../util/common/vsTimer.h++ vsKinematics.h++
vsKinematics.o: ../../util/common/vsUpdatable.h++
vsKinematics.o: ../../graphics/Performer/vsTransformAttribute.h++
vsKinematics.o: /usr/include/Performer/pf/pfDCS.h
vsKinematics.o: /usr/include/Performer/pf/pfSCS.h
vsKinematics.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsKinematics.o: /usr/include/Performer/pfdb/pfflt.h
vsKinematics.o: ../../graphics/Performer/vsGeometry.h++
vsKinematics.o: /usr/include/Performer/pf/pfGeode.h
vsMotionModel.o: vsMotionModel.h++ ../../util/common/vsUpdatable.h++
vsMotionModel.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsMotionModel.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsMotionModel.o: /usr/include/gnu/stubs.h
vsMotionModel.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsMotionModel.o: /usr/include/bits/types.h /usr/include/libio.h
vsMotionModel.o: /usr/include/_G_config.h /usr/include/wchar.h
vsMotionModel.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsMotionModel.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsMotionModel.o: /usr/include/bits/stdio_lim.h
vsMotionModel.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsMotionModel.o: ../../util/common/vsTreeMap.h++
vsMotionModel.o: ../../util/common/vsGrowableArray.h++
vsMotionModel.o: /usr/include/sys/time.h /usr/include/time.h
vsMotionModel.o: /usr/include/bits/time.h /usr/include/sys/select.h
vsMotionModel.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsSphericalMotion.o: vsSphericalMotion.h++ vsMotionModel.h++
vsSphericalMotion.o: ../../util/common/vsUpdatable.h++
vsSphericalMotion.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsSphericalMotion.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsSphericalMotion.o: /usr/include/gnu/stubs.h
vsSphericalMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSphericalMotion.o: /usr/include/bits/types.h /usr/include/libio.h
vsSphericalMotion.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSphericalMotion.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSphericalMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSphericalMotion.o: /usr/include/bits/stdio_lim.h
vsSphericalMotion.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSphericalMotion.o: ../../util/common/vsTreeMap.h++
vsSphericalMotion.o: ../../util/common/vsGrowableArray.h++
vsSphericalMotion.o: ../../io/common/vsMouse.h++
vsSphericalMotion.o: ../../io/common/vsInputDevice.h++
vsSphericalMotion.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsSphericalMotion.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsSphericalMotion.o: /usr/include/bits/mathcalls.h
vsSphericalMotion.o: ../../io/common/vsInputAxis.h++
vsSphericalMotion.o: ../../io/common/vsInputButton.h++ vsKinematics.h++
vsSphericalMotion.o: ../../graphics/Performer/vsComponent.h++
vsSphericalMotion.o: /usr/include/Performer/pf/pfGroup.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfNode.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfUpdatable.h
vsSphericalMotion.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsSphericalMotion.o: /usr/include/sys/types.h /usr/include/time.h
vsSphericalMotion.o: /usr/include/endian.h /usr/include/bits/endian.h
vsSphericalMotion.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsSphericalMotion.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsSphericalMotion.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsSphericalMotion.o: /usr/include/malloc.h /usr/include/limits.h
vsSphericalMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSphericalMotion.o: /usr/include/bits/posix1_lim.h
vsSphericalMotion.o: /usr/include/bits/local_lim.h
vsSphericalMotion.o: /usr/include/linux/limits.h
vsSphericalMotion.o: /usr/include/bits/posix2_lim.h
vsSphericalMotion.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsSphericalMotion.o: /usr/include/bits/posix_opt.h
vsSphericalMotion.o: /usr/include/bits/confname.h /usr/include/errno.h
vsSphericalMotion.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsSphericalMotion.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsSphericalMotion.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsSphericalMotion.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsSphericalMotion.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsSphericalMotion.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsSphericalMotion.o: /usr/include/Performer/opengl.h
vsSphericalMotion.o: /usr/include/Performer/linuxstubs.h
vsSphericalMotion.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsSphericalMotion.o: /usr/include/bits/waitflags.h
vsSphericalMotion.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsSphericalMotion.o: /usr/include/string.h /usr/include/sys/prctl.h
vsSphericalMotion.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsSphericalMotion.o: /usr/include/bits/wordsize.h
vsSphericalMotion.o: /usr/include/Performer/prmath.h
vsSphericalMotion.o: /usr/include/Performer/prstats.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfLinMath.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfObject.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfMemory.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfType.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfStruct.h
vsSphericalMotion.o: /usr/include/Performer/pfstats.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfBuffer.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfUpdate.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfLists.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfFrameStats.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfPipe.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfVideoChannel.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfList.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfCalligraphic.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfStats.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfDispList.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfGeoState.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfState.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfByteBank.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfGSetBank.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfGeoMath.h
vsSphericalMotion.o: ../../util/common/vsVector.h++
vsSphericalMotion.o: ../../graphics/Performer/vsAttribute.h++
vsSphericalMotion.o: ../../util/common/vsObjectMap.h++
vsSphericalMotion.o: ../../graphics/Performer/vsNode.h++
vsSphericalMotion.o: ../../util/common/vsMatrix.h++
vsSphericalMotion.o: ../../util/common/vsQuat.h++
vsSphericalMotion.o: ../../graphics/Performer/vsTransformAttribute.h++
vsSphericalMotion.o: /usr/include/Performer/pf/pfDCS.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfSCS.h
vsSphericalMotion.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsSphericalMotion.o: /usr/include/Performer/pfdb/pfflt.h
vsSphericalMotion.o: ../../graphics/Performer/vsGeometry.h++
vsSphericalMotion.o: /usr/include/Performer/pf/pfGeode.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfFlux.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfEngine.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfGeoSet.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfPassList.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfFBState.h
vsSphericalMotion.o: /usr/include/Performer/pr/pfWindow.h
vsSphericalMotion.o: ../../system/Performer/vsSystem.h++
vsSphericalMotion.o: ../../graphics/Performer_X/vsPipe.h++
vsSphericalMotion.o: ../../graphics/Performer_X/vsScreen.h++
vsSphericalMotion.o: ../../graphics/Performer_X/vsWindow.h++
vsSphericalMotion.o: /usr/include/Performer/pf/pfPipeWindow.h
vsSphericalMotion.o: ../../graphics/Performer_X/vsPane.h++
vsSphericalMotion.o: /usr/include/Performer/pf/pfChannel.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfEarthSky.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfLODState.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfScene.h
vsSphericalMotion.o: ../../graphics/Performer/vsView.h++
vsSphericalMotion.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsSphericalMotion.o: ../../graphics/Performer/vsScene.h++
vsSphericalMotion.o: ../../graphics/Performer/vsLightAttribute.h++
vsSphericalMotion.o: /usr/include/Performer/pr/pfLight.h
vsSphericalMotion.o: /usr/include/Performer/pf/pfLightSource.h
vsSphericalMotion.o: ../../util/common/vsTimer.h++
vsTerrainFollow.o: vsTerrainFollow.h++ vsMotionModel.h++
vsTerrainFollow.o: ../../util/common/vsUpdatable.h++
vsTerrainFollow.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsTerrainFollow.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsTerrainFollow.o: /usr/include/gnu/stubs.h
vsTerrainFollow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTerrainFollow.o: /usr/include/bits/types.h /usr/include/libio.h
vsTerrainFollow.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTerrainFollow.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTerrainFollow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTerrainFollow.o: /usr/include/bits/stdio_lim.h
vsTerrainFollow.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTerrainFollow.o: ../../util/common/vsTreeMap.h++
vsTerrainFollow.o: ../../util/common/vsGrowableArray.h++ vsKinematics.h++
vsTerrainFollow.o: ../../graphics/Performer/vsComponent.h++
vsTerrainFollow.o: /usr/include/Performer/pf/pfGroup.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfNode.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfUpdatable.h
vsTerrainFollow.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsTerrainFollow.o: /usr/include/sys/types.h /usr/include/time.h
vsTerrainFollow.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTerrainFollow.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsTerrainFollow.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsTerrainFollow.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsTerrainFollow.o: /usr/include/malloc.h /usr/include/limits.h
vsTerrainFollow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTerrainFollow.o: /usr/include/bits/posix1_lim.h
vsTerrainFollow.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsTerrainFollow.o: /usr/include/bits/posix2_lim.h
vsTerrainFollow.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsTerrainFollow.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsTerrainFollow.o: /usr/include/errno.h /usr/include/bits/errno.h
vsTerrainFollow.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsTerrainFollow.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsTerrainFollow.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsTerrainFollow.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsTerrainFollow.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsTerrainFollow.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsTerrainFollow.o: /usr/include/Performer/linuxstubs.h
vsTerrainFollow.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsTerrainFollow.o: /usr/include/bits/waitflags.h
vsTerrainFollow.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsTerrainFollow.o: /usr/include/string.h /usr/include/sys/prctl.h
vsTerrainFollow.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsTerrainFollow.o: /usr/include/bits/wordsize.h
vsTerrainFollow.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsTerrainFollow.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTerrainFollow.o: /usr/include/bits/mathcalls.h
vsTerrainFollow.o: /usr/include/Performer/prstats.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfLinMath.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfObject.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfMemory.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfType.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfStruct.h
vsTerrainFollow.o: /usr/include/Performer/pfstats.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfBuffer.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfUpdate.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfLists.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfFrameStats.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfPipe.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfList.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfStats.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfDispList.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfGeoState.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfState.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfByteBank.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfGSetBank.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfGeoMath.h
vsTerrainFollow.o: ../../util/common/vsVector.h++
vsTerrainFollow.o: ../../util/common/vsGlobals.h++
vsTerrainFollow.o: ../../graphics/Performer/vsAttribute.h++
vsTerrainFollow.o: ../../util/common/vsObjectMap.h++
vsTerrainFollow.o: ../../graphics/Performer/vsNode.h++
vsTerrainFollow.o: ../../util/common/vsMatrix.h++
vsTerrainFollow.o: ../../util/common/vsQuat.h++
vsTerrainFollow.o: ../../graphics/Performer/vsTransformAttribute.h++
vsTerrainFollow.o: /usr/include/Performer/pf/pfDCS.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfSCS.h
vsTerrainFollow.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsTerrainFollow.o: /usr/include/Performer/pfdb/pfflt.h
vsTerrainFollow.o: ../../graphics/Performer/vsGeometry.h++
vsTerrainFollow.o: /usr/include/Performer/pf/pfGeode.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfFlux.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfEngine.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfGeoSet.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfPassList.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfFBState.h
vsTerrainFollow.o: /usr/include/Performer/pr/pfWindow.h
vsTerrainFollow.o: ../../graphics/Performer/vsIntersect.h++
vsTerrainFollow.o: ../../graphics/Performer_X/vsPane.h++
vsTerrainFollow.o: /usr/include/Performer/pf/pfChannel.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfPipeWindow.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfEarthSky.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfLODState.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfScene.h
vsTerrainFollow.o: ../../graphics/Performer_X/vsWindow.h++
vsTerrainFollow.o: ../../graphics/Performer_X/vsScreen.h++
vsTerrainFollow.o: ../../graphics/Performer_X/vsPipe.h++
vsTerrainFollow.o: ../../graphics/Performer/vsView.h++
vsTerrainFollow.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsTerrainFollow.o: ../../graphics/Performer/vsScene.h++
vsTerrainFollow.o: ../../graphics/Performer/vsLightAttribute.h++
vsTerrainFollow.o: /usr/include/Performer/pr/pfLight.h
vsTerrainFollow.o: /usr/include/Performer/pf/pfLightSource.h
vsTrackballMotion.o: vsTrackballMotion.h++ vsMotionModel.h++
vsTrackballMotion.o: ../../util/common/vsUpdatable.h++
vsTrackballMotion.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsTrackballMotion.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsTrackballMotion.o: /usr/include/gnu/stubs.h
vsTrackballMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTrackballMotion.o: /usr/include/bits/types.h /usr/include/libio.h
vsTrackballMotion.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTrackballMotion.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTrackballMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTrackballMotion.o: /usr/include/bits/stdio_lim.h
vsTrackballMotion.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTrackballMotion.o: ../../util/common/vsTreeMap.h++
vsTrackballMotion.o: ../../util/common/vsGrowableArray.h++
vsTrackballMotion.o: ../../io/common/vsMouse.h++
vsTrackballMotion.o: ../../io/common/vsInputDevice.h++
vsTrackballMotion.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsTrackballMotion.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTrackballMotion.o: /usr/include/bits/mathcalls.h
vsTrackballMotion.o: ../../io/common/vsInputAxis.h++
vsTrackballMotion.o: ../../io/common/vsInputButton.h++ vsKinematics.h++
vsTrackballMotion.o: ../../graphics/Performer/vsComponent.h++
vsTrackballMotion.o: /usr/include/Performer/pf/pfGroup.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfNode.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfUpdatable.h
vsTrackballMotion.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsTrackballMotion.o: /usr/include/sys/types.h /usr/include/time.h
vsTrackballMotion.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTrackballMotion.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsTrackballMotion.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsTrackballMotion.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsTrackballMotion.o: /usr/include/malloc.h /usr/include/limits.h
vsTrackballMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTrackballMotion.o: /usr/include/bits/posix1_lim.h
vsTrackballMotion.o: /usr/include/bits/local_lim.h
vsTrackballMotion.o: /usr/include/linux/limits.h
vsTrackballMotion.o: /usr/include/bits/posix2_lim.h
vsTrackballMotion.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsTrackballMotion.o: /usr/include/bits/posix_opt.h
vsTrackballMotion.o: /usr/include/bits/confname.h /usr/include/errno.h
vsTrackballMotion.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsTrackballMotion.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsTrackballMotion.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsTrackballMotion.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsTrackballMotion.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsTrackballMotion.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsTrackballMotion.o: /usr/include/Performer/opengl.h
vsTrackballMotion.o: /usr/include/Performer/linuxstubs.h
vsTrackballMotion.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsTrackballMotion.o: /usr/include/bits/waitflags.h
vsTrackballMotion.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsTrackballMotion.o: /usr/include/string.h /usr/include/sys/prctl.h
vsTrackballMotion.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsTrackballMotion.o: /usr/include/bits/wordsize.h
vsTrackballMotion.o: /usr/include/Performer/prmath.h
vsTrackballMotion.o: /usr/include/Performer/prstats.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfLinMath.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfObject.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfMemory.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfType.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfStruct.h
vsTrackballMotion.o: /usr/include/Performer/pfstats.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfBuffer.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfUpdate.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfLists.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfFrameStats.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfPipe.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfList.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfStats.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfDispList.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfGeoState.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfState.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfByteBank.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfGSetBank.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfGeoMath.h
vsTrackballMotion.o: ../../util/common/vsVector.h++
vsTrackballMotion.o: ../../graphics/Performer/vsAttribute.h++
vsTrackballMotion.o: ../../util/common/vsObjectMap.h++
vsTrackballMotion.o: ../../graphics/Performer/vsNode.h++
vsTrackballMotion.o: ../../util/common/vsMatrix.h++
vsTrackballMotion.o: ../../util/common/vsQuat.h++
vsTrackballMotion.o: ../../graphics/Performer/vsTransformAttribute.h++
vsTrackballMotion.o: /usr/include/Performer/pf/pfDCS.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfSCS.h
vsTrackballMotion.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsTrackballMotion.o: /usr/include/Performer/pfdb/pfflt.h
vsTrackballMotion.o: ../../graphics/Performer/vsGeometry.h++
vsTrackballMotion.o: /usr/include/Performer/pf/pfGeode.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfFlux.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfEngine.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfGeoSet.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfPassList.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfFBState.h
vsTrackballMotion.o: /usr/include/Performer/pr/pfWindow.h
vsTrackballMotion.o: ../../system/Performer/vsSystem.h++
vsTrackballMotion.o: ../../graphics/Performer_X/vsPipe.h++
vsTrackballMotion.o: ../../graphics/Performer_X/vsScreen.h++
vsTrackballMotion.o: ../../graphics/Performer_X/vsWindow.h++
vsTrackballMotion.o: /usr/include/Performer/pf/pfPipeWindow.h
vsTrackballMotion.o: ../../graphics/Performer_X/vsPane.h++
vsTrackballMotion.o: /usr/include/Performer/pf/pfChannel.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfEarthSky.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfLODState.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfScene.h
vsTrackballMotion.o: ../../graphics/Performer/vsView.h++
vsTrackballMotion.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsTrackballMotion.o: ../../graphics/Performer/vsScene.h++
vsTrackballMotion.o: ../../graphics/Performer/vsLightAttribute.h++
vsTrackballMotion.o: /usr/include/Performer/pr/pfLight.h
vsTrackballMotion.o: /usr/include/Performer/pf/pfLightSource.h
vsTrackballMotion.o: ../../util/common/vsTimer.h++
vsTrackedMotion.o: /usr/include/stdio.h /usr/include/features.h
vsTrackedMotion.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTrackedMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTrackedMotion.o: /usr/include/bits/types.h /usr/include/libio.h
vsTrackedMotion.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTrackedMotion.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTrackedMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTrackedMotion.o: /usr/include/bits/stdio_lim.h
vsTrackedMotion.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTrackedMotion.o: vsTrackedMotion.h++ vsMotionModel.h++
vsTrackedMotion.o: ../../util/common/vsUpdatable.h++
vsTrackedMotion.o: ../../util/common/vsObject.h++
vsTrackedMotion.o: ../../util/common/vsTreeMap.h++
vsTrackedMotion.o: ../../util/common/vsGrowableArray.h++
vsTrackedMotion.o: ../../io/common/vsMotionTracker.h++
vsTrackedMotion.o: ../../io/common/vs6DInputDevice.h++
vsTrackedMotion.o: ../../util/common/vsVector.h++
vsTrackedMotion.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsTrackedMotion.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTrackedMotion.o: /usr/include/bits/mathcalls.h
vsTrackedMotion.o: ../../util/common/vsMatrix.h++
vsTrackedMotion.o: ../../util/common/vsQuat.h++
vsTrackedMotion.o: ../../io/common/vsInputDevice.h++
vsTrackedMotion.o: ../../io/common/vsInputAxis.h++
vsTrackedMotion.o: ../../io/common/vsInputButton.h++ vsKinematics.h++
vsTrackedMotion.o: ../../graphics/Performer/vsComponent.h++
vsTrackedMotion.o: /usr/include/Performer/pf/pfGroup.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfNode.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfUpdatable.h
vsTrackedMotion.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsTrackedMotion.o: /usr/include/sys/types.h /usr/include/time.h
vsTrackedMotion.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTrackedMotion.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsTrackedMotion.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsTrackedMotion.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsTrackedMotion.o: /usr/include/malloc.h /usr/include/limits.h
vsTrackedMotion.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTrackedMotion.o: /usr/include/bits/posix1_lim.h
vsTrackedMotion.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsTrackedMotion.o: /usr/include/bits/posix2_lim.h
vsTrackedMotion.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsTrackedMotion.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsTrackedMotion.o: /usr/include/errno.h /usr/include/bits/errno.h
vsTrackedMotion.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsTrackedMotion.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsTrackedMotion.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsTrackedMotion.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsTrackedMotion.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsTrackedMotion.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsTrackedMotion.o: /usr/include/Performer/linuxstubs.h
vsTrackedMotion.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsTrackedMotion.o: /usr/include/bits/waitflags.h
vsTrackedMotion.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsTrackedMotion.o: /usr/include/string.h /usr/include/sys/prctl.h
vsTrackedMotion.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsTrackedMotion.o: /usr/include/bits/wordsize.h
vsTrackedMotion.o: /usr/include/Performer/prmath.h
vsTrackedMotion.o: /usr/include/Performer/prstats.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfLinMath.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfObject.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfMemory.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfType.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfStruct.h
vsTrackedMotion.o: /usr/include/Performer/pfstats.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfBuffer.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfUpdate.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfLists.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfFrameStats.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfPipe.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfVideoChannel.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfList.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfCalligraphic.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfStats.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfDispList.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfGeoState.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfState.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfByteBank.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfGSetBank.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfGeoMath.h
vsTrackedMotion.o: ../../graphics/Performer/vsAttribute.h++
vsTrackedMotion.o: ../../util/common/vsObjectMap.h++
vsTrackedMotion.o: ../../graphics/Performer/vsNode.h++
vsTrackedMotion.o: ../../graphics/Performer/vsTransformAttribute.h++
vsTrackedMotion.o: /usr/include/Performer/pf/pfDCS.h
vsTrackedMotion.o: /usr/include/Performer/pf/pfSCS.h
vsTrackedMotion.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsTrackedMotion.o: /usr/include/Performer/pfdb/pfflt.h
vsTrackedMotion.o: ../../graphics/Performer/vsGeometry.h++
vsTrackedMotion.o: /usr/include/Performer/pf/pfGeode.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfFlux.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfEngine.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfGeoSet.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfPassList.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfFBState.h
vsTrackedMotion.o: /usr/include/Performer/pr/pfWindow.h
vsWalkArticulation.o: /usr/include/stdio.h /usr/include/features.h
vsWalkArticulation.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWalkArticulation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWalkArticulation.o: /usr/include/bits/types.h /usr/include/libio.h
vsWalkArticulation.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWalkArticulation.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWalkArticulation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWalkArticulation.o: /usr/include/bits/stdio_lim.h
vsWalkArticulation.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWalkArticulation.o: ../../system/Performer/vsSystem.h++
vsWalkArticulation.o: ../../graphics/Performer_X/vsPipe.h++
vsWalkArticulation.o: /usr/include/Performer/pf/pfPipe.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfUpdatable.h
vsWalkArticulation.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsWalkArticulation.o: /usr/include/sys/types.h /usr/include/time.h
vsWalkArticulation.o: /usr/include/endian.h /usr/include/bits/endian.h
vsWalkArticulation.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsWalkArticulation.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsWalkArticulation.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsWalkArticulation.o: /usr/include/malloc.h /usr/include/limits.h
vsWalkArticulation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsWalkArticulation.o: /usr/include/bits/posix1_lim.h
vsWalkArticulation.o: /usr/include/bits/local_lim.h
vsWalkArticulation.o: /usr/include/linux/limits.h
vsWalkArticulation.o: /usr/include/bits/posix2_lim.h
vsWalkArticulation.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsWalkArticulation.o: /usr/include/bits/posix_opt.h
vsWalkArticulation.o: /usr/include/bits/confname.h /usr/include/errno.h
vsWalkArticulation.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsWalkArticulation.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsWalkArticulation.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsWalkArticulation.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsWalkArticulation.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsWalkArticulation.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsWalkArticulation.o: /usr/include/Performer/opengl.h
vsWalkArticulation.o: /usr/include/Performer/linuxstubs.h
vsWalkArticulation.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsWalkArticulation.o: /usr/include/bits/waitflags.h
vsWalkArticulation.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsWalkArticulation.o: /usr/include/string.h /usr/include/sys/prctl.h
vsWalkArticulation.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsWalkArticulation.o: /usr/include/bits/wordsize.h
vsWalkArticulation.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsWalkArticulation.o: /usr/include/bits/huge_val.h
vsWalkArticulation.o: /usr/include/bits/mathdef.h
vsWalkArticulation.o: /usr/include/bits/mathcalls.h
vsWalkArticulation.o: /usr/include/Performer/prstats.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfLinMath.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfObject.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfMemory.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfType.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfStruct.h
vsWalkArticulation.o: /usr/include/Performer/pfstats.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfBuffer.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfUpdate.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfLists.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfVideoChannel.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfList.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfCalligraphic.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfStats.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfDispList.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfGeoState.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfState.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfByteBank.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfGSetBank.h
vsWalkArticulation.o: ../../util/common/vsGlobals.h++
vsWalkArticulation.o: ../../graphics/Performer_X/vsScreen.h++
vsWalkArticulation.o: ../../graphics/Performer_X/vsWindow.h++
vsWalkArticulation.o: /usr/include/Performer/pf/pfPipeWindow.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfWindow.h
vsWalkArticulation.o: ../../graphics/Performer_X/vsPane.h++
vsWalkArticulation.o: /usr/include/Performer/pf/pfChannel.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfEarthSky.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfLODState.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfFrameStats.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfScene.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfGroup.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfNode.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfGeoMath.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfGeoSet.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfFlux.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfEngine.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfPassList.h
vsWalkArticulation.o: /usr/include/Performer/pr/pfFBState.h
vsWalkArticulation.o: ../../graphics/Performer/vsView.h++
vsWalkArticulation.o: ../../util/common/vsVector.h++
vsWalkArticulation.o: ../../util/common/vsMatrix.h++
vsWalkArticulation.o: ../../util/common/vsQuat.h++
vsWalkArticulation.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsWalkArticulation.o: ../../graphics/Performer/vsComponent.h++
vsWalkArticulation.o: ../../util/common/vsGrowableArray.h++
vsWalkArticulation.o: ../../graphics/Performer/vsAttribute.h++
vsWalkArticulation.o: ../../util/common/vsObject.h++
vsWalkArticulation.o: ../../util/common/vsTreeMap.h++
vsWalkArticulation.o: ../../util/common/vsObjectMap.h++
vsWalkArticulation.o: ../../graphics/Performer/vsNode.h++
vsWalkArticulation.o: ../../graphics/Performer/vsScene.h++
vsWalkArticulation.o: ../../graphics/Performer/vsLightAttribute.h++
vsWalkArticulation.o: /usr/include/Performer/pr/pfLight.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfLightSource.h
vsWalkArticulation.o: ../../util/common/vsTimer.h++ vsWalkArticulation.h++
vsWalkArticulation.o: vsMotionModel.h++ ../../util/common/vsUpdatable.h++
vsWalkArticulation.o: vsKinematics.h++
vsWalkArticulation.o: ../../graphics/Performer/vsTransformAttribute.h++
vsWalkArticulation.o: /usr/include/Performer/pf/pfDCS.h
vsWalkArticulation.o: /usr/include/Performer/pf/pfSCS.h
vsWalkArticulation.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsWalkArticulation.o: /usr/include/Performer/pfdb/pfflt.h
vsWalkArticulation.o: ../../graphics/Performer/vsGeometry.h++
vsWalkArticulation.o: /usr/include/Performer/pf/pfGeode.h
vsWalkInPlace.o: vsWalkInPlace.h++ vsMotionModel.h++
vsWalkInPlace.o: ../../util/common/vsUpdatable.h++
vsWalkInPlace.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsWalkInPlace.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsWalkInPlace.o: /usr/include/gnu/stubs.h
vsWalkInPlace.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWalkInPlace.o: /usr/include/bits/types.h /usr/include/libio.h
vsWalkInPlace.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWalkInPlace.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWalkInPlace.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWalkInPlace.o: /usr/include/bits/stdio_lim.h
vsWalkInPlace.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWalkInPlace.o: ../../util/common/vsTreeMap.h++
vsWalkInPlace.o: ../../util/common/vsGrowableArray.h++
vsWalkInPlace.o: ../../io/common/vsMotionTracker.h++
vsWalkInPlace.o: ../../io/common/vs6DInputDevice.h++
vsWalkInPlace.o: ../../util/common/vsVector.h++
vsWalkInPlace.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWalkInPlace.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsWalkInPlace.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsWalkInPlace.o: ../../util/common/vsQuat.h++
vsWalkInPlace.o: ../../io/common/vsInputDevice.h++
vsWalkInPlace.o: ../../io/common/vsInputAxis.h++
vsWalkInPlace.o: ../../io/common/vsInputButton.h++ vsKinematics.h++
vsWalkInPlace.o: ../../graphics/Performer/vsComponent.h++
vsWalkInPlace.o: /usr/include/Performer/pf/pfGroup.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfNode.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfUpdatable.h
vsWalkInPlace.o: /usr/include/Performer/pf.h /usr/include/Performer/pr.h
vsWalkInPlace.o: /usr/include/sys/types.h /usr/include/time.h
vsWalkInPlace.o: /usr/include/endian.h /usr/include/bits/endian.h
vsWalkInPlace.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsWalkInPlace.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsWalkInPlace.o: /usr/include/sys/sysmacros.h /usr/include/ulocks.h
vsWalkInPlace.o: /usr/include/malloc.h /usr/include/limits.h
vsWalkInPlace.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsWalkInPlace.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsWalkInPlace.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsWalkInPlace.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsWalkInPlace.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsWalkInPlace.o: /usr/include/errno.h /usr/include/bits/errno.h
vsWalkInPlace.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsWalkInPlace.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsWalkInPlace.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsWalkInPlace.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsWalkInPlace.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsWalkInPlace.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsWalkInPlace.o: /usr/include/Performer/linuxstubs.h
vsWalkInPlace.o: /usr/include/Performer/image.h /usr/include/stdlib.h
vsWalkInPlace.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsWalkInPlace.o: /usr/include/alloca.h /usr/include/string.h
vsWalkInPlace.o: /usr/include/sys/prctl.h /usr/include/linux/prctl.h
vsWalkInPlace.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsWalkInPlace.o: /usr/include/Performer/prmath.h
vsWalkInPlace.o: /usr/include/Performer/prstats.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfLinMath.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfObject.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfMemory.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfType.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfStruct.h
vsWalkInPlace.o: /usr/include/Performer/pfstats.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfBuffer.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfUpdate.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfLists.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfFrameStats.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfPipe.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfVideoChannel.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfList.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfCalligraphic.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfStats.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfDispList.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfGeoState.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfState.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfByteBank.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfGSetBank.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfGeoMath.h
vsWalkInPlace.o: ../../graphics/Performer/vsAttribute.h++
vsWalkInPlace.o: ../../util/common/vsObjectMap.h++
vsWalkInPlace.o: ../../graphics/Performer/vsNode.h++
vsWalkInPlace.o: ../../graphics/Performer/vsTransformAttribute.h++
vsWalkInPlace.o: /usr/include/Performer/pf/pfDCS.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfSCS.h
vsWalkInPlace.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsWalkInPlace.o: /usr/include/Performer/pfdb/pfflt.h
vsWalkInPlace.o: ../../graphics/Performer/vsGeometry.h++
vsWalkInPlace.o: /usr/include/Performer/pf/pfGeode.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfFlux.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfEngine.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfGeoSet.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfPassList.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfFBState.h
vsWalkInPlace.o: /usr/include/Performer/pr/pfWindow.h
vsWalkInPlace.o: ../../system/Performer/vsSystem.h++
vsWalkInPlace.o: ../../graphics/Performer_X/vsPipe.h++
vsWalkInPlace.o: ../../graphics/Performer_X/vsScreen.h++
vsWalkInPlace.o: ../../graphics/Performer_X/vsWindow.h++
vsWalkInPlace.o: /usr/include/Performer/pf/pfPipeWindow.h
vsWalkInPlace.o: ../../graphics/Performer_X/vsPane.h++
vsWalkInPlace.o: /usr/include/Performer/pf/pfChannel.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfEarthSky.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfLODState.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfScene.h
vsWalkInPlace.o: ../../graphics/Performer/vsView.h++
vsWalkInPlace.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsWalkInPlace.o: ../../graphics/Performer/vsScene.h++
vsWalkInPlace.o: ../../graphics/Performer/vsLightAttribute.h++
vsWalkInPlace.o: /usr/include/Performer/pr/pfLight.h
vsWalkInPlace.o: /usr/include/Performer/pf/pfLightSource.h
vsWalkInPlace.o: ../../util/common/vsTimer.h++
