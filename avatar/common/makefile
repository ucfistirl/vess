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
vsAvatar.o: ../../graphics/OSG/vsNode.h++ ../../util/common/vsVector.h++
vsAvatar.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsAvatar.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsAvatar.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsAvatar.o: ../../util/common/vsQuat.h++ ../../graphics/OSG/vsAttribute.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsAvatar.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsAvatar.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsAvatar.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsAvatar.o: ../../util/common/vsObjectMap.h++
vsAvatar.o: ../../graphics/common/vsOptimizer.h++
vsAvatar.o: ../../graphics/OSG/vsGeometry.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsAvatar.o: /usr/include/string.h
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsAvatar.o: ../../graphics/OSG/vsComponent.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsAvatar.o: ../../graphics/OSG/vsDatabaseLoader.h++
vsAvatar.o: ../../io/common/vsISTJoystickBox.h++
vsAvatar.o: ../../io/common/vsJoystick.h++ ../../io/common/vsInputDevice.h++
vsAvatar.o: ../../io/common/vsInputAxis.h++ ../../io/common/vsInputButton.h++
vsAvatar.o: ../../io/common/vsJoystickBox.h++
vsAvatar.o: ../../io/common/vsInputSystem.h++ ../../io/Linux/vsSerialPort.h++
vsAvatar.o: /usr/include/termios.h /usr/include/bits/termios.h
vsAvatar.o: /usr/include/sys/ttydefaults.h /usr/include/unistd.h
vsAvatar.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsAvatar.o: ../../io/common/vsUnwinder.h++ ../../io/common/vsFlockOfBirds.h++
vsAvatar.o: ../../io/Linux/vsAscensionSerialTrackingSystem.h++
vsAvatar.o: ../../io/common/vsMotionTracker.h++
vsAvatar.o: ../../io/common/vs6DInputDevice.h++
vsAvatar.o: ../../io/common/vsTrackingSystem.h++
vsAvatar.o: ../../io/common/vsSharedInputData.h++ /usr/include/sys/types.h
vsAvatar.o: /usr/include/time.h /usr/include/sys/select.h
vsAvatar.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
vsAvatar.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
vsAvatar.o: /usr/include/sys/ipc.h /usr/include/bits/ipc.h
vsAvatar.o: /usr/include/sys/shm.h /usr/include/bits/shm.h
vsAvatar.o: /usr/include/sys/sem.h /usr/include/bits/sem.h
vsAvatar.o: ../../io/common/vsSerialMotionStar.h++
vsAvatar.o: ../../io/Linux/vsFastrak.h++ ../../io/Linux/vsIS600.h++
vsAvatar.o: ../../io/Linux/vsEthernetMotionStar.h++
vsAvatar.o: ../../io/common/vsUDPUnicastNetworkInterface.h++
vsAvatar.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
vsAvatar.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
vsAvatar.o: /usr/include/limits.h
vsAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsAvatar.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsAvatar.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsAvatar.o: /usr/include/bits/xopen_lim.h /usr/include/bits/sockaddr.h
vsAvatar.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
vsAvatar.o: /usr/include/netinet/in.h /usr/include/stdint.h
vsAvatar.o: /usr/include/bits/wordsize.h /usr/include/bits/in.h
vsAvatar.o: /usr/include/bits/byteswap.h /usr/include/arpa/inet.h
vsAvatar.o: /usr/include/netdb.h /usr/include/rpc/netdb.h
vsAvatar.o: /usr/include/bits/netdb.h
vsAvatar.o: ../../io/common/vsUDPNetworkInterface.h++ /usr/include/fcntl.h
vsAvatar.o: /usr/include/bits/fcntl.h /usr/include/sys/stat.h
vsAvatar.o: /usr/include/bits/stat.h ../../io/common/vsNetworkInterface.h++
vsAvatar.o: /usr/include/sys/time.h /usr/include/errno.h
vsAvatar.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vsAvatar.o: /usr/include/asm/errno.h ../../io/X/vsWSSpaceball.h++
vsAvatar.o: ../../io/common/vsSpaceball.h++ ../../io/OSG_X/vsWindowSystem.h++
vsAvatar.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
vsAvatar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsAvatar.o: /usr/include/X11/Xutil.h /usr/include/X11/Xos.h
vsAvatar.o: /usr/include/X11/Xarch.h ../../graphics/OSG_X/vsWindow.h++
vsAvatar.o: /usr/include/GL/glx.h /usr/include/GL/gl.h
vsAvatar.o: /usr/include/GL/glext.h /usr/include/GL/glxtokens.h
vsAvatar.o: ../../graphics/OSG_X/vsScreen.h++ ../../graphics/OSG_X/vsPipe.h++
vsAvatar.o: ../../graphics/OSG_X/vsPane.h++ ../../graphics/OSG/vsView.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsAvatar.o: ../../graphics/OSG/vsScene.h++
vsAvatar.o: ../../graphics/OSG/vsLightAttribute.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
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
vsAvatar.o: ../../graphics/OSG/vsTransformAttribute.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsAvatar.o: ../../motion/common/vsAxisRotation.h++
vsAvatar.o: ../../motion/common/vsCollision.h++
vsAvatar.o: ../../graphics/OSG/vsIntersect.h++
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/IntersectVisitor
vsAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LineSegment
vsAvatar.o: ../../motion/common/vsDrivingMotion.h++
vsAvatar.o: ../../motion/common/vsFlyingMotion.h++
vsAvatar.o: ../../motion/common/vsDifferentialTrackedOrientation.h++
vsAvatar.o: ../../motion/common/vsTerrainFollow.h++
vsAvatar.o: ../../motion/common/vsTrackballMotion.h++
vsAvatar.o: ../../motion/common/vsTrackedMotion.h++
vsAvatar.o: ../../motion/common/vsWalkArticulation.h++
vsAvatar.o: ../../motion/common/vsWalkInPlace.h++
vsAvatar.o: ../../graphics/OSG/vsViewpointAttribute.h++
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
vsConfigAvatar.o: ../../graphics/OSG/vsNode.h++
vsConfigAvatar.o: ../../util/common/vsVector.h++
vsConfigAvatar.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsConfigAvatar.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsConfigAvatar.o: /usr/include/bits/mathcalls.h
vsConfigAvatar.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsConfigAvatar.o: ../../graphics/OSG/vsAttribute.h++
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsConfigAvatar.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsConfigAvatar.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsConfigAvatar.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsConfigAvatar.o: ../../util/common/vsObjectMap.h++
vsConfigAvatar.o: ../../graphics/common/vsOptimizer.h++
vsConfigAvatar.o: ../../graphics/OSG/vsGeometry.h++
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsConfigAvatar.o: /usr/include/string.h
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsConfigAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsConfigAvatar.o: ../../graphics/OSG/vsComponent.h++
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsConfigAvatar.o: ../../motion/common/vsKinematics.h++
vsConfigAvatar.o: ../../graphics/OSG/vsTransformAttribute.h++
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsConfigAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
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
vsGhostFlyAvatar.o: ../../graphics/OSG/vsNode.h++
vsGhostFlyAvatar.o: ../../util/common/vsVector.h++
vsGhostFlyAvatar.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsGhostFlyAvatar.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsGhostFlyAvatar.o: /usr/include/bits/mathcalls.h
vsGhostFlyAvatar.o: ../../util/common/vsMatrix.h++
vsGhostFlyAvatar.o: ../../util/common/vsQuat.h++
vsGhostFlyAvatar.o: ../../graphics/OSG/vsAttribute.h++
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsGhostFlyAvatar.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsGhostFlyAvatar.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
vsGhostFlyAvatar.o: /usr/include/bits/endian.h /usr/include/alloca.h
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsGhostFlyAvatar.o: ../../util/common/vsObjectMap.h++
vsGhostFlyAvatar.o: ../../graphics/common/vsOptimizer.h++
vsGhostFlyAvatar.o: ../../graphics/OSG/vsGeometry.h++
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsGhostFlyAvatar.o: /usr/include/string.h
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsGhostFlyAvatar.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsGhostFlyAvatar.o: ../../graphics/OSG/vsComponent.h++
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsGhostFlyAvatar.o: ../../io/common/vsMouse.h++
vsGhostFlyAvatar.o: ../../io/common/vsInputDevice.h++
vsGhostFlyAvatar.o: ../../io/common/vsInputAxis.h++
vsGhostFlyAvatar.o: ../../io/common/vsInputButton.h++
vsGhostFlyAvatar.o: ../../motion/common/vsKinematics.h++
vsGhostFlyAvatar.o: ../../graphics/OSG/vsTransformAttribute.h++
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
vsGhostFlyAvatar.o: ../../motion/common/vsFlyingMotion.h++
vsGhostFlyAvatar.o: ../../motion/common/vsMotionModel.h++
vsGhostFlyAvatar.o: ../../io/OSG_X/vsWindowSystem.h++ /usr/include/X11/Xlib.h
vsGhostFlyAvatar.o: /usr/include/sys/types.h /usr/include/time.h
vsGhostFlyAvatar.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsGhostFlyAvatar.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsGhostFlyAvatar.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsGhostFlyAvatar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsGhostFlyAvatar.o: /usr/include/X11/Xutil.h /usr/include/X11/Xos.h
vsGhostFlyAvatar.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
vsGhostFlyAvatar.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
vsGhostFlyAvatar.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
vsGhostFlyAvatar.o: /usr/include/bits/confname.h /usr/include/sys/time.h
vsGhostFlyAvatar.o: /usr/include/X11/Xarch.h
vsGhostFlyAvatar.o: ../../io/common/vsInputSystem.h++
vsGhostFlyAvatar.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/GL/glx.h
vsGhostFlyAvatar.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
vsGhostFlyAvatar.o: /usr/include/GL/glxtokens.h
vsGhostFlyAvatar.o: ../../graphics/OSG_X/vsScreen.h++
vsGhostFlyAvatar.o: ../../graphics/OSG_X/vsPipe.h++
vsGhostFlyAvatar.o: ../../graphics/OSG_X/vsPane.h++
vsGhostFlyAvatar.o: ../../graphics/OSG/vsView.h++
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsGhostFlyAvatar.o: ../../graphics/OSG/vsScene.h++
vsGhostFlyAvatar.o: ../../graphics/OSG/vsLightAttribute.h++
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsGhostFlyAvatar.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsGhostFlyAvatar.o: ../../io/common/vsKeyboard.h++ /usr/include/X11/keysym.h
vsGhostFlyAvatar.o: /usr/include/X11/keysymdef.h
vsGhostFlyAvatar.o: ../../graphics/OSG/vsViewpointAttribute.h++
