#!/bin/make
# makefile for graphics/OSG_X

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
          -I../../graphics/$(SG)_$(OS) \
          -I/usr/X11R6/include \
          -I/usr/include/g++-3 \
	  -I../../util/common \
          $(EXTRA_INCDIRS)

LIBOBJS = \
          vsPane.o \
          vsPipe.o \
          vsScreen.o \
          vsWindow.o

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

vsPane.o: vsPane.h++ vsWindow.h++ /usr/X11R6/include/X11/Xlib.h
vsPane.o: /usr/include/sys/types.h /usr/include/features.h
vsPane.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsPane.o: /usr/include/bits/types.h
vsPane.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsPane.o: /usr/include/time.h /usr/include/endian.h
vsPane.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsPane.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsPane.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsPane.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
vsPane.o: /usr/X11R6/include/X11/Xosdefs.h /usr/include/GL/glx.h
vsPane.o: /usr/X11R6/include/X11/Xutil.h /usr/include/GL/gl.h
vsPane.o: /usr/include/GL/glext.h /usr/include/GL/glxtokens.h vsScreen.h++
vsPane.o: vsPipe.h++ ../../util/common/vsGlobals.h++ /usr/include/math.h
vsPane.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsPane.o: /usr/include/bits/mathcalls.h ../../util/common/vsGrowableArray.h++
vsPane.o: ../../graphics/OSG/vsView.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsPane.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsPane.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsPane.o: /usr/include/string.h
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsPane.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsPane.o: ../../util/common/vsVector.h++ /usr/include/stdio.h
vsPane.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
vsPane.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsPane.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsPane.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsPane.o: /usr/include/getopt.h ../../util/common/vsMatrix.h++
vsPane.o: ../../util/common/vsQuat.h++ ../../graphics/OSG/vsNode.h++
vsPane.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsPane.o: ../../graphics/OSG/vsAttribute.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsPane.o: ../../util/common/vsObjectMap.h++ ../../graphics/OSG/vsScene.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsPane.o: ../../graphics/OSG/vsLightAttribute.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsPane.o: ../../graphics/OSG/vsComponent.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsPane.o: ../../graphics/OSG/vsGeometry.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsPane.o: ../../graphics/OSG/vsViewpointAttribute.h++
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullFace
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightModel
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadeModel
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/DisplayListVisitor
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/AppVisitor
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Billboard
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Switch
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Projection
vsPane.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/OccluderNode
vsPipe.o: /usr/include/stdio.h /usr/include/features.h
vsPipe.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsPipe.o: /usr/include/bits/types.h /usr/include/libio.h
vsPipe.o: /usr/include/_G_config.h /usr/include/wchar.h
vsPipe.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsPipe.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsPipe.o: /usr/include/getopt.h vsPipe.h++ /usr/X11R6/include/X11/Xlib.h
vsPipe.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
vsPipe.o: /usr/include/bits/endian.h /usr/include/sys/select.h
vsPipe.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsPipe.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsPipe.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
vsPipe.o: /usr/X11R6/include/X11/Xosdefs.h ../../util/common/vsGlobals.h++
vsPipe.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsPipe.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsPipe.o: vsScreen.h++ vsWindow.h++ /usr/include/GL/glx.h
vsPipe.o: /usr/X11R6/include/X11/Xutil.h /usr/include/GL/gl.h
vsPipe.o: /usr/include/GL/glext.h /usr/include/GL/glxtokens.h vsPane.h++
vsPipe.o: ../../graphics/OSG/vsView.h++
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsPipe.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsPipe.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsPipe.o: /usr/include/string.h
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsPipe.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsPipe.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsPipe.o: ../../util/common/vsQuat.h++ ../../graphics/OSG/vsNode.h++
vsPipe.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsPipe.o: ../../util/common/vsGrowableArray.h++
vsPipe.o: ../../graphics/OSG/vsAttribute.h++
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsPipe.o: ../../util/common/vsObjectMap.h++ ../../graphics/OSG/vsScene.h++
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsPipe.o: ../../graphics/OSG/vsLightAttribute.h++
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsPipe.o: ../../graphics/OSG/vsComponent.h++
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsPipe.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsScreen.o: /usr/include/stdio.h /usr/include/features.h
vsScreen.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsScreen.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsScreen.o: /usr/include/bits/types.h /usr/include/libio.h
vsScreen.o: /usr/include/_G_config.h /usr/include/wchar.h
vsScreen.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsScreen.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsScreen.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsScreen.o: /usr/include/getopt.h vsScreen.h++ /usr/X11R6/include/X11/Xlib.h
vsScreen.o: /usr/include/sys/types.h /usr/include/time.h
vsScreen.o: /usr/include/endian.h /usr/include/bits/endian.h
vsScreen.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsScreen.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsScreen.o: /usr/include/sys/sysmacros.h /usr/X11R6/include/X11/X.h
vsScreen.o: /usr/X11R6/include/X11/Xfuncproto.h
vsScreen.o: /usr/X11R6/include/X11/Xosdefs.h vsPipe.h++
vsScreen.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsScreen.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsScreen.o: /usr/include/bits/mathcalls.h vsWindow.h++ /usr/include/GL/glx.h
vsScreen.o: /usr/X11R6/include/X11/Xutil.h /usr/include/GL/gl.h
vsScreen.o: /usr/include/GL/glext.h /usr/include/GL/glxtokens.h vsPane.h++
vsScreen.o: ../../graphics/OSG/vsView.h++
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsScreen.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsScreen.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsScreen.o: /usr/include/string.h
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsScreen.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsScreen.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsScreen.o: ../../util/common/vsQuat.h++ ../../graphics/OSG/vsNode.h++
vsScreen.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsScreen.o: ../../util/common/vsGrowableArray.h++
vsScreen.o: ../../graphics/OSG/vsAttribute.h++
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsScreen.o: ../../util/common/vsObjectMap.h++ ../../graphics/OSG/vsScene.h++
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsScreen.o: ../../graphics/OSG/vsLightAttribute.h++
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsScreen.o: ../../graphics/OSG/vsComponent.h++
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsScreen.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsWindow.o: /usr/include/stdio.h /usr/include/features.h
vsWindow.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWindow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWindow.o: /usr/include/bits/types.h /usr/include/libio.h
vsWindow.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWindow.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWindow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWindow.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsWindow.o: /usr/include/getopt.h /usr/include/sys/time.h /usr/include/time.h
vsWindow.o: /usr/include/bits/time.h /usr/include/sys/select.h
vsWindow.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Image
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsWindow.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsWindow.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsWindow.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/WriteFile
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsWindow.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsWindow.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgDB/Export
vsWindow.o: vsWindow.h++ /usr/X11R6/include/X11/Xlib.h
vsWindow.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
vsWindow.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
vsWindow.o: /usr/X11R6/include/X11/Xosdefs.h /usr/include/GL/glx.h
vsWindow.o: /usr/X11R6/include/X11/Xutil.h /usr/include/GL/gl.h
vsWindow.o: /usr/include/GL/glext.h /usr/include/GL/glxtokens.h vsScreen.h++
vsWindow.o: vsPipe.h++ ../../util/common/vsGlobals.h++
vsWindow.o: ../../util/common/vsGrowableArray.h++ vsPane.h++
vsWindow.o: ../../graphics/OSG/vsView.h++
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsWindow.o: /usr/include/string.h
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsWindow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsWindow.o: ../../util/common/vsVector.h++ ../../util/common/vsMatrix.h++
vsWindow.o: ../../util/common/vsQuat.h++ ../../graphics/OSG/vsNode.h++
vsWindow.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsWindow.o: ../../graphics/OSG/vsAttribute.h++
vsWindow.o: ../../util/common/vsObjectMap.h++ ../../graphics/OSG/vsScene.h++
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsWindow.o: ../../graphics/OSG/vsLightAttribute.h++
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsWindow.o: ../../graphics/OSG/vsComponent.h++
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsWindow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsWindow.o: /usr/X11R6/include/Xm/MwmUtil.h /usr/X11R6/include/X11/Xmd.h
