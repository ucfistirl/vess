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
vsOptimizer.o: ../../graphics/OSG/vsAttribute.h++
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsOptimizer.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsOptimizer.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsOptimizer.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsOptimizer.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsOptimizer.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsOptimizer.o: /usr/include/bits/mathcalls.h ../../util/common/vsObject.h++
vsOptimizer.o: ../../util/common/vsTreeMap.h++
vsOptimizer.o: ../../util/common/vsGrowableArray.h++
vsOptimizer.o: ../../util/common/vsObjectMap.h++
vsOptimizer.o: ../../graphics/OSG/vsNode.h++ ../../util/common/vsVector.h++
vsOptimizer.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsOptimizer.o: ../../graphics/OSG/vsGeometry.h++
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsOptimizer.o: /usr/include/string.h
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsOptimizer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsOptimizer.o: ../../graphics/OSG/vsComponent.h++
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsOptimizer.o: ../../graphics/OSG/vsStateAttribute.h++
vsOptimizer.o: ../../graphics/OSG/vsDecalAttribute.h++
vsOptimizer.o: ../../graphics/OSG/vsLODAttribute.h++
vsOptimizer.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
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
vsScenePrinter.o: ../../graphics/OSG/vsNode.h++
vsScenePrinter.o: ../../util/common/vsObject.h++
vsScenePrinter.o: ../../util/common/vsTreeMap.h++
vsScenePrinter.o: ../../util/common/vsGrowableArray.h++
vsScenePrinter.o: ../../util/common/vsVector.h++
vsScenePrinter.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsScenePrinter.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsScenePrinter.o: /usr/include/bits/mathcalls.h
vsScenePrinter.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsScenePrinter.o: ../../graphics/OSG/vsAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsScenePrinter.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsScenePrinter.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsScenePrinter.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsScenePrinter.o: ../../util/common/vsObjectMap.h++
vsScenePrinter.o: ../../graphics/OSG/vsGeometry.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsScenePrinter.o: /usr/include/string.h
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsScenePrinter.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsScenePrinter.o: ../../graphics/OSG/vsComponent.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsScenePrinter.o: ../../graphics/OSG/vsBackfaceAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsScenePrinter.o: ../../graphics/OSG/vsStateAttribute.h++
vsScenePrinter.o: ../../graphics/OSG/vsMaterialAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Material
vsScenePrinter.o: ../../graphics/OSG/vsShadingAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsScenePrinter.o: ../../graphics/OSG/vsTransparencyAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsScenePrinter.o: ../../graphics/OSG/vsTransformAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsScenePrinter.o: ../../graphics/OSG/vsTextureAttribute.h++
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture2D
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsScenePrinter.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
