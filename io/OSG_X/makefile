#!/bin/make
# makefile for io/X_OSG

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../io/common \
	  -I../../io/$(WS) \
	  -I../../sound/$(AL) \
          -I../../graphics/common \
	  -I../../graphics/$(SG) \
	  -I../../graphics/$(SG)_$(WS) \
	  -I../../util/common \
          -I/usr/include/g++-3 \
	  $(EXTRA_INCDIRS)

LIBOBJS = \
          vsWindowSystem.o

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

vsWindowSystem.o: /usr/include/stdio.h /usr/include/features.h
vsWindowSystem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsWindowSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsWindowSystem.o: /usr/include/bits/types.h /usr/include/libio.h
vsWindowSystem.o: /usr/include/_G_config.h /usr/include/wchar.h
vsWindowSystem.o: /usr/include/bits/wchar.h /usr/include/gconv.h
vsWindowSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsWindowSystem.o: /usr/include/bits/stdio_lim.h
vsWindowSystem.o: /usr/include/bits/sys_errlist.h /usr/include/getopt.h
vsWindowSystem.o: vsWindowSystem.h++ /usr/include/X11/Xlib.h
vsWindowSystem.o: /usr/include/sys/types.h /usr/include/time.h
vsWindowSystem.o: /usr/include/endian.h /usr/include/bits/endian.h
vsWindowSystem.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsWindowSystem.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsWindowSystem.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsWindowSystem.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsWindowSystem.o: /usr/include/X11/Xutil.h /usr/include/X11/Xos.h
vsWindowSystem.o: /usr/include/string.h /usr/include/fcntl.h
vsWindowSystem.o: /usr/include/bits/fcntl.h /usr/include/sys/stat.h
vsWindowSystem.o: /usr/include/bits/stat.h /usr/include/unistd.h
vsWindowSystem.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
vsWindowSystem.o: /usr/include/sys/time.h /usr/include/X11/Xarch.h
vsWindowSystem.o: ../../util/common/vsObjectMap.h++
vsWindowSystem.o: ../../util/common/vsTreeMap.h++
vsWindowSystem.o: ../../util/common/vsGrowableArray.h++
vsWindowSystem.o: ../../io/common/vsInputSystem.h++
vsWindowSystem.o: ../../util/common/vsUpdatable.h++
vsWindowSystem.o: ../../util/common/vsObject.h++
vsWindowSystem.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/GL/glx.h
vsWindowSystem.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
vsWindowSystem.o: /usr/include/GL/glxtokens.h
vsWindowSystem.o: ../../graphics/OSG_X/vsScreen.h++
vsWindowSystem.o: ../../graphics/OSG_X/vsPipe.h++
vsWindowSystem.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsWindowSystem.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsWindowSystem.o: /usr/include/bits/mathcalls.h
vsWindowSystem.o: ../../graphics/OSG_X/vsPane.h++
vsWindowSystem.o: ../../graphics/OSG/vsView.h++
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsWindowSystem.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
vsWindowSystem.o: /usr/include/bits/waitstatus.h /usr/include/alloca.h
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsWindowSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsWindowSystem.o: ../../util/common/vsVector.h++
vsWindowSystem.o: ../../util/common/vsMatrix.h++ ../../util/common/vsQuat.h++
vsWindowSystem.o: ../../graphics/OSG/vsNode.h++
vsWindowSystem.o: ../../graphics/OSG/vsAttribute.h++
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsWindowSystem.o: ../../graphics/OSG/vsScene.h++
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsWindowSystem.o: ../../graphics/OSG/vsLightAttribute.h++
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsWindowSystem.o: ../../graphics/OSG/vsComponent.h++
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsWindowSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsWindowSystem.o: ../../io/common/vsMouse.h++
vsWindowSystem.o: ../../io/common/vsInputDevice.h++
vsWindowSystem.o: ../../io/common/vsInputAxis.h++
vsWindowSystem.o: ../../io/common/vsInputButton.h++
vsWindowSystem.o: ../../io/common/vsKeyboard.h++ /usr/include/X11/keysym.h
vsWindowSystem.o: /usr/include/X11/keysymdef.h
