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
vs3TrackerArm.o: ../../graphics/OSG/vsComponent.h++
vs3TrackerArm.o: ../../util/common/vsVector.h++
vs3TrackerArm.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vs3TrackerArm.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vs3TrackerArm.o: /usr/include/bits/mathcalls.h
vs3TrackerArm.o: ../../graphics/OSG/vsAttribute.h++
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vs3TrackerArm.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vs3TrackerArm.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vs3TrackerArm.o: /usr/include/bits/endian.h /usr/include/alloca.h
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vs3TrackerArm.o: ../../util/common/vsObjectMap.h++
vs3TrackerArm.o: ../../graphics/OSG/vsNode.h++ ../../util/common/vsMatrix.h++
vs3TrackerArm.o: ../../util/common/vsQuat.h++
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vs3TrackerArm.o: /usr/include/string.h
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vs3TrackerArm.o: ../../graphics/OSG/vsTransformAttribute.h++
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vs3TrackerArm.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsAxisRotation.o: ../../graphics/OSG/vsComponent.h++
vsAxisRotation.o: ../../util/common/vsVector.h++
vsAxisRotation.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsAxisRotation.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsAxisRotation.o: /usr/include/bits/mathcalls.h
vsAxisRotation.o: ../../graphics/OSG/vsAttribute.h++
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsAxisRotation.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsAxisRotation.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsAxisRotation.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsAxisRotation.o: ../../util/common/vsObjectMap.h++
vsAxisRotation.o: ../../graphics/OSG/vsNode.h++
vsAxisRotation.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsAxisRotation.o: /usr/include/string.h
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsAxisRotation.o: ../../graphics/OSG/vsTransformAttribute.h++
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsAxisRotation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsCollision.o: ../../graphics/OSG/vsComponent.h++
vsCollision.o: ../../util/common/vsVector.h++ ../../util/common/vsGlobals.h++
vsCollision.o: /usr/include/math.h /usr/include/bits/huge_val.h
vsCollision.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
vsCollision.o: ../../graphics/OSG/vsAttribute.h++
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsCollision.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsCollision.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsCollision.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsCollision.o: ../../util/common/vsObjectMap.h++
vsCollision.o: ../../graphics/OSG/vsNode.h++ ../../util/common/vsMatrix.h++
vsCollision.o: ../../util/common/vsQuat.h++
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsCollision.o: /usr/include/string.h
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsCollision.o: ../../graphics/OSG/vsTransformAttribute.h++
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsCollision.o: ../../graphics/OSG/vsIntersect.h++
vsCollision.o: ../../graphics/OSG/vsGeometry.h++
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsCollision.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsCollision.o: ../../graphics/OSG_X/vsPane.h++
vsCollision.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/X11/Xlib.h
vsCollision.o: /usr/include/sys/types.h /usr/include/time.h
vsCollision.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsCollision.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsCollision.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsCollision.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsCollision.o: /usr/include/GL/glx.h /usr/include/X11/Xutil.h
vsCollision.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
vsCollision.o: /usr/include/GL/glxtokens.h ../../graphics/OSG_X/vsScreen.h++
vsCollision.o: ../../graphics/OSG_X/vsPipe.h++ ../../graphics/OSG/vsView.h++
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsCollision.o: ../../graphics/OSG/vsScene.h++
vsCollision.o: ../../graphics/OSG/vsLightAttribute.h++
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/IntersectVisitor
vsCollision.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LineSegment
vsCollision.o: ../../util/common/vsTimer.h++
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
vsDifferentialTrackedOrientation.o: ../../graphics/OSG/vsComponent.h++
vsDifferentialTrackedOrientation.o: ../../graphics/OSG/vsAttribute.h++
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsDifferentialTrackedOrientation.o: /usr/include/stdlib.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/waitflags.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/waitstatus.h
vsDifferentialTrackedOrientation.o: /usr/include/endian.h
vsDifferentialTrackedOrientation.o: /usr/include/bits/endian.h
vsDifferentialTrackedOrientation.o: /usr/include/alloca.h
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsDifferentialTrackedOrientation.o: ../../util/common/vsObjectMap.h++
vsDifferentialTrackedOrientation.o: ../../graphics/OSG/vsNode.h++
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsDifferentialTrackedOrientation.o: /usr/include/string.h
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsDifferentialTrackedOrientation.o: ../../graphics/OSG/vsTransformAttribute.h++
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsDifferentialTrackedOrientation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsDrivingMotion.o: ../../graphics/OSG/vsComponent.h++
vsDrivingMotion.o: ../../util/common/vsVector.h++
vsDrivingMotion.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsDrivingMotion.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsDrivingMotion.o: /usr/include/bits/mathcalls.h
vsDrivingMotion.o: ../../graphics/OSG/vsAttribute.h++
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsDrivingMotion.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsDrivingMotion.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsDrivingMotion.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsDrivingMotion.o: ../../util/common/vsObjectMap.h++
vsDrivingMotion.o: ../../graphics/OSG/vsNode.h++
vsDrivingMotion.o: ../../util/common/vsMatrix.h++
vsDrivingMotion.o: ../../util/common/vsQuat.h++
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsDrivingMotion.o: /usr/include/string.h
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsDrivingMotion.o: ../../graphics/OSG/vsTransformAttribute.h++
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsDrivingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsDrivingMotion.o: ../../io/common/vsMouse.h++
vsDrivingMotion.o: ../../io/common/vsInputDevice.h++
vsDrivingMotion.o: ../../io/common/vsInputAxis.h++
vsDrivingMotion.o: ../../io/common/vsInputButton.h++
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
vsFlyingMotion.o: ../../graphics/OSG/vsComponent.h++
vsFlyingMotion.o: ../../util/common/vsVector.h++
vsFlyingMotion.o: ../../graphics/OSG/vsAttribute.h++
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsFlyingMotion.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsFlyingMotion.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsFlyingMotion.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsFlyingMotion.o: ../../util/common/vsObjectMap.h++
vsFlyingMotion.o: ../../graphics/OSG/vsNode.h++
vsFlyingMotion.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsFlyingMotion.o: /usr/include/string.h
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsFlyingMotion.o: ../../graphics/OSG/vsTransformAttribute.h++
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsFlyingMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsFlyingMotion.o: ../../util/common/vsTimer.h++
vsKinematics.o: /usr/include/stdio.h /usr/include/features.h
vsKinematics.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsKinematics.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsKinematics.o: /usr/include/bits/types.h /usr/include/libio.h
vsKinematics.o: /usr/include/_G_config.h /usr/include/wchar.h
vsKinematics.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsKinematics.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsKinematics.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsKinematics.o: /usr/include/getopt.h ../../util/common/vsTimer.h++
vsKinematics.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsKinematics.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsKinematics.o: /usr/include/bits/mathcalls.h vsKinematics.h++
vsKinematics.o: ../../util/common/vsUpdatable.h++
vsKinematics.o: ../../util/common/vsObject.h++
vsKinematics.o: ../../util/common/vsTreeMap.h++
vsKinematics.o: ../../util/common/vsGrowableArray.h++
vsKinematics.o: ../../graphics/OSG/vsComponent.h++
vsKinematics.o: ../../util/common/vsVector.h++
vsKinematics.o: ../../graphics/OSG/vsAttribute.h++
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsKinematics.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsKinematics.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsKinematics.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsKinematics.o: ../../util/common/vsObjectMap.h++
vsKinematics.o: ../../graphics/OSG/vsNode.h++ ../../util/common/vsMatrix.h++
vsKinematics.o: ../../util/common/vsQuat.h++
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsKinematics.o: /usr/include/string.h
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsKinematics.o: ../../graphics/OSG/vsTransformAttribute.h++
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsKinematics.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsSphericalMotion.o: ../../graphics/OSG/vsComponent.h++
vsSphericalMotion.o: ../../util/common/vsVector.h++
vsSphericalMotion.o: ../../graphics/OSG/vsAttribute.h++
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsSphericalMotion.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsSphericalMotion.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsSphericalMotion.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsSphericalMotion.o: ../../util/common/vsObjectMap.h++
vsSphericalMotion.o: ../../graphics/OSG/vsNode.h++
vsSphericalMotion.o: ../../util/common/vsMatrix.h++
vsSphericalMotion.o: ../../util/common/vsQuat.h++
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsSphericalMotion.o: /usr/include/string.h
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsSphericalMotion.o: ../../graphics/OSG/vsTransformAttribute.h++
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsSphericalMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsTerrainFollow.o: ../../graphics/OSG/vsComponent.h++
vsTerrainFollow.o: ../../util/common/vsVector.h++
vsTerrainFollow.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsTerrainFollow.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsTerrainFollow.o: /usr/include/bits/mathcalls.h
vsTerrainFollow.o: ../../graphics/OSG/vsAttribute.h++
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTerrainFollow.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsTerrainFollow.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsTerrainFollow.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTerrainFollow.o: ../../util/common/vsObjectMap.h++
vsTerrainFollow.o: ../../graphics/OSG/vsNode.h++
vsTerrainFollow.o: ../../util/common/vsMatrix.h++
vsTerrainFollow.o: ../../util/common/vsQuat.h++
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTerrainFollow.o: /usr/include/string.h
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsTerrainFollow.o: ../../graphics/OSG/vsTransformAttribute.h++
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsTerrainFollow.o: ../../graphics/OSG/vsIntersect.h++
vsTerrainFollow.o: ../../graphics/OSG/vsGeometry.h++
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsTerrainFollow.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsTerrainFollow.o: ../../graphics/OSG_X/vsPane.h++
vsTerrainFollow.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/X11/Xlib.h
vsTerrainFollow.o: /usr/include/sys/types.h /usr/include/time.h
vsTerrainFollow.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsTerrainFollow.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsTerrainFollow.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsTerrainFollow.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsTerrainFollow.o: /usr/include/GL/glx.h /usr/include/X11/Xutil.h
vsTerrainFollow.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
vsTerrainFollow.o: /usr/include/GL/glxtokens.h
vsTerrainFollow.o: ../../graphics/OSG_X/vsScreen.h++
vsTerrainFollow.o: ../../graphics/OSG_X/vsPipe.h++
vsTerrainFollow.o: ../../graphics/OSG/vsView.h++
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsTerrainFollow.o: ../../graphics/OSG/vsScene.h++
vsTerrainFollow.o: ../../graphics/OSG/vsLightAttribute.h++
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/IntersectVisitor
vsTerrainFollow.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LineSegment
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
vsTrackballMotion.o: ../../graphics/OSG/vsComponent.h++
vsTrackballMotion.o: ../../util/common/vsVector.h++
vsTrackballMotion.o: ../../graphics/OSG/vsAttribute.h++
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTrackballMotion.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsTrackballMotion.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsTrackballMotion.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTrackballMotion.o: ../../util/common/vsObjectMap.h++
vsTrackballMotion.o: ../../graphics/OSG/vsNode.h++
vsTrackballMotion.o: ../../util/common/vsMatrix.h++
vsTrackballMotion.o: ../../util/common/vsQuat.h++
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTrackballMotion.o: /usr/include/string.h
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsTrackballMotion.o: ../../graphics/OSG/vsTransformAttribute.h++
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsTrackballMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsTrackedMotion.o: ../../graphics/OSG/vsComponent.h++
vsTrackedMotion.o: ../../graphics/OSG/vsAttribute.h++
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsTrackedMotion.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsTrackedMotion.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsTrackedMotion.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsTrackedMotion.o: ../../util/common/vsObjectMap.h++
vsTrackedMotion.o: ../../graphics/OSG/vsNode.h++
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsTrackedMotion.o: /usr/include/string.h
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsTrackedMotion.o: ../../graphics/OSG/vsTransformAttribute.h++
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsTrackedMotion.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsWalkArticulation.o: /usr/include/stdio.h /usr/include/features.h
vsWalkArticulation.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWalkArticulation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWalkArticulation.o: /usr/include/bits/types.h /usr/include/libio.h
vsWalkArticulation.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWalkArticulation.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWalkArticulation.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWalkArticulation.o: /usr/include/bits/stdio_lim.h
vsWalkArticulation.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWalkArticulation.o: ../../util/common/vsTimer.h++
vsWalkArticulation.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWalkArticulation.o: /usr/include/bits/huge_val.h
vsWalkArticulation.o: /usr/include/bits/mathdef.h
vsWalkArticulation.o: /usr/include/bits/mathcalls.h vsWalkArticulation.h++
vsWalkArticulation.o: vsMotionModel.h++ ../../util/common/vsUpdatable.h++
vsWalkArticulation.o: ../../util/common/vsObject.h++
vsWalkArticulation.o: ../../util/common/vsTreeMap.h++
vsWalkArticulation.o: ../../util/common/vsGrowableArray.h++ vsKinematics.h++
vsWalkArticulation.o: ../../graphics/OSG/vsComponent.h++
vsWalkArticulation.o: ../../util/common/vsVector.h++
vsWalkArticulation.o: ../../graphics/OSG/vsAttribute.h++
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsWalkArticulation.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsWalkArticulation.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsWalkArticulation.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsWalkArticulation.o: ../../util/common/vsObjectMap.h++
vsWalkArticulation.o: ../../graphics/OSG/vsNode.h++
vsWalkArticulation.o: ../../util/common/vsMatrix.h++
vsWalkArticulation.o: ../../util/common/vsQuat.h++
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsWalkArticulation.o: /usr/include/string.h
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsWalkArticulation.o: ../../graphics/OSG/vsTransformAttribute.h++
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsWalkArticulation.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsWalkInPlace.o: ../../graphics/OSG/vsComponent.h++
vsWalkInPlace.o: ../../graphics/OSG/vsAttribute.h++
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsWalkInPlace.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsWalkInPlace.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsWalkInPlace.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsWalkInPlace.o: ../../util/common/vsObjectMap.h++
vsWalkInPlace.o: ../../graphics/OSG/vsNode.h++
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsWalkInPlace.o: /usr/include/string.h
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsWalkInPlace.o: ../../graphics/OSG/vsTransformAttribute.h++
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsWalkInPlace.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsWalkInPlace.o: ../../util/common/vsTimer.h++
