#!/bin/make
# makefile for io/common

.SUFFIXES: .c .c++ .o .h .h++

CC_FLAGS=-g -O -ansi -D__linux__ -D_XOPEN_SOURCE -D_BSD_SOURCE -D_MISC_SOURCE -DPF_CPLUSPLUS_API=1

INCDIRS = -I. \
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
          vs6DInputDevice.o \
          vsArticulationGlove.o \
          vsChordGloves.o \
          vsCyberGloveBox.o \
          vsFlockOfBirds.o \
          vsISTJoystickBox.o \
          vsInputAxis.o \
          vsInputButton.o \
          vsInputDevice.o \
          vsInputSystem.o \
          vsJoystick.o \
          vsJoystickBox.o \
          vsKeyboard.o \
          vsMotionTracker.o \
          vsMouse.o \
          vsNetworkInterface.o \
          vsPinchGloveBox.o \
          vsSerialMotionStar.o \
          vsSharedInputData.o \
          vsSpaceball.o \
          vsTrackingSystem.o \
          vsUDPNetworkInterface.o \
          vsUDPUnicastNetworkInterface.o \
          vsUnwinder.o

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

vs6DInputDevice.o: vs6DInputDevice.h++ ../../util/common/vsVector.h++
vs6DInputDevice.o: /usr/include/stdio.h /usr/include/features.h
vs6DInputDevice.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vs6DInputDevice.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vs6DInputDevice.o: /usr/include/bits/types.h /usr/include/libio.h
vs6DInputDevice.o: /usr/include/_G_config.h /usr/include/wchar.h
vs6DInputDevice.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vs6DInputDevice.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vs6DInputDevice.o: /usr/include/bits/stdio_lim.h
vs6DInputDevice.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vs6DInputDevice.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vs6DInputDevice.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vs6DInputDevice.o: /usr/include/bits/mathcalls.h
vs6DInputDevice.o: ../../util/common/vsMatrix.h++
vs6DInputDevice.o: ../../util/common/vsQuat.h++ vsInputDevice.h++
vs6DInputDevice.o: vsInputAxis.h++ vsInputButton.h++ /usr/include/stdlib.h
vs6DInputDevice.o: /usr/include/bits/waitflags.h
vs6DInputDevice.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vs6DInputDevice.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsArticulationGlove.o: vsArticulationGlove.h++ vsInputDevice.h++
vsArticulationGlove.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsArticulationGlove.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsArticulationGlove.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsArticulationGlove.o: /usr/include/bits/mathdef.h
vsArticulationGlove.o: /usr/include/bits/mathcalls.h vsInputAxis.h++
vsArticulationGlove.o: vsInputButton.h++ ../../util/common/vsQuat.h++
vsArticulationGlove.o: ../../util/common/vsMatrix.h++ /usr/include/stdio.h
vsArticulationGlove.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsArticulationGlove.o: /usr/include/bits/types.h /usr/include/libio.h
vsArticulationGlove.o: /usr/include/_G_config.h /usr/include/wchar.h
vsArticulationGlove.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsArticulationGlove.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsArticulationGlove.o: /usr/include/bits/stdio_lim.h
vsArticulationGlove.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsArticulationGlove.o: ../../util/common/vsVector.h++
vsChordGloves.o: vsChordGloves.h++ ../../util/common/vsGlobals.h++
vsChordGloves.o: /usr/include/math.h /usr/include/features.h
vsChordGloves.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsChordGloves.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsChordGloves.o: /usr/include/bits/mathcalls.h vsInputDevice.h++
vsChordGloves.o: vsInputAxis.h++ vsInputButton.h++ /usr/include/stdio.h
vsChordGloves.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsChordGloves.o: /usr/include/bits/types.h /usr/include/libio.h
vsChordGloves.o: /usr/include/_G_config.h /usr/include/wchar.h
vsChordGloves.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsChordGloves.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsChordGloves.o: /usr/include/bits/stdio_lim.h
vsChordGloves.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsChordGloves.o: /usr/include/string.h
vsCyberGloveBox.o: vsCyberGloveBox.h++ vsArticulationGlove.h++
vsCyberGloveBox.o: vsInputDevice.h++ ../../util/common/vsGlobals.h++
vsCyberGloveBox.o: /usr/include/math.h /usr/include/features.h
vsCyberGloveBox.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsCyberGloveBox.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsCyberGloveBox.o: /usr/include/bits/mathcalls.h vsInputAxis.h++
vsCyberGloveBox.o: vsInputButton.h++ ../../util/common/vsQuat.h++
vsCyberGloveBox.o: ../../util/common/vsMatrix.h++ /usr/include/stdio.h
vsCyberGloveBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsCyberGloveBox.o: /usr/include/bits/types.h /usr/include/libio.h
vsCyberGloveBox.o: /usr/include/_G_config.h /usr/include/wchar.h
vsCyberGloveBox.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsCyberGloveBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsCyberGloveBox.o: /usr/include/bits/stdio_lim.h
vsCyberGloveBox.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsCyberGloveBox.o: ../../util/common/vsVector.h++ vsInputSystem.h++
vsCyberGloveBox.o: ../../util/common/vsUpdatable.h++
vsCyberGloveBox.o: ../../util/common/vsObject.h++
vsCyberGloveBox.o: ../../util/common/vsTreeMap.h++
vsCyberGloveBox.o: ../../util/common/vsGrowableArray.h++
vsFlockOfBirds.o: vsFlockOfBirds.h++
vsISTJoystickBox.o: vsISTJoystickBox.h++ vsJoystick.h++ vsInputDevice.h++
vsISTJoystickBox.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsISTJoystickBox.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsISTJoystickBox.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsISTJoystickBox.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsISTJoystickBox.o: vsInputAxis.h++ vsInputButton.h++ vsJoystickBox.h++
vsISTJoystickBox.o: vsInputSystem.h++ ../../util/common/vsUpdatable.h++
vsISTJoystickBox.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsISTJoystickBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsISTJoystickBox.o: /usr/include/bits/types.h /usr/include/libio.h
vsISTJoystickBox.o: /usr/include/_G_config.h /usr/include/wchar.h
vsISTJoystickBox.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsISTJoystickBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsISTJoystickBox.o: /usr/include/bits/stdio_lim.h
vsISTJoystickBox.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsISTJoystickBox.o: ../../util/common/vsTreeMap.h++
vsISTJoystickBox.o: ../../util/common/vsGrowableArray.h++
vsInputAxis.o: vsInputAxis.h++ ../../util/common/vsGlobals.h++
vsInputAxis.o: /usr/include/math.h /usr/include/features.h
vsInputAxis.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsInputAxis.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsInputAxis.o: /usr/include/bits/mathcalls.h /usr/include/stdio.h
vsInputAxis.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsInputAxis.o: /usr/include/bits/types.h /usr/include/libio.h
vsInputAxis.o: /usr/include/_G_config.h /usr/include/wchar.h
vsInputAxis.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsInputAxis.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsInputAxis.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsInputAxis.o: /usr/include/getopt.h /usr/include/string.h
vsInputButton.o: vsInputButton.h++ ../../util/common/vsGlobals.h++
vsInputButton.o: /usr/include/math.h /usr/include/features.h
vsInputButton.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsInputButton.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsInputButton.o: /usr/include/bits/mathcalls.h /usr/include/sys/time.h
vsInputButton.o: /usr/include/bits/types.h
vsInputButton.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsInputButton.o: /usr/include/time.h /usr/include/bits/time.h
vsInputButton.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsInputButton.o: /usr/include/bits/sigset.h /usr/include/stdio.h
vsInputButton.o: /usr/include/libio.h /usr/include/_G_config.h
vsInputButton.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsInputButton.o: /usr/include/gconv.h
vsInputButton.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsInputButton.o: /usr/include/bits/stdio_lim.h
vsInputButton.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsInputDevice.o: vsInputDevice.h++ ../../util/common/vsGlobals.h++
vsInputDevice.o: /usr/include/math.h /usr/include/features.h
vsInputDevice.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsInputDevice.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsInputDevice.o: /usr/include/bits/mathcalls.h vsInputAxis.h++
vsInputDevice.o: vsInputButton.h++
vsInputSystem.o: vsInputSystem.h++ ../../util/common/vsUpdatable.h++
vsInputSystem.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsInputSystem.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsInputSystem.o: /usr/include/gnu/stubs.h
vsInputSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsInputSystem.o: /usr/include/bits/types.h /usr/include/libio.h
vsInputSystem.o: /usr/include/_G_config.h /usr/include/wchar.h
vsInputSystem.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsInputSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsInputSystem.o: /usr/include/bits/stdio_lim.h
vsInputSystem.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsInputSystem.o: ../../util/common/vsTreeMap.h++
vsInputSystem.o: ../../util/common/vsGrowableArray.h++
vsJoystick.o: vsJoystick.h++ vsInputDevice.h++
vsJoystick.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsJoystick.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsJoystick.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsJoystick.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsJoystick.o: vsInputAxis.h++ vsInputButton.h++ /usr/include/stdio.h
vsJoystick.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsJoystick.o: /usr/include/bits/types.h /usr/include/libio.h
vsJoystick.o: /usr/include/_G_config.h /usr/include/wchar.h
vsJoystick.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsJoystick.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsJoystick.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsJoystick.o: /usr/include/getopt.h /usr/include/string.h
vsJoystickBox.o: vsJoystickBox.h++ vsInputSystem.h++
vsJoystickBox.o: ../../util/common/vsUpdatable.h++
vsJoystickBox.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsJoystickBox.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsJoystickBox.o: /usr/include/gnu/stubs.h
vsJoystickBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsJoystickBox.o: /usr/include/bits/types.h /usr/include/libio.h
vsJoystickBox.o: /usr/include/_G_config.h /usr/include/wchar.h
vsJoystickBox.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsJoystickBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsJoystickBox.o: /usr/include/bits/stdio_lim.h
vsJoystickBox.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsJoystickBox.o: ../../util/common/vsTreeMap.h++
vsJoystickBox.o: ../../util/common/vsGrowableArray.h++ vsJoystick.h++
vsJoystickBox.o: vsInputDevice.h++ ../../util/common/vsGlobals.h++
vsJoystickBox.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsJoystickBox.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsJoystickBox.o: vsInputAxis.h++ vsInputButton.h++
vsKeyboard.o: vsKeyboard.h++ vsInputDevice.h++
vsKeyboard.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsKeyboard.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsKeyboard.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
vsKeyboard.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsKeyboard.o: vsInputAxis.h++ vsInputButton.h++ /usr/include/X11/Xlib.h
vsKeyboard.o: /usr/include/sys/types.h /usr/include/bits/types.h
vsKeyboard.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsKeyboard.o: /usr/include/time.h /usr/include/endian.h
vsKeyboard.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsKeyboard.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsKeyboard.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsKeyboard.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
vsKeyboard.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
vsKeyboard.o: /usr/include/X11/Xos.h /usr/include/string.h
vsKeyboard.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsKeyboard.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
vsKeyboard.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
vsKeyboard.o: /usr/include/bits/confname.h /usr/include/getopt.h
vsKeyboard.o: /usr/include/sys/time.h /usr/include/X11/Xarch.h
vsKeyboard.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
vsKeyboard.o: /usr/include/stdio.h /usr/include/libio.h
vsKeyboard.o: /usr/include/_G_config.h /usr/include/wchar.h
vsKeyboard.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsKeyboard.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsKeyboard.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsKeyboard.o: /usr/include/ctype.h
vsMotionTracker.o: /usr/include/stdio.h /usr/include/features.h
vsMotionTracker.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsMotionTracker.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsMotionTracker.o: /usr/include/bits/types.h /usr/include/libio.h
vsMotionTracker.o: /usr/include/_G_config.h /usr/include/wchar.h
vsMotionTracker.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsMotionTracker.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsMotionTracker.o: /usr/include/bits/stdio_lim.h
vsMotionTracker.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsMotionTracker.o: vsMotionTracker.h++ vs6DInputDevice.h++
vsMotionTracker.o: ../../util/common/vsVector.h++
vsMotionTracker.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsMotionTracker.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsMotionTracker.o: /usr/include/bits/mathcalls.h
vsMotionTracker.o: ../../util/common/vsMatrix.h++
vsMotionTracker.o: ../../util/common/vsQuat.h++ vsInputDevice.h++
vsMotionTracker.o: vsInputAxis.h++ vsInputButton.h++
vsMouse.o: vsMouse.h++ vsInputDevice.h++ ../../util/common/vsGlobals.h++
vsMouse.o: /usr/include/math.h /usr/include/features.h
vsMouse.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsMouse.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsMouse.o: /usr/include/bits/mathcalls.h vsInputAxis.h++ vsInputButton.h++
vsMouse.o: /usr/include/stdio.h
vsMouse.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsMouse.o: /usr/include/bits/types.h /usr/include/libio.h
vsMouse.o: /usr/include/_G_config.h /usr/include/wchar.h
vsMouse.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsMouse.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsMouse.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsMouse.o: /usr/include/getopt.h
vsNetworkInterface.o: vsNetworkInterface.h++ /usr/include/stdio.h
vsNetworkInterface.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsNetworkInterface.o: /usr/include/gnu/stubs.h
vsNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsNetworkInterface.o: /usr/include/bits/types.h /usr/include/libio.h
vsNetworkInterface.o: /usr/include/_G_config.h /usr/include/wchar.h
vsNetworkInterface.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsNetworkInterface.o: /usr/include/bits/stdio_lim.h
vsNetworkInterface.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsNetworkInterface.o: /usr/include/sys/types.h /usr/include/time.h
vsNetworkInterface.o: /usr/include/endian.h /usr/include/bits/endian.h
vsNetworkInterface.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsNetworkInterface.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsNetworkInterface.o: /usr/include/sys/sysmacros.h /usr/include/sys/socket.h
vsNetworkInterface.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
vsNetworkInterface.o: /usr/include/bits/socket.h /usr/include/limits.h
vsNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsNetworkInterface.o: /usr/include/bits/posix1_lim.h
vsNetworkInterface.o: /usr/include/bits/local_lim.h
vsNetworkInterface.o: /usr/include/linux/limits.h
vsNetworkInterface.o: /usr/include/bits/posix2_lim.h
vsNetworkInterface.o: /usr/include/bits/xopen_lim.h
vsNetworkInterface.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
vsNetworkInterface.o: /usr/include/asm/sockios.h /usr/include/sys/time.h
vsNetworkInterface.o: /usr/include/netinet/in.h /usr/include/stdint.h
vsNetworkInterface.o: /usr/include/bits/wordsize.h /usr/include/bits/in.h
vsNetworkInterface.o: /usr/include/bits/byteswap.h /usr/include/string.h
vsNetworkInterface.o: /usr/include/errno.h /usr/include/bits/errno.h
vsNetworkInterface.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsPinchGloveBox.o: /usr/include/stdio.h /usr/include/features.h
vsPinchGloveBox.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsPinchGloveBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsPinchGloveBox.o: /usr/include/bits/types.h /usr/include/libio.h
vsPinchGloveBox.o: /usr/include/_G_config.h /usr/include/wchar.h
vsPinchGloveBox.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsPinchGloveBox.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsPinchGloveBox.o: /usr/include/bits/stdio_lim.h
vsPinchGloveBox.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsPinchGloveBox.o: /usr/include/string.h vsPinchGloveBox.h++
vsPinchGloveBox.o: vsInputSystem.h++ ../../util/common/vsUpdatable.h++
vsPinchGloveBox.o: ../../util/common/vsObject.h++
vsPinchGloveBox.o: ../../util/common/vsTreeMap.h++
vsPinchGloveBox.o: ../../util/common/vsGrowableArray.h++ vsChordGloves.h++
vsPinchGloveBox.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsPinchGloveBox.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsPinchGloveBox.o: /usr/include/bits/mathcalls.h vsInputDevice.h++
vsPinchGloveBox.o: vsInputAxis.h++ vsInputButton.h++
vsSerialMotionStar.o: vsSerialMotionStar.h++
vsSharedInputData.o: /usr/include/stdio.h /usr/include/features.h
vsSharedInputData.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSharedInputData.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSharedInputData.o: /usr/include/bits/types.h /usr/include/libio.h
vsSharedInputData.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSharedInputData.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSharedInputData.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSharedInputData.o: /usr/include/bits/stdio_lim.h
vsSharedInputData.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSharedInputData.o: vsSharedInputData.h++ /usr/include/sys/types.h
vsSharedInputData.o: /usr/include/time.h /usr/include/endian.h
vsSharedInputData.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsSharedInputData.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsSharedInputData.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsSharedInputData.o: /usr/include/sys/ipc.h /usr/include/bits/ipc.h
vsSharedInputData.o: /usr/include/sys/shm.h /usr/include/bits/shm.h
vsSharedInputData.o: /usr/include/sys/sem.h /usr/include/bits/sem.h
vsSharedInputData.o: ../../util/common/vsVector.h++
vsSharedInputData.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsSharedInputData.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsSharedInputData.o: /usr/include/bits/mathcalls.h
vsSharedInputData.o: ../../util/common/vsQuat.h++
vsSharedInputData.o: ../../util/common/vsMatrix.h++
vsSpaceball.o: vsSpaceball.h++ vs6DInputDevice.h++
vsSpaceball.o: ../../util/common/vsVector.h++ /usr/include/stdio.h
vsSpaceball.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsSpaceball.o: /usr/include/gnu/stubs.h
vsSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSpaceball.o: /usr/include/bits/types.h /usr/include/libio.h
vsSpaceball.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSpaceball.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSpaceball.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsSpaceball.o: /usr/include/getopt.h ../../util/common/vsGlobals.h++
vsSpaceball.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSpaceball.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsSpaceball.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsSpaceball.o: vsInputDevice.h++ vsInputAxis.h++ vsInputButton.h++
vsTrackingSystem.o: vsTrackingSystem.h++ vsMotionTracker.h++
vsTrackingSystem.o: vs6DInputDevice.h++ ../../util/common/vsVector.h++
vsTrackingSystem.o: /usr/include/stdio.h /usr/include/features.h
vsTrackingSystem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTrackingSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTrackingSystem.o: /usr/include/bits/types.h /usr/include/libio.h
vsTrackingSystem.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTrackingSystem.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTrackingSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTrackingSystem.o: /usr/include/bits/stdio_lim.h
vsTrackingSystem.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTrackingSystem.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsTrackingSystem.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTrackingSystem.o: /usr/include/bits/mathcalls.h
vsTrackingSystem.o: ../../util/common/vsMatrix.h++
vsTrackingSystem.o: ../../util/common/vsQuat.h++ vsInputDevice.h++
vsTrackingSystem.o: vsInputAxis.h++ vsInputButton.h++ vsInputSystem.h++
vsTrackingSystem.o: ../../util/common/vsUpdatable.h++
vsTrackingSystem.o: ../../util/common/vsObject.h++
vsTrackingSystem.o: ../../util/common/vsTreeMap.h++
vsTrackingSystem.o: ../../util/common/vsGrowableArray.h++
vsUDPNetworkInterface.o: vsUDPNetworkInterface.h++ /usr/include/unistd.h
vsUDPNetworkInterface.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsUDPNetworkInterface.o: /usr/include/gnu/stubs.h
vsUDPNetworkInterface.o: /usr/include/bits/posix_opt.h
vsUDPNetworkInterface.o: /usr/include/bits/types.h
vsUDPNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsUDPNetworkInterface.o: /usr/include/bits/confname.h /usr/include/getopt.h
vsUDPNetworkInterface.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsUDPNetworkInterface.o: /usr/include/sys/types.h /usr/include/time.h
vsUDPNetworkInterface.o: /usr/include/endian.h /usr/include/bits/endian.h
vsUDPNetworkInterface.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsUDPNetworkInterface.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsUDPNetworkInterface.o: /usr/include/sys/sysmacros.h /usr/include/sys/stat.h
vsUDPNetworkInterface.o: /usr/include/bits/stat.h vsNetworkInterface.h++
vsUDPNetworkInterface.o: /usr/include/stdio.h /usr/include/libio.h
vsUDPNetworkInterface.o: /usr/include/_G_config.h /usr/include/wchar.h
vsUDPNetworkInterface.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsUDPNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsUDPNetworkInterface.o: /usr/include/bits/stdio_lim.h
vsUDPNetworkInterface.o: /usr/include/bits/sys_errlist.h
vsUDPNetworkInterface.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
vsUDPNetworkInterface.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
vsUDPNetworkInterface.o: /usr/include/limits.h
vsUDPNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsUDPNetworkInterface.o: /usr/include/bits/posix1_lim.h
vsUDPNetworkInterface.o: /usr/include/bits/local_lim.h
vsUDPNetworkInterface.o: /usr/include/linux/limits.h
vsUDPNetworkInterface.o: /usr/include/bits/posix2_lim.h
vsUDPNetworkInterface.o: /usr/include/bits/xopen_lim.h
vsUDPNetworkInterface.o: /usr/include/bits/sockaddr.h
vsUDPNetworkInterface.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
vsUDPNetworkInterface.o: /usr/include/sys/time.h /usr/include/netinet/in.h
vsUDPNetworkInterface.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
vsUDPNetworkInterface.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
vsUDPNetworkInterface.o: /usr/include/string.h /usr/include/errno.h
vsUDPNetworkInterface.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsUDPNetworkInterface.o: /usr/include/asm/errno.h
vsUDPUnicastNetworkInterface.o: vsUDPUnicastNetworkInterface.h++
vsUDPUnicastNetworkInterface.o: /usr/include/sys/types.h
vsUDPUnicastNetworkInterface.o: /usr/include/features.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/cdefs.h
vsUDPUnicastNetworkInterface.o: /usr/include/gnu/stubs.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/types.h
vsUDPUnicastNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsUDPUnicastNetworkInterface.o: /usr/include/time.h /usr/include/endian.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/endian.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/select.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/select.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/sigset.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/time.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/sysmacros.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/socket.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/uio.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/uio.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/socket.h
vsUDPUnicastNetworkInterface.o: /usr/include/limits.h
vsUDPUnicastNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/posix1_lim.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/local_lim.h
vsUDPUnicastNetworkInterface.o: /usr/include/linux/limits.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/posix2_lim.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/xopen_lim.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/stdio_lim.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/sockaddr.h
vsUDPUnicastNetworkInterface.o: /usr/include/asm/socket.h
vsUDPUnicastNetworkInterface.o: /usr/include/asm/sockios.h
vsUDPUnicastNetworkInterface.o: /usr/include/netinet/in.h
vsUDPUnicastNetworkInterface.o: /usr/include/stdint.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/wchar.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/wordsize.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/in.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/byteswap.h
vsUDPUnicastNetworkInterface.o: /usr/include/arpa/inet.h /usr/include/netdb.h
vsUDPUnicastNetworkInterface.o: /usr/include/rpc/netdb.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/netdb.h
vsUDPUnicastNetworkInterface.o: vsUDPNetworkInterface.h++
vsUDPUnicastNetworkInterface.o: /usr/include/unistd.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/posix_opt.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/confname.h
vsUDPUnicastNetworkInterface.o: /usr/include/getopt.h /usr/include/fcntl.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/fcntl.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/stat.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/stat.h
vsUDPUnicastNetworkInterface.o: vsNetworkInterface.h++ /usr/include/stdio.h
vsUDPUnicastNetworkInterface.o: /usr/include/libio.h /usr/include/_G_config.h
vsUDPUnicastNetworkInterface.o: /usr/include/wchar.h /usr/include/gconv.h
vsUDPUnicastNetworkInterface.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/sys_errlist.h
vsUDPUnicastNetworkInterface.o: /usr/include/sys/time.h /usr/include/string.h
vsUDPUnicastNetworkInterface.o: /usr/include/errno.h
vsUDPUnicastNetworkInterface.o: /usr/include/bits/errno.h
vsUDPUnicastNetworkInterface.o: /usr/include/linux/errno.h
vsUDPUnicastNetworkInterface.o: /usr/include/asm/errno.h
vsUnwinder.o: vsUnwinder.h++ vsJoystickBox.h++ vsInputSystem.h++
vsUnwinder.o: ../../util/common/vsUpdatable.h++
vsUnwinder.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsUnwinder.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsUnwinder.o: /usr/include/gnu/stubs.h
vsUnwinder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsUnwinder.o: /usr/include/bits/types.h /usr/include/libio.h
vsUnwinder.o: /usr/include/_G_config.h /usr/include/wchar.h
vsUnwinder.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsUnwinder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsUnwinder.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsUnwinder.o: /usr/include/getopt.h ../../util/common/vsTreeMap.h++
vsUnwinder.o: ../../util/common/vsGrowableArray.h++ vsJoystick.h++
vsUnwinder.o: vsInputDevice.h++ ../../util/common/vsGlobals.h++
vsUnwinder.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsUnwinder.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsUnwinder.o: vsInputAxis.h++ vsInputButton.h++
