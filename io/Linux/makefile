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
