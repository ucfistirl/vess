#!/bin/make
# makefile for avatar/common

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../system/$(SG) \
	  -I../../motion/common \
	  -I../../io/common \
	  -I../../io/$(WS) \
	  -I../../io/$(SG)_$(WS) \
          -I../../io/$(OS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsAvatar.o \
	  vsConfigAvatar.o \
	  vsGhostFlyAvatar.o

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

vsAvatar.o: vsAvatar.h++ /usr/include/stdio.h /usr/include/features.h
vsAvatar.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsAvatar.o: /usr/include/bits/types.h /usr/include/libio.h
vsAvatar.o: /usr/include/_G_config.h /usr/include/wchar.h
vsAvatar.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsAvatar.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsAvatar.o: /usr/include/getopt.h ../../util/common/vsUpdatable.h++
vsAvatar.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsAvatar.o: ../../util/common/vsGrowableArray.h++
vsAvatar.o: ../../graphics/Performer/vsNode.h++
vsAvatar.o: ../../graphics/Performer/vsAttribute.h++ /usr/include/stdlib.h
vsAvatar.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsAvatar.o: /usr/include/endian.h /usr/include/bits/endian.h
vsAvatar.o: /usr/include/alloca.h /usr/include/Performer/pr/pfGeoState.h
vsAvatar.o: /usr/include/Performer/pr/pfObject.h
vsAvatar.o: /usr/include/Performer/pr/pfMemory.h /usr/include/sys/types.h
vsAvatar.o: /usr/include/time.h /usr/include/sys/select.h
vsAvatar.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsAvatar.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsAvatar.o: /usr/include/string.h /usr/include/Performer/pr.h
vsAvatar.o: /usr/include/ulocks.h /usr/include/malloc.h /usr/include/limits.h
vsAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsAvatar.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsAvatar.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsAvatar.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsAvatar.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsAvatar.o: /usr/include/errno.h /usr/include/bits/errno.h
vsAvatar.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsAvatar.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsAvatar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsAvatar.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsAvatar.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsAvatar.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsAvatar.o: /usr/include/Performer/linuxstubs.h
vsAvatar.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsAvatar.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsAvatar.o: /usr/include/bits/wordsize.h /usr/include/Performer/prmath.h
vsAvatar.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsAvatar.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsAvatar.o: /usr/include/Performer/prstats.h
vsAvatar.o: /usr/include/Performer/pr/pfType.h
vsAvatar.o: ../../util/common/vsGlobals.h++ ../../util/common/vsObjectMap.h++
vsAvatar.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsAvatar.o: ../../util/common/vsQuat.h++
vsAvatar.o: ../../graphics/common/vsOptimizer.h++
vsAvatar.o: ../../graphics/Performer/vsGeometry.h++
vsAvatar.o: /usr/include/Performer/pf/pfGeode.h
vsAvatar.o: /usr/include/Performer/pf/pfNode.h
vsAvatar.o: /usr/include/Performer/pf/pfUpdatable.h
vsAvatar.o: /usr/include/Performer/pf.h /usr/include/Performer/pr/pfLinMath.h
vsAvatar.o: /usr/include/Performer/pr/pfStruct.h
vsAvatar.o: /usr/include/Performer/pfstats.h
vsAvatar.o: /usr/include/Performer/pf/pfBuffer.h
vsAvatar.o: /usr/include/Performer/pf/pfUpdate.h
vsAvatar.o: /usr/include/Performer/pf/pfLists.h
vsAvatar.o: /usr/include/Performer/pf/pfFrameStats.h
vsAvatar.o: /usr/include/Performer/pf/pfPipe.h
vsAvatar.o: /usr/include/Performer/pr/pfVideoChannel.h
vsAvatar.o: /usr/include/Performer/pr/pfList.h
vsAvatar.o: /usr/include/Performer/pr/pfCalligraphic.h
vsAvatar.o: /usr/include/Performer/pr/pfStats.h
vsAvatar.o: /usr/include/Performer/pr/pfDispList.h
vsAvatar.o: /usr/include/Performer/pr/pfState.h
vsAvatar.o: /usr/include/Performer/pr/pfByteBank.h
vsAvatar.o: /usr/include/Performer/pr/pfGSetBank.h
vsAvatar.o: /usr/include/Performer/pr/pfGeoMath.h
vsAvatar.o: /usr/include/Performer/pr/pfFlux.h
vsAvatar.o: /usr/include/Performer/pr/pfEngine.h
vsAvatar.o: /usr/include/Performer/pr/pfGeoSet.h
vsAvatar.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsAvatar.o: /usr/include/Performer/pr/pfPassList.h
vsAvatar.o: /usr/include/Performer/pr/pfFBState.h
vsAvatar.o: /usr/include/Performer/pr/pfWindow.h
vsAvatar.o: ../../graphics/Performer/vsComponent.h++
vsAvatar.o: /usr/include/Performer/pf/pfGroup.h
vsAvatar.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsAvatar.o: /usr/include/Performer/pfdb/pfflt.h
vsAvatar.o: ../../io/common/vsISTJoystickBox.h++
vsAvatar.o: ../../io/common/vsJoystick.h++ ../../io/common/vsInputDevice.h++
vsAvatar.o: ../../io/common/vsInputAxis.h++ ../../io/common/vsInputButton.h++
vsAvatar.o: ../../io/common/vsJoystickBox.h++
vsAvatar.o: ../../io/common/vsInputSystem.h++ ../../io/X/vsSerialPort.h++
vsAvatar.o: /usr/include/termios.h /usr/include/bits/termios.h
vsAvatar.o: /usr/include/sys/ttydefaults.h ../../io/common/vsUnwinder.h++
vsAvatar.o: ../../io/common/vsFlockOfBirds.h++
vsAvatar.o: ../../io/X/vsAscensionSerialTrackingSystem.h++
vsAvatar.o: ../../io/common/vsMotionTracker.h++
vsAvatar.o: ../../io/common/vs6DInputDevice.h++
vsAvatar.o: ../../io/common/vsTrackingSystem.h++
vsAvatar.o: ../../io/common/vsSharedInputData.h++ /usr/include/sys/ipc.h
vsAvatar.o: /usr/include/bits/ipc.h /usr/include/sys/shm.h
vsAvatar.o: /usr/include/bits/shm.h /usr/include/sys/sem.h
vsAvatar.o: /usr/include/bits/sem.h ../../io/common/vsSerialMotionStar.h++
vsAvatar.o: ../../io/X/vsFastrak.h++ ../../io/X/vsIS600.h++
vsAvatar.o: ../../io/X/vsEthernetMotionStar.h++
vsAvatar.o: ../../io/common/vsUDPUnicastNetworkInterface.h++
vsAvatar.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
vsAvatar.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
vsAvatar.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
vsAvatar.o: /usr/include/asm/sockios.h /usr/include/netinet/in.h
vsAvatar.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
vsAvatar.o: /usr/include/arpa/inet.h /usr/include/netdb.h
vsAvatar.o: /usr/include/rpc/netdb.h /usr/include/bits/netdb.h
vsAvatar.o: ../../io/common/vsUDPNetworkInterface.h++ /usr/include/fcntl.h
vsAvatar.o: /usr/include/bits/fcntl.h /usr/include/sys/stat.h
vsAvatar.o: /usr/include/bits/stat.h ../../io/common/vsNetworkInterface.h++
vsAvatar.o: /usr/include/sys/time.h ../../io/X/vsWSSpaceball.h++
vsAvatar.o: ../../io/common/vsSpaceball.h++
vsAvatar.o: ../../io/Performer_X/vsWindowSystem.h++ /usr/include/X11/Xos.h
vsAvatar.o: /usr/include/X11/Xarch.h ../../graphics/Performer_X/vsWindow.h++
vsAvatar.o: /usr/include/Performer/pf/pfPipeWindow.h
vsAvatar.o: ../../graphics/Performer_X/vsScreen.h++
vsAvatar.o: ../../graphics/Performer_X/vsPipe.h++
vsAvatar.o: ../../graphics/Performer_X/vsPane.h++
vsAvatar.o: /usr/include/Performer/pf/pfChannel.h
vsAvatar.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsAvatar.o: /usr/include/Performer/pf/pfEarthSky.h
vsAvatar.o: /usr/include/Performer/pf/pfLODState.h
vsAvatar.o: /usr/include/Performer/pf/pfScene.h
vsAvatar.o: ../../graphics/Performer/vsView.h++
vsAvatar.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsAvatar.o: ../../graphics/Performer/vsScene.h++
vsAvatar.o: ../../graphics/Performer/vsLightAttribute.h++
vsAvatar.o: /usr/include/Performer/pr/pfLight.h
vsAvatar.o: /usr/include/Performer/pf/pfLightSource.h
vsAvatar.o: ../../io/common/vsMouse.h++ ../../io/common/vsKeyboard.h++
vsAvatar.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
vsAvatar.o: /usr/include/X11/extensions/XI.h
vsAvatar.o: /usr/include/X11/extensions/XInput.h
vsAvatar.o: ../../io/common/vsPinchGloveBox.h++
vsAvatar.o: ../../io/common/vsChordGloves.h++
vsAvatar.o: ../../io/common/vsCyberGloveBox.h++
vsAvatar.o: ../../io/common/vsArticulationGlove.h++
vsAvatar.o: ../../motion/common/vs3TrackerArm.h++
vsAvatar.o: ../../motion/common/vsMotionModel.h++
vsAvatar.o: ../../motion/common/vsKinematics.h++
vsAvatar.o: ../../graphics/Performer/vsTransformAttribute.h++
vsAvatar.o: /usr/include/Performer/pf/pfDCS.h
vsAvatar.o: /usr/include/Performer/pf/pfSCS.h
vsAvatar.o: ../../motion/common/vsCollision.h++
vsAvatar.o: ../../graphics/Performer/vsIntersect.h++
vsAvatar.o: ../../motion/common/vsDrivingMotion.h++
vsAvatar.o: ../../motion/common/vsFlyingMotion.h++
vsAvatar.o: ../../motion/common/vsDifferentialTrackedOrientation.h++
vsAvatar.o: ../../motion/common/vsTerrainFollow.h++
vsAvatar.o: ../../motion/common/vsTrackballMotion.h++
vsAvatar.o: ../../motion/common/vsTrackedMotion.h++
vsAvatar.o: ../../motion/common/vsWalkArticulation.h++
vsAvatar.o: ../../motion/common/vsWalkInPlace.h++
vsAvatar.o: ../../io/Linux/vsLinuxJoystickSystem.h++
vsAvatar.o: /usr/include/linux/joystick.h /usr/include/asm/types.h
vsAvatar.o: /usr/include/linux/input.h /usr/include/sys/ioctl.h
vsAvatar.o: /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h
vsAvatar.o: /usr/include/asm/ioctl.h /usr/include/bits/ioctl-types.h
vsConfigAvatar.o: vsConfigAvatar.h++ vsAvatar.h++ /usr/include/stdio.h
vsConfigAvatar.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsConfigAvatar.o: /usr/include/gnu/stubs.h
vsConfigAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsConfigAvatar.o: /usr/include/bits/types.h /usr/include/libio.h
vsConfigAvatar.o: /usr/include/_G_config.h /usr/include/wchar.h
vsConfigAvatar.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsConfigAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsConfigAvatar.o: /usr/include/bits/stdio_lim.h
vsConfigAvatar.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsConfigAvatar.o: ../../util/common/vsUpdatable.h++
vsConfigAvatar.o: ../../util/common/vsObject.h++
vsConfigAvatar.o: ../../util/common/vsTreeMap.h++
vsConfigAvatar.o: ../../util/common/vsGrowableArray.h++
vsConfigAvatar.o: ../../graphics/Performer/vsNode.h++
vsConfigAvatar.o: ../../graphics/Performer/vsAttribute.h++
vsConfigAvatar.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsConfigAvatar.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsConfigAvatar.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfGeoState.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfObject.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfMemory.h
vsConfigAvatar.o: /usr/include/sys/types.h /usr/include/time.h
vsConfigAvatar.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsConfigAvatar.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsConfigAvatar.o: /usr/include/sys/sysmacros.h /usr/include/string.h
vsConfigAvatar.o: /usr/include/Performer/pr.h /usr/include/ulocks.h
vsConfigAvatar.o: /usr/include/malloc.h /usr/include/limits.h
vsConfigAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsConfigAvatar.o: /usr/include/bits/posix1_lim.h
vsConfigAvatar.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsConfigAvatar.o: /usr/include/bits/posix2_lim.h
vsConfigAvatar.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsConfigAvatar.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsConfigAvatar.o: /usr/include/errno.h /usr/include/bits/errno.h
vsConfigAvatar.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsConfigAvatar.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
vsConfigAvatar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsConfigAvatar.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsConfigAvatar.o: /usr/include/GL/glx.h /usr/include/GL/glxtokens.h
vsConfigAvatar.o: /usr/include/GL/glu.h /usr/include/Performer/opengl.h
vsConfigAvatar.o: /usr/include/Performer/linuxstubs.h
vsConfigAvatar.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsConfigAvatar.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsConfigAvatar.o: /usr/include/bits/wordsize.h
vsConfigAvatar.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsConfigAvatar.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsConfigAvatar.o: /usr/include/bits/mathcalls.h
vsConfigAvatar.o: /usr/include/Performer/prstats.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfType.h
vsConfigAvatar.o: ../../util/common/vsGlobals.h++
vsConfigAvatar.o: ../../util/common/vsObjectMap.h++
vsConfigAvatar.o: ../../util/common/vsVector.h++
vsConfigAvatar.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsConfigAvatar.o: ../../graphics/common/vsOptimizer.h++
vsConfigAvatar.o: ../../graphics/Performer/vsGeometry.h++
vsConfigAvatar.o: /usr/include/Performer/pf/pfGeode.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfNode.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfUpdatable.h
vsConfigAvatar.o: /usr/include/Performer/pf.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfLinMath.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfStruct.h
vsConfigAvatar.o: /usr/include/Performer/pfstats.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfBuffer.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfUpdate.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfLists.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfFrameStats.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfPipe.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfVideoChannel.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfList.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfCalligraphic.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfStats.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfDispList.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfState.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfByteBank.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfGSetBank.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfGeoMath.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfFlux.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfEngine.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfGeoSet.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfPassList.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfFBState.h
vsConfigAvatar.o: /usr/include/Performer/pr/pfWindow.h
vsConfigAvatar.o: ../../graphics/Performer/vsComponent.h++
vsConfigAvatar.o: /usr/include/Performer/pf/pfGroup.h
vsConfigAvatar.o: ../../motion/common/vsKinematics.h++
vsConfigAvatar.o: ../../graphics/Performer/vsTransformAttribute.h++
vsConfigAvatar.o: /usr/include/Performer/pf/pfDCS.h
vsConfigAvatar.o: /usr/include/Performer/pf/pfSCS.h
vsConfigAvatar.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsConfigAvatar.o: /usr/include/Performer/pfdb/pfflt.h
vsGhostFlyAvatar.o: vsGhostFlyAvatar.h++ vsAvatar.h++ /usr/include/stdio.h
vsGhostFlyAvatar.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsGhostFlyAvatar.o: /usr/include/gnu/stubs.h
vsGhostFlyAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsGhostFlyAvatar.o: /usr/include/bits/types.h /usr/include/libio.h
vsGhostFlyAvatar.o: /usr/include/_G_config.h /usr/include/wchar.h
vsGhostFlyAvatar.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsGhostFlyAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsGhostFlyAvatar.o: /usr/include/bits/stdio_lim.h
vsGhostFlyAvatar.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsGhostFlyAvatar.o: ../../util/common/vsUpdatable.h++
vsGhostFlyAvatar.o: ../../util/common/vsObject.h++
vsGhostFlyAvatar.o: ../../util/common/vsTreeMap.h++
vsGhostFlyAvatar.o: ../../util/common/vsGrowableArray.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsNode.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsAttribute.h++
vsGhostFlyAvatar.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsGhostFlyAvatar.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsGhostFlyAvatar.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfGeoState.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfObject.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfMemory.h
vsGhostFlyAvatar.o: /usr/include/sys/types.h /usr/include/time.h
vsGhostFlyAvatar.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsGhostFlyAvatar.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsGhostFlyAvatar.o: /usr/include/sys/sysmacros.h /usr/include/string.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr.h /usr/include/ulocks.h
vsGhostFlyAvatar.o: /usr/include/malloc.h /usr/include/limits.h
vsGhostFlyAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsGhostFlyAvatar.o: /usr/include/bits/posix1_lim.h
vsGhostFlyAvatar.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
vsGhostFlyAvatar.o: /usr/include/bits/posix2_lim.h
vsGhostFlyAvatar.o: /usr/include/bits/xopen_lim.h /usr/include/unistd.h
vsGhostFlyAvatar.o: /usr/include/bits/posix_opt.h
vsGhostFlyAvatar.o: /usr/include/bits/confname.h /usr/include/errno.h
vsGhostFlyAvatar.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsGhostFlyAvatar.o: /usr/include/asm/errno.h /usr/include/X11/X.h
vsGhostFlyAvatar.o: /usr/include/X11/Xlib.h /usr/include/X11/Xfuncproto.h
vsGhostFlyAvatar.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsGhostFlyAvatar.o: /usr/include/GL/gl.h /usr/include/GL/glx.h
vsGhostFlyAvatar.o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
vsGhostFlyAvatar.o: /usr/include/Performer/opengl.h
vsGhostFlyAvatar.o: /usr/include/Performer/linuxstubs.h
vsGhostFlyAvatar.o: /usr/include/Performer/image.h /usr/include/sys/prctl.h
vsGhostFlyAvatar.o: /usr/include/linux/prctl.h /usr/include/stdint.h
vsGhostFlyAvatar.o: /usr/include/bits/wordsize.h
vsGhostFlyAvatar.o: /usr/include/Performer/prmath.h /usr/include/math.h
vsGhostFlyAvatar.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsGhostFlyAvatar.o: /usr/include/bits/mathcalls.h
vsGhostFlyAvatar.o: /usr/include/Performer/prstats.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfType.h
vsGhostFlyAvatar.o: ../../util/common/vsGlobals.h++
vsGhostFlyAvatar.o: ../../util/common/vsObjectMap.h++
vsGhostFlyAvatar.o: ../../util/common/vsVector.h++
vsGhostFlyAvatar.o: ../../util/common/vsMatrix.h++
vsGhostFlyAvatar.o: ../../util/common/vsQuat.h++
vsGhostFlyAvatar.o: ../../graphics/common/vsOptimizer.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsGeometry.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfGeode.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfNode.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfUpdatable.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfLinMath.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfStruct.h
vsGhostFlyAvatar.o: /usr/include/Performer/pfstats.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfBuffer.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfUpdate.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfLists.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfFrameStats.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfPipe.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfVideoChannel.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfList.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfCalligraphic.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfStats.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfDispList.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfState.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfByteBank.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfGSetBank.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfGeoMath.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfFlux.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfEngine.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfGeoSet.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfCycleBuffer.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfPassList.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfFBState.h
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfWindow.h
vsGhostFlyAvatar.o: ../../graphics/Performer/vsComponent.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfGroup.h
vsGhostFlyAvatar.o: ../../io/common/vsMouse.h++
vsGhostFlyAvatar.o: ../../io/common/vsInputDevice.h++
vsGhostFlyAvatar.o: ../../io/common/vsInputAxis.h++
vsGhostFlyAvatar.o: ../../io/common/vsInputButton.h++
vsGhostFlyAvatar.o: ../../motion/common/vsKinematics.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsTransformAttribute.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfDCS.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfSCS.h
vsGhostFlyAvatar.o: ../../graphics/Performer/vsDatabaseLoader.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pfdb/pfflt.h
vsGhostFlyAvatar.o: ../../motion/common/vsFlyingMotion.h++
vsGhostFlyAvatar.o: ../../motion/common/vsMotionModel.h++
vsGhostFlyAvatar.o: ../../io/Performer_X/vsWindowSystem.h++
vsGhostFlyAvatar.o: /usr/include/X11/Xos.h /usr/include/fcntl.h
vsGhostFlyAvatar.o: /usr/include/bits/fcntl.h /usr/include/sys/stat.h
vsGhostFlyAvatar.o: /usr/include/bits/stat.h /usr/include/sys/time.h
vsGhostFlyAvatar.o: /usr/include/X11/Xarch.h
vsGhostFlyAvatar.o: ../../io/common/vsInputSystem.h++
vsGhostFlyAvatar.o: ../../graphics/Performer_X/vsWindow.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfPipeWindow.h
vsGhostFlyAvatar.o: ../../graphics/Performer_X/vsScreen.h++
vsGhostFlyAvatar.o: ../../graphics/Performer_X/vsPipe.h++
vsGhostFlyAvatar.o: ../../graphics/Performer_X/vsPane.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfChannel.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfPipeVideoChannel.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfEarthSky.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfLODState.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfScene.h
vsGhostFlyAvatar.o: ../../graphics/Performer/vsView.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsViewpointAttribute.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsScene.h++
vsGhostFlyAvatar.o: ../../graphics/Performer/vsLightAttribute.h++
vsGhostFlyAvatar.o: /usr/include/Performer/pr/pfLight.h
vsGhostFlyAvatar.o: /usr/include/Performer/pf/pfLightSource.h
vsGhostFlyAvatar.o: ../../io/common/vsKeyboard.h++ /usr/include/X11/keysym.h
vsGhostFlyAvatar.o: /usr/include/X11/keysymdef.h
