#!/bin/make
# makefile for sound/OpenAL

.SUFFIXES: .c .c++ .o .h .h++

SG=OSG
WS=X
AL=OpenAL
CC=g++
CC_FLAGS=-D_XOPEN_SOURCE -D_BSD_SOURCE -D_MISC_SOURCE -DPF_CPLUSPLUS_API=1 -g -O -ansi -D__linux__ $(INCDIRS)

INCDIRS = -I. \
	  -I../../graphics/common \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../graphics/$(SG) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
	  vsSoundBuffer.o \
	  vsSoundListenerAttribute.o \
	  vsSoundPipe.o \
	  vsSoundSample.o \
	  vsSoundSourceAttribute.o \
	  vsSoundStream.o

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

vsSoundBuffer.o: vsSoundBuffer.h++
vsSoundListenerAttribute.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundListenerAttribute.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundListenerAttribute.o: /usr/include/sys/time.h /usr/include/features.h
vsSoundListenerAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundListenerAttribute.o: /usr/include/bits/types.h
vsSoundListenerAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundListenerAttribute.o: /usr/include/time.h /usr/include/bits/time.h
vsSoundListenerAttribute.o: /usr/include/sys/select.h
vsSoundListenerAttribute.o: /usr/include/bits/select.h
vsSoundListenerAttribute.o: /usr/include/bits/sigset.h /usr/include/stdio.h
vsSoundListenerAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsSoundListenerAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsSoundListenerAttribute.o: /usr/include/gconv.h
vsSoundListenerAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundListenerAttribute.o: /usr/include/bits/stdio_lim.h
vsSoundListenerAttribute.o: /usr/include/bits/sys_errlist.h
vsSoundListenerAttribute.o: /usr/include/getopt.h
vsSoundListenerAttribute.o: vsSoundListenerAttribute.h++
vsSoundListenerAttribute.o: ../../graphics/OSG/vsComponent.h++
vsSoundListenerAttribute.o: ../../util/common/vsGrowableArray.h++
vsSoundListenerAttribute.o: ../../util/common/vsVector.h++
vsSoundListenerAttribute.o: ../../util/common/vsGlobals.h++
vsSoundListenerAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSoundListenerAttribute.o: /usr/include/bits/mathdef.h
vsSoundListenerAttribute.o: /usr/include/bits/mathcalls.h
vsSoundListenerAttribute.o: ../../graphics/OSG/vsAttribute.h++
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsSoundListenerAttribute.o: /usr/include/stdlib.h
vsSoundListenerAttribute.o: /usr/include/bits/waitflags.h
vsSoundListenerAttribute.o: /usr/include/bits/waitstatus.h
vsSoundListenerAttribute.o: /usr/include/endian.h /usr/include/bits/endian.h
vsSoundListenerAttribute.o: /usr/include/alloca.h
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsSoundListenerAttribute.o: ../../util/common/vsObject.h++
vsSoundListenerAttribute.o: ../../util/common/vsTreeMap.h++
vsSoundListenerAttribute.o: ../../util/common/vsObjectMap.h++
vsSoundListenerAttribute.o: ../../graphics/OSG/vsNode.h++
vsSoundListenerAttribute.o: ../../util/common/vsMatrix.h++
vsSoundListenerAttribute.o: ../../util/common/vsQuat.h++
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsSoundListenerAttribute.o: /usr/include/string.h
vsSoundListenerAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsSoundListenerAttribute.o: vsSoundBuffer.h++
vsSoundPipe.o: vsSoundPipe.h++ /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundPipe.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundPipe.o: /usr/include/stdio.h /usr/include/features.h
vsSoundPipe.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundPipe.o: /usr/include/bits/types.h /usr/include/libio.h
vsSoundPipe.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSoundPipe.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSoundPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundPipe.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsSoundPipe.o: /usr/include/getopt.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/alut.h
vsSoundSample.o: /ist/projects/nve/tools/openal/include/AL/aluttypes.h
vsSoundSample.o: /usr/include/stdio.h /usr/include/features.h
vsSoundSample.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundSample.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundSample.o: /usr/include/bits/types.h /usr/include/libio.h
vsSoundSample.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSoundSample.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSoundSample.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundSample.o: /usr/include/bits/stdio_lim.h
vsSoundSample.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSoundSample.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSoundSample.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSoundSample.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSoundSample.o: vsSoundSample.h++ ../../util/common/vsGlobals.h++
vsSoundSample.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSoundSample.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsSoundSample.o: vsSoundBuffer.h++
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSourceAttribute.o: /usr/include/sys/time.h /usr/include/features.h
vsSoundSourceAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundSourceAttribute.o: /usr/include/bits/types.h
vsSoundSourceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundSourceAttribute.o: /usr/include/time.h /usr/include/bits/time.h
vsSoundSourceAttribute.o: /usr/include/sys/select.h
vsSoundSourceAttribute.o: /usr/include/bits/select.h
vsSoundSourceAttribute.o: /usr/include/bits/sigset.h /usr/include/stdio.h
vsSoundSourceAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsSoundSourceAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsSoundSourceAttribute.o: /usr/include/gconv.h
vsSoundSourceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundSourceAttribute.o: /usr/include/bits/stdio_lim.h
vsSoundSourceAttribute.o: /usr/include/bits/sys_errlist.h
vsSoundSourceAttribute.o: /usr/include/getopt.h vsSoundSourceAttribute.h++
vsSoundSourceAttribute.o: ../../graphics/OSG/vsComponent.h++
vsSoundSourceAttribute.o: ../../util/common/vsGrowableArray.h++
vsSoundSourceAttribute.o: ../../util/common/vsVector.h++
vsSoundSourceAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsSoundSourceAttribute.o: /usr/include/bits/huge_val.h
vsSoundSourceAttribute.o: /usr/include/bits/mathdef.h
vsSoundSourceAttribute.o: /usr/include/bits/mathcalls.h
vsSoundSourceAttribute.o: ../../graphics/OSG/vsAttribute.h++
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsSoundSourceAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSoundSourceAttribute.o: /usr/include/bits/waitstatus.h
vsSoundSourceAttribute.o: /usr/include/endian.h /usr/include/bits/endian.h
vsSoundSourceAttribute.o: /usr/include/alloca.h
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsSoundSourceAttribute.o: ../../util/common/vsObject.h++
vsSoundSourceAttribute.o: ../../util/common/vsTreeMap.h++
vsSoundSourceAttribute.o: ../../util/common/vsObjectMap.h++
vsSoundSourceAttribute.o: ../../graphics/OSG/vsNode.h++
vsSoundSourceAttribute.o: ../../util/common/vsMatrix.h++
vsSoundSourceAttribute.o: ../../util/common/vsQuat.h++
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsSoundSourceAttribute.o: /usr/include/string.h
vsSoundSourceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsSoundSourceAttribute.o: vsSoundSample.h++
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundSourceAttribute.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundSourceAttribute.o: vsSoundBuffer.h++ vsSoundStream.h++
vsSoundStream.o: /usr/include/stdio.h /usr/include/features.h
vsSoundStream.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSoundStream.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSoundStream.o: /usr/include/bits/types.h /usr/include/libio.h
vsSoundStream.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSoundStream.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSoundStream.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSoundStream.o: /usr/include/bits/stdio_lim.h
vsSoundStream.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSoundStream.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSoundStream.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSoundStream.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSoundStream.o: /usr/include/string.h ../../util/common/vsGlobals.h++
vsSoundStream.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSoundStream.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsSoundStream.o: vsSoundStream.h++
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/al.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/alc.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/altypes.h
vsSoundStream.o: /ist/projects/nve/tools/openal/include/AL/alctypes.h
vsSoundStream.o: vsSoundBuffer.h++
