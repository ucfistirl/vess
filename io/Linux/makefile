#!/bin/make
# makefile for io/Linux

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../system/$(SG) \
	  -I../../avatar/common \
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
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsAscensionSerialTrackingSystem.o \
          vsEthernetMotionStar.o \
          vsFastrak.o \
          vsIS600.o \
          vsLinuxJoystickSystem.o \
          vsSerialPort.o

LIBSRCS = $(LIBOBJS:.o=.c++)


all: $(LIBOBJS)

dep: depend
depend:
	makedepend -- $(CC_FLAGS) $(INCDIRS) -- $(LIBSRCS)

clean:
	@rm -f *.o makefile.bak

.c++.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj

.c.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj
# DO NOT DELETE

vsAscensionSerialTrackingSystem.o: /usr/include/stdio.h
vsAscensionSerialTrackingSystem.o: /usr/include/features.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/cdefs.h
vsAscensionSerialTrackingSystem.o: /usr/include/gnu/stubs.h
vsAscensionSerialTrackingSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/types.h
vsAscensionSerialTrackingSystem.o: /usr/include/libio.h
vsAscensionSerialTrackingSystem.o: /usr/include/_G_config.h
vsAscensionSerialTrackingSystem.o: /usr/include/wchar.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/wchar.h
vsAscensionSerialTrackingSystem.o: /usr/include/gconv.h
vsAscensionSerialTrackingSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/stdio_lim.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/sys_errlist.h
vsAscensionSerialTrackingSystem.o: /usr/include/getopt.h
vsAscensionSerialTrackingSystem.o: /usr/include/signal.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/sigset.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/signum.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/sigaction.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/sigcontext.h
vsAscensionSerialTrackingSystem.o: /usr/include/asm/sigcontext.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/sigstack.h
vsAscensionSerialTrackingSystem.o: /usr/include/ucontext.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/ucontext.h
vsAscensionSerialTrackingSystem.o: /usr/include/unistd.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/posix_opt.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/confname.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/types.h
vsAscensionSerialTrackingSystem.o: /usr/include/time.h /usr/include/endian.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/endian.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/select.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/select.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/time.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/sysmacros.h
vsAscensionSerialTrackingSystem.o: /usr/include/string.h
vsAscensionSerialTrackingSystem.o: /usr/include/stdlib.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/waitflags.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/waitstatus.h
vsAscensionSerialTrackingSystem.o: /usr/include/alloca.h
vsAscensionSerialTrackingSystem.o: vsAscensionSerialTrackingSystem.h++
vsAscensionSerialTrackingSystem.o: vsSerialPort.h++ /usr/include/termios.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/termios.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/ttydefaults.h
vsAscensionSerialTrackingSystem.o: ../../io/common/vsMotionTracker.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vs6DInputDevice.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsVector.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsGlobals.h++
vsAscensionSerialTrackingSystem.o: /usr/include/math.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/huge_val.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/mathdef.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/mathcalls.h
vsAscensionSerialTrackingSystem.o: ../../util/common/vsMatrix.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsQuat.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vsInputDevice.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vsInputAxis.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vsInputButton.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vsTrackingSystem.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vsInputSystem.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsUpdatable.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsObject.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsTreeMap.h++
vsAscensionSerialTrackingSystem.o: ../../util/common/vsGrowableArray.h++
vsAscensionSerialTrackingSystem.o: ../../io/common/vsSharedInputData.h++
vsAscensionSerialTrackingSystem.o: /usr/include/sys/ipc.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/ipc.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/shm.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/shm.h
vsAscensionSerialTrackingSystem.o: /usr/include/sys/sem.h
vsAscensionSerialTrackingSystem.o: /usr/include/bits/sem.h
vsEthernetMotionStar.o: /usr/include/stdio.h /usr/include/features.h
vsEthernetMotionStar.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsEthernetMotionStar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsEthernetMotionStar.o: /usr/include/bits/types.h /usr/include/libio.h
vsEthernetMotionStar.o: /usr/include/_G_config.h /usr/include/wchar.h
vsEthernetMotionStar.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsEthernetMotionStar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsEthernetMotionStar.o: /usr/include/bits/stdio_lim.h
vsEthernetMotionStar.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsEthernetMotionStar.o: /usr/include/signal.h /usr/include/bits/sigset.h
vsEthernetMotionStar.o: /usr/include/bits/signum.h
vsEthernetMotionStar.o: /usr/include/bits/sigaction.h
vsEthernetMotionStar.o: /usr/include/bits/sigcontext.h
vsEthernetMotionStar.o: /usr/include/asm/sigcontext.h
vsEthernetMotionStar.o: /usr/include/bits/sigstack.h /usr/include/ucontext.h
vsEthernetMotionStar.o: /usr/include/sys/ucontext.h /usr/include/stdlib.h
vsEthernetMotionStar.o: /usr/include/bits/waitflags.h
vsEthernetMotionStar.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsEthernetMotionStar.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsEthernetMotionStar.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
vsEthernetMotionStar.o: /usr/include/bits/confname.h /usr/include/time.h
vsEthernetMotionStar.o: vsEthernetMotionStar.h++
vsEthernetMotionStar.o: ../../io/common/vsMotionTracker.h++
vsEthernetMotionStar.o: ../../io/common/vs6DInputDevice.h++
vsEthernetMotionStar.o: ../../util/common/vsVector.h++
vsEthernetMotionStar.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsEthernetMotionStar.o: /usr/include/bits/huge_val.h
vsEthernetMotionStar.o: /usr/include/bits/mathdef.h
vsEthernetMotionStar.o: /usr/include/bits/mathcalls.h
vsEthernetMotionStar.o: ../../util/common/vsMatrix.h++
vsEthernetMotionStar.o: ../../util/common/vsQuat.h++
vsEthernetMotionStar.o: ../../io/common/vsInputDevice.h++
vsEthernetMotionStar.o: ../../io/common/vsInputAxis.h++
vsEthernetMotionStar.o: ../../io/common/vsInputButton.h++
vsEthernetMotionStar.o: ../../io/common/vsTrackingSystem.h++
vsEthernetMotionStar.o: ../../io/common/vsInputSystem.h++
vsEthernetMotionStar.o: ../../util/common/vsUpdatable.h++
vsEthernetMotionStar.o: ../../util/common/vsObject.h++
vsEthernetMotionStar.o: ../../util/common/vsTreeMap.h++
vsEthernetMotionStar.o: ../../util/common/vsGrowableArray.h++
vsEthernetMotionStar.o: ../../io/common/vsSharedInputData.h++
vsEthernetMotionStar.o: /usr/include/sys/types.h /usr/include/sys/select.h
vsEthernetMotionStar.o: /usr/include/bits/select.h /usr/include/bits/time.h
vsEthernetMotionStar.o: /usr/include/sys/sysmacros.h /usr/include/sys/ipc.h
vsEthernetMotionStar.o: /usr/include/bits/ipc.h /usr/include/sys/shm.h
vsEthernetMotionStar.o: /usr/include/bits/shm.h /usr/include/sys/sem.h
vsEthernetMotionStar.o: /usr/include/bits/sem.h
vsEthernetMotionStar.o: ../../io/common/vsUDPUnicastNetworkInterface.h++
vsEthernetMotionStar.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
vsEthernetMotionStar.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
vsEthernetMotionStar.o: /usr/include/limits.h
vsEthernetMotionStar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsEthernetMotionStar.o: /usr/include/bits/posix1_lim.h
vsEthernetMotionStar.o: /usr/include/bits/local_lim.h
vsEthernetMotionStar.o: /usr/include/linux/limits.h
vsEthernetMotionStar.o: /usr/include/bits/posix2_lim.h
vsEthernetMotionStar.o: /usr/include/bits/xopen_lim.h
vsEthernetMotionStar.o: /usr/include/bits/sockaddr.h
vsEthernetMotionStar.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
vsEthernetMotionStar.o: /usr/include/netinet/in.h /usr/include/stdint.h
vsEthernetMotionStar.o: /usr/include/bits/wordsize.h /usr/include/bits/in.h
vsEthernetMotionStar.o: /usr/include/bits/byteswap.h /usr/include/arpa/inet.h
vsEthernetMotionStar.o: /usr/include/netdb.h /usr/include/rpc/netdb.h
vsEthernetMotionStar.o: /usr/include/bits/netdb.h
vsEthernetMotionStar.o: ../../io/common/vsUDPNetworkInterface.h++
vsEthernetMotionStar.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsEthernetMotionStar.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
vsEthernetMotionStar.o: ../../io/common/vsNetworkInterface.h++
vsEthernetMotionStar.o: /usr/include/sys/time.h /usr/include/string.h
vsEthernetMotionStar.o: /usr/include/errno.h /usr/include/bits/errno.h
vsEthernetMotionStar.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
vsFastrak.o: vsFastrak.h++ vsSerialPort.h++ /usr/include/termios.h
vsFastrak.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsFastrak.o: /usr/include/gnu/stubs.h /usr/include/bits/termios.h
vsFastrak.o: /usr/include/sys/ttydefaults.h /usr/include/unistd.h
vsFastrak.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
vsFastrak.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsFastrak.o: /usr/include/bits/confname.h /usr/include/getopt.h
vsFastrak.o: ../../io/common/vsMotionTracker.h++
vsFastrak.o: ../../io/common/vs6DInputDevice.h++
vsFastrak.o: ../../util/common/vsVector.h++ /usr/include/stdio.h
vsFastrak.o: /usr/include/libio.h /usr/include/_G_config.h
vsFastrak.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsFastrak.o: /usr/include/gconv.h
vsFastrak.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsFastrak.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsFastrak.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsFastrak.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsFastrak.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsFastrak.o: ../../util/common/vsQuat.h++ ../../io/common/vsInputDevice.h++
vsFastrak.o: ../../io/common/vsInputAxis.h++
vsFastrak.o: ../../io/common/vsInputButton.h++
vsFastrak.o: ../../io/common/vsTrackingSystem.h++
vsFastrak.o: ../../io/common/vsInputSystem.h++
vsFastrak.o: ../../util/common/vsUpdatable.h++ ../../util/common/vsObject.h++
vsFastrak.o: ../../util/common/vsTreeMap.h++
vsFastrak.o: ../../util/common/vsGrowableArray.h++
vsFastrak.o: ../../io/common/vsSharedInputData.h++ /usr/include/sys/types.h
vsFastrak.o: /usr/include/time.h /usr/include/endian.h
vsFastrak.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsFastrak.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsFastrak.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsFastrak.o: /usr/include/sys/ipc.h /usr/include/bits/ipc.h
vsFastrak.o: /usr/include/sys/shm.h /usr/include/bits/shm.h
vsFastrak.o: /usr/include/sys/sem.h /usr/include/bits/sem.h
vsFastrak.o: /usr/include/string.h /usr/include/stdlib.h
vsFastrak.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsFastrak.o: /usr/include/alloca.h /usr/include/signal.h
vsFastrak.o: /usr/include/bits/signum.h /usr/include/bits/sigaction.h
vsFastrak.o: /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h
vsFastrak.o: /usr/include/bits/sigstack.h /usr/include/ucontext.h
vsFastrak.o: /usr/include/sys/ucontext.h
vsIS600.o: vsIS600.h++ vsSerialPort.h++ /usr/include/termios.h
vsIS600.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsIS600.o: /usr/include/gnu/stubs.h /usr/include/bits/termios.h
vsIS600.o: /usr/include/sys/ttydefaults.h /usr/include/unistd.h
vsIS600.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
vsIS600.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsIS600.o: /usr/include/bits/confname.h /usr/include/getopt.h
vsIS600.o: ../../io/common/vsMotionTracker.h++
vsIS600.o: ../../io/common/vs6DInputDevice.h++ ../../util/common/vsVector.h++
vsIS600.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
vsIS600.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsIS600.o: /usr/include/gconv.h
vsIS600.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsIS600.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsIS600.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsIS600.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsIS600.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsIS600.o: ../../util/common/vsQuat.h++ ../../io/common/vsInputDevice.h++
vsIS600.o: ../../io/common/vsInputAxis.h++ ../../io/common/vsInputButton.h++
vsIS600.o: ../../io/common/vsTrackingSystem.h++
vsIS600.o: ../../io/common/vsInputSystem.h++
vsIS600.o: ../../util/common/vsUpdatable.h++ ../../util/common/vsObject.h++
vsIS600.o: ../../util/common/vsTreeMap.h++
vsIS600.o: ../../util/common/vsGrowableArray.h++
vsIS600.o: ../../io/common/vsSharedInputData.h++ /usr/include/sys/types.h
vsIS600.o: /usr/include/time.h /usr/include/endian.h
vsIS600.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsIS600.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsIS600.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsIS600.o: /usr/include/sys/ipc.h /usr/include/bits/ipc.h
vsIS600.o: /usr/include/sys/shm.h /usr/include/bits/shm.h
vsIS600.o: /usr/include/sys/sem.h /usr/include/bits/sem.h
vsIS600.o: /usr/include/string.h /usr/include/stdlib.h
vsIS600.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsIS600.o: /usr/include/alloca.h /usr/include/signal.h
vsIS600.o: /usr/include/bits/signum.h /usr/include/bits/sigaction.h
vsIS600.o: /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h
vsIS600.o: /usr/include/bits/sigstack.h /usr/include/ucontext.h
vsIS600.o: /usr/include/sys/ucontext.h
vsLinuxJoystickSystem.o: vsLinuxJoystickSystem.h++
vsLinuxJoystickSystem.o: ../../io/common/vsInputSystem.h++
vsLinuxJoystickSystem.o: ../../util/common/vsUpdatable.h++
vsLinuxJoystickSystem.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsLinuxJoystickSystem.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsLinuxJoystickSystem.o: /usr/include/gnu/stubs.h
vsLinuxJoystickSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsLinuxJoystickSystem.o: /usr/include/bits/types.h /usr/include/libio.h
vsLinuxJoystickSystem.o: /usr/include/_G_config.h /usr/include/wchar.h
vsLinuxJoystickSystem.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsLinuxJoystickSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsLinuxJoystickSystem.o: /usr/include/bits/stdio_lim.h
vsLinuxJoystickSystem.o: /usr/include/bits/sys_errlist.h
vsLinuxJoystickSystem.o: /usr/include/getopt.h
vsLinuxJoystickSystem.o: ../../util/common/vsTreeMap.h++
vsLinuxJoystickSystem.o: ../../util/common/vsGrowableArray.h++
vsLinuxJoystickSystem.o: ../../io/common/vsJoystick.h++
vsLinuxJoystickSystem.o: ../../io/common/vsInputDevice.h++
vsLinuxJoystickSystem.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsLinuxJoystickSystem.o: /usr/include/bits/huge_val.h
vsLinuxJoystickSystem.o: /usr/include/bits/mathdef.h
vsLinuxJoystickSystem.o: /usr/include/bits/mathcalls.h
vsLinuxJoystickSystem.o: ../../io/common/vsInputAxis.h++
vsLinuxJoystickSystem.o: ../../io/common/vsInputButton.h++
vsLinuxJoystickSystem.o: /usr/include/linux/joystick.h
vsLinuxJoystickSystem.o: /usr/include/asm/types.h /usr/include/linux/input.h
vsLinuxJoystickSystem.o: /usr/include/sys/time.h /usr/include/time.h
vsLinuxJoystickSystem.o: /usr/include/bits/time.h /usr/include/sys/select.h
vsLinuxJoystickSystem.o: /usr/include/bits/select.h
vsLinuxJoystickSystem.o: /usr/include/bits/sigset.h /usr/include/sys/ioctl.h
vsLinuxJoystickSystem.o: /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h
vsLinuxJoystickSystem.o: /usr/include/asm/ioctl.h
vsLinuxJoystickSystem.o: /usr/include/bits/ioctl-types.h
vsLinuxJoystickSystem.o: /usr/include/sys/ttydefaults.h /usr/include/string.h
vsLinuxJoystickSystem.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsLinuxJoystickSystem.o: /usr/include/sys/types.h /usr/include/endian.h
vsLinuxJoystickSystem.o: /usr/include/bits/endian.h
vsLinuxJoystickSystem.o: /usr/include/sys/sysmacros.h /usr/include/sys/stat.h
vsLinuxJoystickSystem.o: /usr/include/bits/stat.h /usr/include/unistd.h
vsLinuxJoystickSystem.o: /usr/include/bits/posix_opt.h
vsLinuxJoystickSystem.o: /usr/include/bits/confname.h
vsSerialPort.o: /usr/include/stdio.h /usr/include/features.h
vsSerialPort.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSerialPort.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSerialPort.o: /usr/include/bits/types.h /usr/include/libio.h
vsSerialPort.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSerialPort.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSerialPort.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSerialPort.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsSerialPort.o: /usr/include/getopt.h /usr/include/sys/types.h
vsSerialPort.o: /usr/include/time.h /usr/include/endian.h
vsSerialPort.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsSerialPort.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsSerialPort.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsSerialPort.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
vsSerialPort.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsSerialPort.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
vsSerialPort.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
vsSerialPort.o: /usr/include/bits/ioctl-types.h
vsSerialPort.o: /usr/include/sys/ttydefaults.h vsSerialPort.h++
vsSerialPort.o: /usr/include/termios.h /usr/include/bits/termios.h
vsSerialPort.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
vsSerialPort.o: /usr/include/bits/confname.h
