#!/bin/make
# makefile for system/OSG

.SUFFIXES: .c .c++ .o .h .h++

INCDIRS = -I. \
	  -I../../avatar/common \
	  -I../../motion/common \
	  -I../../io/common \
	  -I../../io/$(OS) \
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
          vsSystem.o

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

vsSystem.o: vsSystem.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/FrameStamp
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Referenced
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Export
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MemoryManager
vsSystem.o: /usr/include/stdlib.h /usr/include/features.h
vsSystem.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stddef.h
vsSystem.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
vsSystem.o: /usr/include/endian.h /usr/include/bits/endian.h
vsSystem.o: /usr/include/alloca.h ../../graphics/OSG_X/vsPipe.h++
vsSystem.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
vsSystem.o: /usr/include/bits/types.h /usr/include/time.h
vsSystem.o: /usr/include/sys/select.h /usr/include/bits/select.h
vsSystem.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
vsSystem.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
vsSystem.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
vsSystem.o: ../../util/common/vsGlobals.h++ /usr/include/math.h
vsSystem.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
vsSystem.o: /usr/include/bits/mathcalls.h ../../graphics/OSG_X/vsScreen.h++
vsSystem.o: ../../graphics/OSG_X/vsWindow.h++ /usr/include/GL/glx.h
vsSystem.o: /usr/include/X11/Xutil.h /usr/include/GL/gl.h
vsSystem.o: /usr/include/GL/glext.h /usr/include/GL/glxtokens.h
vsSystem.o: ../../graphics/OSG_X/vsPane.h++ ../../graphics/OSG/vsView.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Camera
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ref_ptr
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Matrix
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Object
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CopyOp
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec3
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Math
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec4
vsSystem.o: /usr/include/string.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Quat
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Viewport
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateAttribute
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/GL
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Types
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DisplaySettings
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Polytope
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Plane
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingSphere
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/BoundingBox
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/float.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/fast_back_stack
vsSystem.o: ../../util/common/vsVector.h++ /usr/include/stdio.h
vsSystem.o: /usr/include/libio.h /usr/include/_G_config.h
vsSystem.o: /usr/include/wchar.h /usr/include/bits/wchar.h
vsSystem.o: /usr/include/gconv.h
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/stdarg.h
vsSystem.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
vsSystem.o: /usr/include/getopt.h ../../util/common/vsMatrix.h++
vsSystem.o: ../../util/common/vsQuat.h++ ../../graphics/OSG/vsNode.h++
vsSystem.o: ../../util/common/vsObject.h++ ../../util/common/vsTreeMap.h++
vsSystem.o: ../../util/common/vsGrowableArray.h++
vsSystem.o: ../../graphics/OSG/vsAttribute.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/StateSet
vsSystem.o: ../../util/common/vsObjectMap.h++ ../../graphics/OSG/vsScene.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Group
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Node
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeCallback
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/NodeVisitor
vsSystem.o: ../../graphics/OSG/vsLightAttribute.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Light
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LightSource
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/State
vsSystem.o: ../../graphics/OSG/vsComponent.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/SceneView
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/CullVisitor
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Drawable
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Shape
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Array
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Vec2
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/UByte4
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/buffered_value
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Impostor
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/LOD
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ImpostorSprite
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AlphaFunc
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/TexEnv
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ClearNode
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Notify
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullStack
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/CullingSet
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ShadowVolumeOccluder
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarOccluder
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ConvexPlanarPolygon
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderGraph
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderLeaf
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/Export
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStage
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/ColorMask
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderBin
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osgUtil/RenderStageLighting
vsSystem.o: ../../util/common/vsTimer.h++ /usr/include/sys/time.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geode
vsSystem.o: ../../graphics/OSG/vsLocalLightCallback.h++
vsSystem.o: ../../graphics/OSG/vsGeometry.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Geometry
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/PrimitiveSet
vsSystem.o: ../../graphics/common/vsOptimizer.h++
vsSystem.o: ../../graphics/OSG/vsGraphicsState.h++
vsSystem.o: ../../graphics/OSG/vsTransparencyAttribute.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Depth
vsSystem.o: ../../graphics/OSG/vsStateAttribute.h++
vsSystem.o: ../../graphics/OSG/vsDatabaseLoader.h++
vsSystem.o: ../../graphics/OSG/vsViewpointAttribute.h++
vsSystem.o: ../../graphics/OSG/vsTextBuilder.h++ /usr/include/GL/glu.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTVectoriser.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTGL.h
vsSystem.o: /usr/include/ft2build.h
vsSystem.o: /usr/include/freetype2/freetype/config/ftheader.h
vsSystem.o: /usr/include/freetype2/freetype/internal/internal.h
vsSystem.o: /usr/include/freetype2/freetype/freetype.h
vsSystem.o: /usr/include/freetype2/freetype/config/ftconfig.h
vsSystem.o: /usr/include/freetype2/freetype/config/ftoption.h
vsSystem.o: /usr/include/freetype2/freetype/config/ftstdlib.h
vsSystem.o: /usr/include/limits.h
vsSystem.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2/include/limits.h
vsSystem.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
vsSystem.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
vsSystem.o: /usr/include/bits/xopen_lim.h /usr/include/ctype.h
vsSystem.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h
vsSystem.o: /usr/include/freetype2/freetype/fterrors.h
vsSystem.o: /usr/include/freetype2/freetype/ftmoderr.h
vsSystem.o: /usr/include/freetype2/freetype/fterrdef.h
vsSystem.o: /usr/include/freetype2/freetype/fttypes.h
vsSystem.o: /usr/include/freetype2/freetype/ftsystem.h
vsSystem.o: /usr/include/freetype2/freetype/ftimage.h
vsSystem.o: /usr/include/freetype2/freetype/ftglyph.h
vsSystem.o: /usr/include/freetype2/freetype/freetype.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTGlyph.h
vsSystem.o: /usr/include/freetype2/freetype/ftglyph.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTFace.h
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/src/osgText/FTSize.h
vsSystem.o: ../../graphics/OSG/vsTransformAttribute.h++
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/MatrixTransform
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/Transform
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/AnimationPath
vsSystem.o: /ist/projects/nve/usr/jdaly/osg/OpenSceneGraph/include/osg/DOFTransform
