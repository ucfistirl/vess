#!/bin/make
# makefile for graphics/OSG

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../system/$(SG) \
	  -I../../avatar/common \
	  -I../../motion/common \
	  -I../../io/common \
	  -I../../io/$(WS) \
	  -I../../io/$(WS)_$(SG) \
	  -I../../io/$(OS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)
    
LIBOBJS = \
          vsAttribute.o \
          vsBackfaceAttribute.o \
          vsBillboardAttribute.o \
          vsBillboardCallback.o \
          vsComponent.o \
          vsDatabaseLoader.o \
          vsDecalAttribute.o \
          vsDecalCallback.o \
          vsDynamicGeometry.o \
          vsFogAttribute.o \
          vsGeometry.o \
          vsGraphicsState.o \
          vsIntersect.o \
          vsLightAttribute.o \
          vsLocalLightCallback.o \
          vsLODAttribute.o \
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
	@rm -f *.o makefile.bak

.c++.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj

.c.o:
	$(CC) $(CC_FLAGS) $(INCDIRS) -c $< -o $@
	@cp $@ ../obj
# DO NOT DELETE

vsAttribute.o: vsAttribute.h++
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsAttribute.o: /usr/include/stdlib.h /usr/include/features.h
vsAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsAttribute.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsAttribute.o: /usr/include/endian.h /usr/include/bits/endian.h
vsAttribute.o: /usr/include/alloca.h
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsAttribute.o: /usr/include/bits/mathcalls.h ../../util/common/vsObject.h++
vsAttribute.o: /usr/include/stdio.h /usr/include/bits/types.h
vsAttribute.o: /usr/include/libio.h /usr/include/_G_config.h
vsAttribute.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsAttribute.o: /usr/include/gconv.h
vsAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsAttribute.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsAttribute.o: /usr/include/getopt.h ../../util/common/vsTreeMap.h++
vsAttribute.o: ../../util/common/vsGrowableArray.h++
vsAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsAttribute.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsAttribute.o: ../../util/common/vsQuat.h++ /usr/include/string.h
vsBackfaceAttribute.o: vsBackfaceAttribute.h++
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsBackfaceAttribute.o: /usr/include/stdlib.h /usr/include/features.h
vsBackfaceAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsBackfaceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsBackfaceAttribute.o: /usr/include/bits/waitflags.h
vsBackfaceAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsBackfaceAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsBackfaceAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsBackfaceAttribute.o: /usr/include/bits/mathdef.h
vsBackfaceAttribute.o: /usr/include/bits/mathcalls.h
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsBackfaceAttribute.o: vsStateAttribute.h++
vsBackfaceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsBackfaceAttribute.o: ../../util/common/vsGlobals.h++
vsBackfaceAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsBackfaceAttribute.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsBackfaceAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsBackfaceAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsBackfaceAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsBackfaceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsBackfaceAttribute.o: /usr/include/bits/stdio_lim.h
vsBackfaceAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsBackfaceAttribute.o: ../../util/common/vsTreeMap.h++
vsBackfaceAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsBackfaceAttribute.o: ../../util/common/vsVector.h++
vsBackfaceAttribute.o: ../../util/common/vsMatrix.h++
vsBackfaceAttribute.o: ../../util/common/vsQuat.h++
vsBillboardAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsBillboardAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsBillboardAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsBillboardAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsBillboardAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsBillboardAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsBillboardAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsBillboardAttribute.o: /usr/include/bits/stdio_lim.h
vsBillboardAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsBillboardAttribute.o: vsBillboardAttribute.h++
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsBillboardAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsBillboardAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsBillboardAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsBillboardAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsBillboardAttribute.o: /usr/include/bits/mathdef.h
vsBillboardAttribute.o: /usr/include/bits/mathcalls.h
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsBillboardAttribute.o: /usr/include/string.h
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsBillboardAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsBillboardAttribute.o: ../../util/common/vsMatrix.h++
vsBillboardAttribute.o: ../../util/common/vsGlobals.h++
vsBillboardAttribute.o: ../../util/common/vsVector.h++
vsBillboardAttribute.o: ../../util/common/vsQuat.h++ vsAttribute.h++
vsBillboardAttribute.o: ../../util/common/vsObject.h++
vsBillboardAttribute.o: ../../util/common/vsTreeMap.h++
vsBillboardAttribute.o: ../../util/common/vsGrowableArray.h++
vsBillboardAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsBillboardAttribute.o: vsBillboardCallback.h++ vsComponent.h++
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsBillboardCallback.o: /usr/include/stdlib.h /usr/include/features.h
vsBillboardCallback.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsBillboardCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsBillboardCallback.o: /usr/include/bits/waitflags.h
vsBillboardCallback.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsBillboardCallback.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsBillboardCallback.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsBillboardCallback.o: /usr/include/bits/mathdef.h
vsBillboardCallback.o: /usr/include/bits/mathcalls.h
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsBillboardCallback.o: /usr/include/string.h
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsBillboardCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsBillboardCallback.o: vsBillboardCallback.h++ vsBillboardAttribute.h++
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsBillboardCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsBillboardCallback.o: ../../util/common/vsMatrix.h++ /usr/include/stdio.h
vsBillboardCallback.o: /usr/include/bits/types.h /usr/include/libio.h
vsBillboardCallback.o: /usr/include/_G_config.h /usr/include/wchar.h
vsBillboardCallback.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsBillboardCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsBillboardCallback.o: /usr/include/bits/stdio_lim.h
vsBillboardCallback.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsBillboardCallback.o: ../../util/common/vsGlobals.h++
vsBillboardCallback.o: ../../util/common/vsVector.h++
vsBillboardCallback.o: ../../util/common/vsQuat.h++ vsAttribute.h++
vsBillboardCallback.o: ../../util/common/vsObject.h++
vsBillboardCallback.o: ../../util/common/vsTreeMap.h++
vsBillboardCallback.o: ../../util/common/vsGrowableArray.h++
vsBillboardCallback.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsComponent.o: vsComponent.h++ ../../util/common/vsGrowableArray.h++
vsComponent.o: ../../util/common/vsVector.h++ /usr/include/stdio.h
vsComponent.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsComponent.o: /usr/include/gnu/stubs.h
vsComponent.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsComponent.o: /usr/include/bits/types.h /usr/include/libio.h
vsComponent.o: /usr/include/_G_config.h /usr/include/wchar.h
vsComponent.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsComponent.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsComponent.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsComponent.o: /usr/include/getopt.h ../../util/common/vsGlobals.h++
vsComponent.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsComponent.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsComponent.o: vsAttribute.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsComponent.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsComponent.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsComponent.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsComponent.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsComponent.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsComponent.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsComponent.o: /usr/include/string.h
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsComponent.o: vsTransformAttribute.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsComponent.o: vsSwitchAttribute.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Switch
vsComponent.o: vsSequenceAttribute.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Sequence
vsComponent.o: vsLODAttribute.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsComponent.o: vsDecalAttribute.h++ vsBillboardAttribute.h++ vsGeometry.h++
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsComponent.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsComponent.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsComponent.o: vsDynamicGeometry.h++
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
vsDatabaseLoader.o: /usr/include/string.h
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Registry
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/DynamicLibrary
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Export
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/ReaderWriter
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsDatabaseLoader.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsDatabaseLoader.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/DotOsgWrapper
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Input
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsDatabaseLoader.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/FieldReaderIterator
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Field
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/FieldReader
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Output
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/ReadFile
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Sequence
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Switch
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PositionAttitudeTransform
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Billboard
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Fog
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Material
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture2D
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonMode
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonOffset
vsDatabaseLoader.o: vsDatabaseLoader.h++ vsComponent.h++
vsDatabaseLoader.o: ../../util/common/vsGrowableArray.h++
vsDatabaseLoader.o: ../../util/common/vsVector.h++
vsDatabaseLoader.o: ../../util/common/vsGlobals.h++ vsAttribute.h++
vsDatabaseLoader.o: ../../util/common/vsObject.h++
vsDatabaseLoader.o: ../../util/common/vsTreeMap.h++
vsDatabaseLoader.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsDatabaseLoader.o: ../../util/common/vsMatrix.h++
vsDatabaseLoader.o: ../../util/common/vsQuat.h++ vsGeometry.h++
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsDatabaseLoader.o: vsLODAttribute.h++ vsSequenceAttribute.h++
vsDatabaseLoader.o: vsSwitchAttribute.h++ vsTransformAttribute.h++
vsDatabaseLoader.o: vsBillboardAttribute.h++ vsFogAttribute.h++
vsDatabaseLoader.o: vsStateAttribute.h++ vsMaterialAttribute.h++
vsDatabaseLoader.o: vsTextureAttribute.h++ vsBackfaceAttribute.h++
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsDatabaseLoader.o: vsTransparencyAttribute.h++
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsDatabaseLoader.o: vsWireframeAttribute.h++ vsShadingAttribute.h++
vsDatabaseLoader.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsDatabaseLoader.o: vsDecalAttribute.h++
vsDatabaseLoader.o: ../../graphics/common/vsOptimizer.h++
vsDecalAttribute.o: vsDecalAttribute.h++
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsDecalAttribute.o: /usr/include/stdlib.h /usr/include/features.h
vsDecalAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsDecalAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDecalAttribute.o: /usr/include/bits/waitflags.h
vsDecalAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsDecalAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsDecalAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsDecalAttribute.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsDecalAttribute.o: /usr/include/string.h
vsDecalAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsDecalAttribute.o: vsAttribute.h++ ../../util/common/vsGlobals.h++
vsDecalAttribute.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsDecalAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsDecalAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDecalAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDecalAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDecalAttribute.o: /usr/include/bits/stdio_lim.h
vsDecalAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDecalAttribute.o: ../../util/common/vsTreeMap.h++
vsDecalAttribute.o: ../../util/common/vsGrowableArray.h++
vsDecalAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsDecalAttribute.o: ../../util/common/vsVector.h++
vsDecalAttribute.o: ../../util/common/vsMatrix.h++
vsDecalAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsDecalAttribute.o: vsDecalCallback.h++
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonOffset
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsDecalCallback.o: /usr/include/stdlib.h /usr/include/features.h
vsDecalCallback.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsDecalCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDecalCallback.o: /usr/include/bits/waitflags.h
vsDecalCallback.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsDecalCallback.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsDecalCallback.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsDecalCallback.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsDecalCallback.o: /usr/include/string.h
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsDecalCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsDecalCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsDecalCallback.o: vsDecalCallback.h++ vsDecalAttribute.h++ vsAttribute.h++
vsDecalCallback.o: ../../util/common/vsGlobals.h++
vsDecalCallback.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsDecalCallback.o: /usr/include/bits/types.h /usr/include/libio.h
vsDecalCallback.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDecalCallback.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDecalCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDecalCallback.o: /usr/include/bits/stdio_lim.h
vsDecalCallback.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDecalCallback.o: ../../util/common/vsTreeMap.h++
vsDecalCallback.o: ../../util/common/vsGrowableArray.h++
vsDecalCallback.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsDecalCallback.o: ../../util/common/vsVector.h++
vsDecalCallback.o: ../../util/common/vsMatrix.h++
vsDecalCallback.o: ../../util/common/vsQuat.h++
vsDynamicGeometry.o: /usr/include/stdio.h /usr/include/features.h
vsDynamicGeometry.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsDynamicGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsDynamicGeometry.o: /usr/include/bits/types.h /usr/include/libio.h
vsDynamicGeometry.o: /usr/include/_G_config.h /usr/include/wchar.h
vsDynamicGeometry.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsDynamicGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsDynamicGeometry.o: /usr/include/bits/stdio_lim.h
vsDynamicGeometry.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsDynamicGeometry.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsDynamicGeometry.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsDynamicGeometry.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsDynamicGeometry.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsDynamicGeometry.o: /usr/include/bits/mathdef.h
vsDynamicGeometry.o: /usr/include/bits/mathcalls.h
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsDynamicGeometry.o: /usr/include/string.h
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsDynamicGeometry.o: vsDynamicGeometry.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsDynamicGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsDynamicGeometry.o: ../../util/common/vsVector.h++
vsDynamicGeometry.o: ../../util/common/vsGlobals.h++ vsAttribute.h++
vsDynamicGeometry.o: ../../util/common/vsObject.h++
vsDynamicGeometry.o: ../../util/common/vsTreeMap.h++
vsDynamicGeometry.o: ../../util/common/vsGrowableArray.h++
vsDynamicGeometry.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsDynamicGeometry.o: ../../util/common/vsMatrix.h++
vsDynamicGeometry.o: ../../util/common/vsQuat.h++ vsGeometry.h++
vsDynamicGeometry.o: vsComponent.h++ vsBackfaceAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsDynamicGeometry.o: vsStateAttribute.h++ vsFogAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Fog
vsDynamicGeometry.o: vsMaterialAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Material
vsDynamicGeometry.o: vsShadingAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsDynamicGeometry.o: vsTextureAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture2D
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsDynamicGeometry.o: vsTransparencyAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsDynamicGeometry.o: vsWireframeAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonMode
vsDynamicGeometry.o: vsGraphicsState.h++ vsLightAttribute.h++
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsDynamicGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsDynamicGeometry.o: vsScene.h++
vsFogAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsFogAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsFogAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsFogAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsFogAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsFogAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsFogAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsFogAttribute.o: /usr/include/bits/stdio_lim.h
vsFogAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsFogAttribute.o: vsFogAttribute.h++
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Fog
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsFogAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsFogAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsFogAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsFogAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsFogAttribute.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsFogAttribute.o: vsStateAttribute.h++
vsFogAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsFogAttribute.o: ../../util/common/vsGlobals.h++
vsFogAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsFogAttribute.o: ../../util/common/vsObject.h++
vsFogAttribute.o: ../../util/common/vsTreeMap.h++
vsFogAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsFogAttribute.o: ../../util/common/vsVector.h++
vsFogAttribute.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsGeometry.o: /usr/include/stdio.h /usr/include/features.h
vsGeometry.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsGeometry.o: /usr/include/bits/types.h /usr/include/libio.h
vsGeometry.o: /usr/include/_G_config.h /usr/include/wchar.h
vsGeometry.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsGeometry.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsGeometry.o: /usr/include/getopt.h
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsGeometry.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsGeometry.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsGeometry.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsGeometry.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsGeometry.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsGeometry.o: /usr/include/string.h
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsGeometry.o: vsGeometry.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsGeometry.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsGeometry.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsGeometry.o: vsAttribute.h++ ../../util/common/vsObject.h++
vsGeometry.o: ../../util/common/vsTreeMap.h++
vsGeometry.o: ../../util/common/vsGrowableArray.h++
vsGeometry.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsGeometry.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsGeometry.o: vsComponent.h++ vsBackfaceAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsGeometry.o: vsStateAttribute.h++ vsFogAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Fog
vsGeometry.o: vsMaterialAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Material
vsGeometry.o: vsShadingAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsGeometry.o: vsTextureAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture2D
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsGeometry.o: vsTransparencyAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsGeometry.o: vsWireframeAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonMode
vsGeometry.o: vsGraphicsState.h++ vsLightAttribute.h++
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsGeometry.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsGeometry.o: vsScene.h++
vsGraphicsState.o: vsGraphicsState.h++ vsTransparencyAttribute.h++
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsGraphicsState.o: /usr/include/stdlib.h /usr/include/features.h
vsGraphicsState.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsGraphicsState.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsGraphicsState.o: /usr/include/bits/waitflags.h
vsGraphicsState.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsGraphicsState.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsGraphicsState.o: vsStateAttribute.h++
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsGraphicsState.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsGraphicsState.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsGraphicsState.o: /usr/include/bits/mathcalls.h
vsGraphicsState.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsGraphicsState.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsGraphicsState.o: /usr/include/bits/types.h /usr/include/libio.h
vsGraphicsState.o: /usr/include/_G_config.h /usr/include/wchar.h
vsGraphicsState.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsGraphicsState.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsGraphicsState.o: /usr/include/bits/stdio_lim.h
vsGraphicsState.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsGraphicsState.o: ../../util/common/vsTreeMap.h++
vsGraphicsState.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsGraphicsState.o: ../../util/common/vsVector.h++
vsGraphicsState.o: ../../util/common/vsMatrix.h++
vsGraphicsState.o: ../../util/common/vsQuat.h++ vsLightAttribute.h++
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsGraphicsState.o: /usr/include/string.h
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsGraphicsState.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsGraphicsState.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsGraphicsState.o: vsScene.h++
vsIntersect.o: /usr/include/stdio.h /usr/include/features.h
vsIntersect.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsIntersect.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsIntersect.o: /usr/include/bits/types.h /usr/include/libio.h
vsIntersect.o: /usr/include/_G_config.h /usr/include/wchar.h
vsIntersect.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsIntersect.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsIntersect.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsIntersect.o: /usr/include/getopt.h vsIntersect.h++
vsIntersect.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsIntersect.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsIntersect.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsIntersect.o: vsGeometry.h++
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsIntersect.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsIntersect.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsIntersect.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsIntersect.o: /usr/include/string.h
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsIntersect.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsIntersect.o: vsAttribute.h++ ../../util/common/vsObject.h++
vsIntersect.o: ../../util/common/vsTreeMap.h++
vsIntersect.o: ../../util/common/vsGrowableArray.h++
vsIntersect.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsIntersect.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsIntersect.o: ../../graphics/OSG_X/vsPane.h++
vsIntersect.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/X11/Xlib.h
vsIntersect.o: /usr/include/sys/types.h /usr/include/time.h
vsIntersect.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsIntersect.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsIntersect.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsIntersect.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsIntersect.o: /usr/include/GL/glx.h /usr/include/X11/Xutil.h
vsIntersect.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
vsIntersect.o: /usr/include/GL/glxtokens.h ../../graphics/OSG_X/vsScreen.h++
vsIntersect.o: ../../graphics/OSG_X/vsPipe.h++ vsView.h++
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsIntersect.o: vsScene.h++
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsIntersect.o: vsLightAttribute.h++
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsIntersect.o: vsComponent.h++
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/IntersectVisitor
vsIntersect.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LineSegment
vsIntersect.o: vsDynamicGeometry.h++
vsLightAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsLightAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsLightAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsLightAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsLightAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsLightAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsLightAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsLightAttribute.o: /usr/include/bits/stdio_lim.h
vsLightAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsLightAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsLightAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsLightAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsLightAttribute.o: vsLightAttribute.h++
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsLightAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsLightAttribute.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsLightAttribute.o: /usr/include/string.h
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsLightAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsLightAttribute.o: vsAttribute.h++ ../../util/common/vsGlobals.h++
vsLightAttribute.o: ../../util/common/vsObject.h++
vsLightAttribute.o: ../../util/common/vsTreeMap.h++
vsLightAttribute.o: ../../util/common/vsGrowableArray.h++
vsLightAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsLightAttribute.o: ../../util/common/vsVector.h++
vsLightAttribute.o: ../../util/common/vsMatrix.h++
vsLightAttribute.o: ../../util/common/vsQuat.h++ vsScene.h++ vsComponent.h++
vsLightAttribute.o: vsGraphicsState.h++ vsTransparencyAttribute.h++
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsLightAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsLightAttribute.o: vsStateAttribute.h++
vsLocalLightCallback.o: vsLocalLightCallback.h++
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsLocalLightCallback.o: /usr/include/stdlib.h /usr/include/features.h
vsLocalLightCallback.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsLocalLightCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsLocalLightCallback.o: /usr/include/bits/waitflags.h
vsLocalLightCallback.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsLocalLightCallback.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsLocalLightCallback.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsLocalLightCallback.o: /usr/include/bits/mathdef.h
vsLocalLightCallback.o: /usr/include/bits/mathcalls.h
vsLocalLightCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsLocalLightCallback.o: /usr/include/string.h
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsLocalLightCallback.o: vsLightAttribute.h++
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsLocalLightCallback.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsLocalLightCallback.o: vsAttribute.h++ ../../util/common/vsGlobals.h++
vsLocalLightCallback.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsLocalLightCallback.o: /usr/include/bits/types.h /usr/include/libio.h
vsLocalLightCallback.o: /usr/include/_G_config.h /usr/include/wchar.h
vsLocalLightCallback.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsLocalLightCallback.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsLocalLightCallback.o: /usr/include/bits/stdio_lim.h
vsLocalLightCallback.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsLocalLightCallback.o: ../../util/common/vsTreeMap.h++
vsLocalLightCallback.o: ../../util/common/vsGrowableArray.h++
vsLocalLightCallback.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsLocalLightCallback.o: ../../util/common/vsVector.h++
vsLocalLightCallback.o: ../../util/common/vsMatrix.h++
vsLocalLightCallback.o: ../../util/common/vsQuat.h++ vsScene.h++
vsLODAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsLODAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsLODAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsLODAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsLODAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsLODAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsLODAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsLODAttribute.o: /usr/include/bits/stdio_lim.h
vsLODAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsLODAttribute.o: vsLODAttribute.h++
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsLODAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsLODAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsLODAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsLODAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsLODAttribute.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsLODAttribute.o: /usr/include/string.h
vsLODAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsLODAttribute.o: vsAttribute.h++ ../../util/common/vsGlobals.h++
vsLODAttribute.o: ../../util/common/vsObject.h++
vsLODAttribute.o: ../../util/common/vsTreeMap.h++
vsLODAttribute.o: ../../util/common/vsGrowableArray.h++
vsLODAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsLODAttribute.o: ../../util/common/vsVector.h++
vsLODAttribute.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsLODAttribute.o: vsComponent.h++
vsMaterialAttribute.o: vsMaterialAttribute.h++
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Material
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsMaterialAttribute.o: /usr/include/math.h /usr/include/features.h
vsMaterialAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsMaterialAttribute.o: /usr/include/bits/huge_val.h
vsMaterialAttribute.o: /usr/include/bits/mathdef.h
vsMaterialAttribute.o: /usr/include/bits/mathcalls.h
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsMaterialAttribute.o: /usr/include/stdlib.h
vsMaterialAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsMaterialAttribute.o: /usr/include/bits/waitflags.h
vsMaterialAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsMaterialAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsMaterialAttribute.o: vsStateAttribute.h++
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsMaterialAttribute.o: ../../util/common/vsGlobals.h++
vsMaterialAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsMaterialAttribute.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsMaterialAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsMaterialAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsMaterialAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsMaterialAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsMaterialAttribute.o: /usr/include/bits/stdio_lim.h
vsMaterialAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsMaterialAttribute.o: ../../util/common/vsTreeMap.h++
vsMaterialAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsMaterialAttribute.o: ../../util/common/vsVector.h++
vsMaterialAttribute.o: ../../util/common/vsMatrix.h++
vsMaterialAttribute.o: ../../util/common/vsQuat.h++ vsGraphicsState.h++
vsMaterialAttribute.o: vsTransparencyAttribute.h++
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsMaterialAttribute.o: vsLightAttribute.h++
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsMaterialAttribute.o: /usr/include/string.h
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsMaterialAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsMaterialAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsMaterialAttribute.o: vsScene.h++
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
vsNode.o: ../../util/common/vsGrowableArray.h++
vsNode.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsNode.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsNode.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsNode.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsNode.o: vsAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsNode.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsNode.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsNode.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsNode.o: ../../util/common/vsObjectMap.h++ vsComponent.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsNode.o: /usr/include/string.h
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsNode.o: vsBackfaceAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsNode.o: vsStateAttribute.h++ vsFogAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Fog
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsNode.o: vsMaterialAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Material
vsNode.o: vsTextureAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture2D
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsNode.o: vsTransparencyAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsNode.o: vsShadingAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsNode.o: vsWireframeAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonMode
vsNode.o: vsLightAttribute.h++
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsNode.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsNode.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsNode.o: vsScene.h++
vsScene.o: vsScene.h++
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsScene.o: /usr/include/stdlib.h /usr/include/features.h
vsScene.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsScene.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsScene.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsScene.o: /usr/include/endian.h /usr/include/bits/endian.h
vsScene.o: /usr/include/alloca.h
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsScene.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsScene.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsScene.o: /usr/include/string.h
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsScene.o: vsNode.h++ ../../util/common/vsObject.h++ /usr/include/stdio.h
vsScene.o: /usr/include/bits/types.h /usr/include/libio.h
vsScene.o: /usr/include/_G_config.h /usr/include/wchar.h
vsScene.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsScene.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsScene.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsScene.o: /usr/include/getopt.h ../../util/common/vsTreeMap.h++
vsScene.o: ../../util/common/vsGrowableArray.h++
vsScene.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsScene.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsScene.o: vsAttribute.h++ ../../util/common/vsObjectMap.h++
vsScene.o: vsLightAttribute.h++
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsScene.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsScene.o: vsComponent.h++ vsGeometry.h++
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsScene.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsScene.o: vsDynamicGeometry.h++
vsSequenceAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsSequenceAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSequenceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSequenceAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsSequenceAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSequenceAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSequenceAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSequenceAttribute.o: /usr/include/bits/stdio_lim.h
vsSequenceAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSequenceAttribute.o: vsSequenceAttribute.h++
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Sequence
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Switch
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsSequenceAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSequenceAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSequenceAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsSequenceAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSequenceAttribute.o: /usr/include/bits/mathdef.h
vsSequenceAttribute.o: /usr/include/bits/mathcalls.h
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsSequenceAttribute.o: /usr/include/string.h
vsSequenceAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsSequenceAttribute.o: vsAttribute.h++ ../../util/common/vsGlobals.h++
vsSequenceAttribute.o: ../../util/common/vsObject.h++
vsSequenceAttribute.o: ../../util/common/vsTreeMap.h++
vsSequenceAttribute.o: ../../util/common/vsGrowableArray.h++
vsSequenceAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsSequenceAttribute.o: ../../util/common/vsVector.h++
vsSequenceAttribute.o: ../../util/common/vsMatrix.h++
vsSequenceAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsShadingAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsShadingAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsShadingAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsShadingAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsShadingAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsShadingAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsShadingAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsShadingAttribute.o: /usr/include/bits/stdio_lim.h
vsShadingAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsShadingAttribute.o: vsShadingAttribute.h++
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsShadingAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsShadingAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsShadingAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsShadingAttribute.o: vsStateAttribute.h++
vsShadingAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsShadingAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsShadingAttribute.o: /usr/include/bits/huge_val.h
vsShadingAttribute.o: /usr/include/bits/mathdef.h
vsShadingAttribute.o: /usr/include/bits/mathcalls.h
vsShadingAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsShadingAttribute.o: ../../util/common/vsObject.h++
vsShadingAttribute.o: ../../util/common/vsTreeMap.h++
vsShadingAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsShadingAttribute.o: ../../util/common/vsVector.h++
vsShadingAttribute.o: ../../util/common/vsMatrix.h++
vsShadingAttribute.o: ../../util/common/vsQuat.h++
vsStateAttribute.o: vsStateAttribute.h++
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsStateAttribute.o: /usr/include/stdlib.h /usr/include/features.h
vsStateAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsStateAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsStateAttribute.o: /usr/include/bits/waitflags.h
vsStateAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsStateAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsStateAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsStateAttribute.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsStateAttribute.o: /usr/include/bits/mathcalls.h
vsStateAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsStateAttribute.o: ../../util/common/vsObject.h++ /usr/include/stdio.h
vsStateAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsStateAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsStateAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsStateAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsStateAttribute.o: /usr/include/bits/stdio_lim.h
vsStateAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsStateAttribute.o: ../../util/common/vsTreeMap.h++
vsStateAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsStateAttribute.o: ../../util/common/vsVector.h++
vsStateAttribute.o: ../../util/common/vsMatrix.h++
vsStateAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsStateAttribute.o: /usr/include/string.h
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsStateAttribute.o: vsGeometry.h++
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsStateAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsStateAttribute.o: vsDynamicGeometry.h++ vsScene.h++ vsLightAttribute.h++
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsStateAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsSwitchAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsSwitchAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSwitchAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSwitchAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsSwitchAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsSwitchAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsSwitchAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSwitchAttribute.o: /usr/include/bits/stdio_lim.h
vsSwitchAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsSwitchAttribute.o: vsSwitchAttribute.h++
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Switch
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsSwitchAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSwitchAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSwitchAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsSwitchAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsSwitchAttribute.o: /usr/include/bits/mathdef.h
vsSwitchAttribute.o: /usr/include/bits/mathcalls.h
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsSwitchAttribute.o: /usr/include/string.h
vsSwitchAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsSwitchAttribute.o: vsAttribute.h++ ../../util/common/vsGlobals.h++
vsSwitchAttribute.o: ../../util/common/vsObject.h++
vsSwitchAttribute.o: ../../util/common/vsTreeMap.h++
vsSwitchAttribute.o: ../../util/common/vsGrowableArray.h++
vsSwitchAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsSwitchAttribute.o: ../../util/common/vsVector.h++
vsSwitchAttribute.o: ../../util/common/vsMatrix.h++
vsSwitchAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsTextureAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsTextureAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTextureAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTextureAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsTextureAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTextureAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTextureAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTextureAttribute.o: /usr/include/bits/stdio_lim.h
vsTextureAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/ReadFile
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTextureAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsTextureAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsTextureAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTextureAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsTextureAttribute.o: /usr/include/bits/mathdef.h
vsTextureAttribute.o: /usr/include/bits/mathcalls.h
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Export
vsTextureAttribute.o: vsTextureAttribute.h++
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture2D
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Texture
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsTextureAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsTextureAttribute.o: vsStateAttribute.h++ ../../util/common/vsGlobals.h++
vsTextureAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsTextureAttribute.o: ../../util/common/vsObject.h++
vsTextureAttribute.o: ../../util/common/vsTreeMap.h++
vsTextureAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsTextureAttribute.o: ../../util/common/vsVector.h++
vsTextureAttribute.o: ../../util/common/vsMatrix.h++
vsTextureAttribute.o: ../../util/common/vsQuat.h++
vsTransformAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsTransformAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTransformAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTransformAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsTransformAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTransformAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTransformAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTransformAttribute.o: /usr/include/bits/stdio_lim.h
vsTransformAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTransformAttribute.o: vsTransformAttribute.h++
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTransformAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsTransformAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsTransformAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTransformAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsTransformAttribute.o: /usr/include/bits/mathdef.h
vsTransformAttribute.o: /usr/include/bits/mathcalls.h
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTransformAttribute.o: /usr/include/string.h
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsTransformAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsTransformAttribute.o: ../../util/common/vsMatrix.h++
vsTransformAttribute.o: ../../util/common/vsGlobals.h++
vsTransformAttribute.o: ../../util/common/vsVector.h++
vsTransformAttribute.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsTransformAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsTransformAttribute.o: ../../util/common/vsObject.h++
vsTransformAttribute.o: ../../util/common/vsTreeMap.h++
vsTransformAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsTransparencyAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsTransparencyAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTransparencyAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTransparencyAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsTransparencyAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsTransparencyAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsTransparencyAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTransparencyAttribute.o: /usr/include/bits/stdio_lim.h
vsTransparencyAttribute.o: /usr/include/bits/sys_errlist.h
vsTransparencyAttribute.o: /usr/include/getopt.h vsTransparencyAttribute.h++
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTransparencyAttribute.o: /usr/include/stdlib.h
vsTransparencyAttribute.o: /usr/include/bits/waitflags.h
vsTransparencyAttribute.o: /usr/include/bits/waitstatus.h
vsTransparencyAttribute.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTransparencyAttribute.o: /usr/include/alloca.h
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsTransparencyAttribute.o: vsStateAttribute.h++
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTransparencyAttribute.o: ../../util/common/vsGlobals.h++
vsTransparencyAttribute.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsTransparencyAttribute.o: /usr/include/bits/mathdef.h
vsTransparencyAttribute.o: /usr/include/bits/mathcalls.h
vsTransparencyAttribute.o: ../../util/common/vsGrowableArray.h++
vsTransparencyAttribute.o: vsAttribute.h++ ../../util/common/vsObject.h++
vsTransparencyAttribute.o: ../../util/common/vsTreeMap.h++
vsTransparencyAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsTransparencyAttribute.o: ../../util/common/vsVector.h++
vsTransparencyAttribute.o: ../../util/common/vsMatrix.h++
vsTransparencyAttribute.o: ../../util/common/vsQuat.h++ vsGraphicsState.h++
vsTransparencyAttribute.o: vsLightAttribute.h++
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsTransparencyAttribute.o: /usr/include/string.h
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsTransparencyAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsTransparencyAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsTransparencyAttribute.o: vsScene.h++
vsView.o: vsView.h++
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsView.o: /usr/include/stdlib.h /usr/include/features.h
vsView.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsView.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsView.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsView.o: /usr/include/endian.h /usr/include/bits/endian.h
vsView.o: /usr/include/alloca.h
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsView.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsView.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsView.o: /usr/include/string.h
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsView.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsView.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsView.o: ../../util/common/vsVector.h++ /usr/include/stdio.h
vsView.o: /usr/include/bits/types.h /usr/include/libio.h
vsView.o: /usr/include/_G_config.h /usr/include/wchar.h
vsView.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsView.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsView.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsView.o: /usr/include/getopt.h ../../util/common/vsGlobals.h++
vsView.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsViewpointAttribute.o: vsViewpointAttribute.h++ /usr/include/stdio.h
vsViewpointAttribute.o: /usr/include/features.h /usr/include/sys/cdefs.h
vsViewpointAttribute.o: /usr/include/gnu/stubs.h
vsViewpointAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsViewpointAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsViewpointAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsViewpointAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsViewpointAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsViewpointAttribute.o: /usr/include/bits/stdio_lim.h
vsViewpointAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsViewpointAttribute.o: vsComponent.h++ ../../util/common/vsGrowableArray.h++
vsViewpointAttribute.o: ../../util/common/vsVector.h++
vsViewpointAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsViewpointAttribute.o: /usr/include/bits/huge_val.h
vsViewpointAttribute.o: /usr/include/bits/mathdef.h
vsViewpointAttribute.o: /usr/include/bits/mathcalls.h vsAttribute.h++
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsViewpointAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsViewpointAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsViewpointAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsViewpointAttribute.o: ../../util/common/vsObject.h++
vsViewpointAttribute.o: ../../util/common/vsTreeMap.h++
vsViewpointAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsViewpointAttribute.o: ../../util/common/vsMatrix.h++
vsViewpointAttribute.o: ../../util/common/vsQuat.h++
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsViewpointAttribute.o: /usr/include/string.h
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsViewpointAttribute.o: vsView.h++
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsViewpointAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsViewpointAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsWireframeAttribute.o: /usr/include/stdio.h /usr/include/features.h
vsWireframeAttribute.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWireframeAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWireframeAttribute.o: /usr/include/bits/types.h /usr/include/libio.h
vsWireframeAttribute.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWireframeAttribute.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWireframeAttribute.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWireframeAttribute.o: /usr/include/bits/stdio_lim.h
vsWireframeAttribute.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWireframeAttribute.o: vsWireframeAttribute.h++
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PolygonMode
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsWireframeAttribute.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsWireframeAttribute.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsWireframeAttribute.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsWireframeAttribute.o: vsStateAttribute.h++
vsWireframeAttribute.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsWireframeAttribute.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWireframeAttribute.o: /usr/include/bits/huge_val.h
vsWireframeAttribute.o: /usr/include/bits/mathdef.h
vsWireframeAttribute.o: /usr/include/bits/mathcalls.h
vsWireframeAttribute.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsWireframeAttribute.o: ../../util/common/vsObject.h++
vsWireframeAttribute.o: ../../util/common/vsTreeMap.h++
vsWireframeAttribute.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsWireframeAttribute.o: ../../util/common/vsVector.h++
vsWireframeAttribute.o: ../../util/common/vsMatrix.h++
vsWireframeAttribute.o: ../../util/common/vsQuat.h++
vsTextBuilder.o: vsTextBuilder.h++ /usr/include/GL/glu.h /usr/include/GL/gl.h
vsTextBuilder.o: /usr/include/GL/glext.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTVectoriser.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTGL.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTextBuilder.o: /usr/include/stdlib.h /usr/include/features.h
vsTextBuilder.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsTextBuilder.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsTextBuilder.o: /usr/include/endian.h /usr/include/bits/endian.h
vsTextBuilder.o: /usr/include/alloca.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTextBuilder.o: /usr/include/ft2build.h
vsTextBuilder.o: /usr/include/freetype2/freetype/config/ftheader.h
vsTextBuilder.o: /usr/include/freetype2/freetype/internal/internal.h
vsTextBuilder.o: /usr/include/freetype2/freetype/freetype.h
vsTextBuilder.o: /usr/include/freetype2/freetype/config/ftconfig.h
vsTextBuilder.o: /usr/include/freetype2/freetype/config/ftoption.h
vsTextBuilder.o: /usr/include/freetype2/freetype/config/ftstdlib.h
vsTextBuilder.o: /usr/include/limits.h
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsTextBuilder.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsTextBuilder.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsTextBuilder.o: /usr/include/bits/xopen_lim.h /usr/include/bits/stdio_lim.h
vsTextBuilder.o: /usr/include/ctype.h /usr/include/bits/types.h
vsTextBuilder.o: /usr/include/string.h /usr/include/stdio.h
vsTextBuilder.o: /usr/include/libio.h /usr/include/_G_config.h
vsTextBuilder.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsTextBuilder.o: /usr/include/gconv.h
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsTextBuilder.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsTextBuilder.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h
vsTextBuilder.o: /usr/include/bits/sigset.h
vsTextBuilder.o: /usr/include/freetype2/freetype/fterrors.h
vsTextBuilder.o: /usr/include/freetype2/freetype/ftmoderr.h
vsTextBuilder.o: /usr/include/freetype2/freetype/fterrdef.h
vsTextBuilder.o: /usr/include/freetype2/freetype/fttypes.h
vsTextBuilder.o: /usr/include/freetype2/freetype/ftsystem.h
vsTextBuilder.o: /usr/include/freetype2/freetype/ftimage.h
vsTextBuilder.o: /usr/include/freetype2/freetype/ftglyph.h
vsTextBuilder.o: /usr/include/freetype2/freetype/freetype.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTGlyph.h
vsTextBuilder.o: /usr/include/freetype2/freetype/ftglyph.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTFace.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTSize.h
vsTextBuilder.o: ../../util/common/vsVector.h++
vsTextBuilder.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsTextBuilder.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTextBuilder.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsTextBuilder.o: ../../util/common/vsQuat.h++ vsComponent.h++
vsTextBuilder.o: ../../util/common/vsGrowableArray.h++ vsAttribute.h++
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTextBuilder.o: ../../util/common/vsObject.h++
vsTextBuilder.o: ../../util/common/vsTreeMap.h++
vsTextBuilder.o: ../../util/common/vsObjectMap.h++ vsNode.h++
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsTextBuilder.o: vsGeometry.h++
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsTextBuilder.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsTextBuilder.o: vsTransformAttribute.h++
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsTextBuilder.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
