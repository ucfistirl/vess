#!/bin/make
# makefile for io/X

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../io/common \
	  -I../../io/$(SG)_$(WS) \
	  -I../../sound/$(AL) \
	  -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsWSSpaceball.o

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

vsWSSpaceball.o: /usr/include/stdio.h /usr/include/features.h
vsWSSpaceball.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWSSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWSSpaceball.o: /usr/include/bits/types.h /usr/include/libio.h
vsWSSpaceball.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWSSpaceball.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWSSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWSSpaceball.o: /usr/include/bits/stdio_lim.h
vsWSSpaceball.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWSSpaceball.o: vsWSSpaceball.h++ ../../io/common/vsInputSystem.h++
vsWSSpaceball.o: ../../util/common/vsUpdatable.h++
vsWSSpaceball.o: ../../util/common/vsObject.h++
vsWSSpaceball.o: ../../util/common/vsTreeMap.h++
vsWSSpaceball.o: ../../util/common/vsGrowableArray.h++
vsWSSpaceball.o: ../../io/common/vsSpaceball.h++
vsWSSpaceball.o: ../../io/common/vs6DInputDevice.h++
vsWSSpaceball.o: ../../util/common/vsVector.h++
vsWSSpaceball.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWSSpaceball.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsWSSpaceball.o: /usr/include/bits/mathcalls.h ../../util/common/vsMatrix.h++
vsWSSpaceball.o: ../../util/common/vsQuat.h++
vsWSSpaceball.o: ../../io/common/vsInputDevice.h++
vsWSSpaceball.o: ../../io/common/vsInputAxis.h++
vsWSSpaceball.o: ../../io/common/vsInputButton.h++
vsWSSpaceball.o: ../../io/OSG_X/vsWindowSystem.h++ /usr/include/X11/Xlib.h
vsWSSpaceball.o: /usr/include/sys/types.h /usr/include/time.h
vsWSSpaceball.o: /usr/include/endian.h /usr/include/bits/endian.h
vsWSSpaceball.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsWSSpaceball.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsWSSpaceball.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsWSSpaceball.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsWSSpaceball.o: /usr/include/X11/Xutil.h /usr/include/X11/Xos.h
vsWSSpaceball.o: /usr/include/string.h /usr/include/fcntl.h
vsWSSpaceball.o: /usr/include/bits/fcntl.h /usr/include/sys/stat.h
vsWSSpaceball.o: /usr/include/bits/stat.h /usr/include/unistd.h
vsWSSpaceball.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsWSSpaceball.o: /usr/include/sys/time.h /usr/include/X11/Xarch.h
vsWSSpaceball.o: ../../util/common/vsObjectMap.h++
vsWSSpaceball.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/GL/glx.h
vsWSSpaceball.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
vsWSSpaceball.o: /usr/include/GL/glxtokens.h
vsWSSpaceball.o: ../../graphics/OSG_X/vsScreen.h++
vsWSSpaceball.o: ../../graphics/OSG_X/vsPipe.h++
vsWSSpaceball.o: ../../graphics/OSG_X/vsPane.h++
vsWSSpaceball.o: ../../graphics/OSG/vsView.h++
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsWSSpaceball.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsWSSpaceball.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsWSSpaceball.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsWSSpaceball.o: ../../graphics/OSG/vsNode.h++
vsWSSpaceball.o: ../../graphics/OSG/vsAttribute.h++
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsWSSpaceball.o: ../../graphics/OSG/vsScene.h++
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsWSSpaceball.o: ../../graphics/OSG/vsLightAttribute.h++
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsWSSpaceball.o: ../../graphics/OSG/vsComponent.h++
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsWSSpaceball.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsWSSpaceball.o: ../../io/common/vsMouse.h++ ../../io/common/vsKeyboard.h++
vsWSSpaceball.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
vsWSSpaceball.o: /usr/include/X11/extensions/XI.h
vsWSSpaceball.o: /usr/include/X11/extensions/XInput.h
