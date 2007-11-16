dnl Process this file with autoconf to produce a configure script.

AC_INIT(vess, 4.1.0, vess@ist.ucf.edu)
AC_CONFIG_SRCDIR(motion/common/vsDrivingMotion.c++)
AC_CONFIG_AUX_DIR(config) 

dnl Get the system name
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Use a config.h header
AM_CONFIG_HEADER(config.h)

dnl Initialize Automake
AM_INIT_AUTOMAKE

dnl Build environment******************************************************

dnl Checks for compiler/installer/make utility.  Look for native compilers
dnl first.
AC_PROG_CC(cc gcc)
AC_PROG_CXX(CC c++ g++)
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Basic CFLAGS, CXXFLAGS and LD_FLAGS.  Use the AM_* versions so the user
dnl can specify any additional flags he or she wants.
AM_CFLAGS="-O"
AM_CXXFLAGS="-O"
AM_LDFLAGS=

dnl Additional CFLAGS/CXXFLAGS based on system type
case "$target" in
   i*86-*-linux*)
   AM_CFLAGS="$AM_CFLAGS -g -D__linux__ -D_MISC_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE -Wno-deprecated"
   AM_CXXFLAGS="$AM_CXXFLAGS -g -D__linux__ -D_MISC_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE -Wno-deprecated" ;;
   mips-sgi-irix*)
   AM_CFLAGS="$AM_CFLAGS -g -xansi -DIRIX -n32 -DSGI"
   AM_CXXFLAGS="$AM_CXXFLAGS -g -xansi -DIRIX -n32 -DSGI"
   AM_LDFLAGS="-n32" ;;
   *)
   AC_MSG_ERROR(
[  Unsupported target $target.  VESS cannot be installed on this machine.]) ;;
esac

dnl Checks for certain (often problematic) header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h sys/time.h unistd.h)

dnl Operating system*******************************************************

dnl gcc/g++ doesn't seem to work well in IRIX, so we'll specify a 
dnl compiler to use.  The IRIX n32 ABI uses /usr/lib32 for its libraries, 
dnl and requires -n32 for a compiler argument, we'll need to know this for 
dnl the library tests.  Complicating this is the fact that the MIPSPro 7.3
dnl compiler needs additional options to specify the language conventions 
dnl (-LANG:std for STL support, required for Open Scene Graph version), 
dnl whereas 7.2.1 does not support these features at all. We have to set 
dnl the CFLAGS for this, so we'll save the current CFLAGS in a temporary 
dnl variable, and restore them when we're done with our tests.
OLDCFLAGS=
OLDCXXFLAGS=

dnl Additionally, we add the vsLinuxJoystickSystem, vsParallelPort, and
dnl vsScentAirSystem classes when compiling under Linux.
dnl Shared libraries are built differently on different systems, so we set 
dnl up variables to handle that (since we're not using libtool).  Finally, 
dnl we set the operating system variable (OS) to indicate the appropriate 
dnl operating system.
case "$target" in
   i*86-*-linux*)
       LIBDIR="lib"
       SO_LINK="-Wl,--whole-archive"
       SO_NO_LINK="-Wl,--no-whole-archive"
       EXTRA_LLIO_OBJECTS="vsParallelPort.$OBJEXT"
       EXTRA_IO_OBJECTS="vsLinuxJoystickSystem.$OBJEXT vsScentAirSystem.$OBJEXT"
       EXTRA_SCENT_OBJECTS="vsScentAirScent.$OBJEXT"
       OS=Linux ;;

   mips-sgi-irix*)
       CC=cc
       CXX=CC
       LIBDIR="lib32"
       SO_LINK="-all"
       SO_NO_LINK="-notall"
       EXTRA_LLIO_OBJECTS=
       EXTRA_IO_OBJECTS=
       EXTRA_SCENT_OBJECTS=
       OS=IRIX

       dnl Save the current CFLAGS so we can fix it after we're done
       dnl configuring.
       OLDCFLAGS="$CFLAGS"
       OLDCXXFLAGS="$CXXFLAGS"

       dnl Get the MIPSPro version
       AC_MSG_CHECKING([MIPSPro compiler version])
       MIPSPRO_MAJOR=`CC -version 2>&1 | awk '{print substr($4, 1, 1)}'`
       MIPSPRO_MINOR=`CC -version 2>&1 | awk '{print substr($4, 3, 1)}'`
       AC_MSG_RESULT([$MIPSPRO_MAJOR.$MIPSPRO_MINOR])

       dnl Bail out right now if we're not using MIPSPro 7.2 or later
       if test $MIPSPRO_MAJOR -lt 7 || test $MIPSPRO_MINOR -lt 2 ; then
           AC_MSG_ERROR([

    VESS under IRIX requires the MIPSPro compiler version 7.2 or later.
    Your compiler version is not sufficient to compile VESS.
    
    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
       fi

       dnl Add the n32 options to the CFLAGS so the library tests will
       dnl work correctly
       CFLAGS="$CFLAGS -n32"
       CXXFLAGS="$CXXFLAGS -n32" 

       dnl Also add the -LANG:std option to both CXXFLAGS for the library 
       dnl tests and AM_CXXFLAGS for the actual compiler options if we're 
       dnl using MIPSPro 7.3 or greater.  We'll also disable the same
       dnl warnings that Open Scene Graph disables in case we're using that
       dnl library.
       if test $MIPSPRO_MAJOR -gt 7 || test $MIPSPRO_MINOR -ge 3 ; then
           CXXFLAGS="$CXXFLAGS -LANG:std -DEBUG:woff=1682 -DEBUG:woff=3303"
           AM_CXXFLAGS="$AM_CXXFLAGS -LANG:std -DEBUG:woff=1682 -DEBUG:woff=3303"
       fi
       ;;

   *)
       AC_MSG_ERROR(
[  Unsupported target $target.  VESS cannot be installed on this machine.]) ;;
esac

dnl Export the OS-specific variables to the makefiles
AC_SUBST(EXTRA_LLIO_OBJECTS)
AC_SUBST(EXTRA_IO_OBJECTS)
AC_SUBST(EXTRA_SCENT_OBJECTS)
AC_SUBST(SO_LINK)
AC_SUBST(SO_NO_LINK)

dnl Window system**********************************************************

dnl Only X supported at the moment.  Set the window system variable to
dnl indicate X.
WS=X

dnl Where is X??  Find it and export the include and lib directories
AC_PATH_X

dnl If the X paths returned are empty, fix them to be /usr/include and
dnl /usr/lib to avoid compilation problems on certain platforms
if test x$x_includes = "x" ; then
    x_includes=/usr/include
fi
if test x$x_libraries = "x" ; then
    x_libraries=/usr/$LIBDIR
fi

dnl Export the X include and library paths
AC_SUBST(x_includes)
AC_SUBST(x_libraries)

dnl Check the X libraries to make sure they work
LIBS=
AC_CHECK_LIB(X11, XGetWindowAttributes,,,-L$x_libraries -L/usr/$LIBDIR)
AC_CHECK_LIB(Xext, XextFindDisplay,,,-L$x_libraries -L/usr/$LIBDIR)
AC_CHECK_LIB(Xi, XSelectExtensionEvent,,,-L$x_libraries -L/usr/$LIBDIR)
AC_CHECK_LIB(Xm, XmTextSetString,,,-L$x_libraries -lXt -L/usr/$LIBDIR)
AC_CHECK_LIB(Xmu, XmuCvtStringToJustify,,,-L$x_libraries -L/usr/$LIBDIR)
AC_CHECK_LIB(Xt, XtFree,,,-L$x_libraries -L/usr/$LIBDIR)

dnl Graphics options*******************************************************

dnl Default to using Performer. Set the scene graph environment variable 
dnl to indicate Performer
SG=Performer

dnl This variable is used for any extra classes that need compiling aside
dnl from the standard VESS classes.
EXTRA_GRAPHICS_OBJECTS=

dnl Initialize the graphics include and library paths
GFX_INCLUDE_DIR=
GFX_LIB_DIR=
GFX_LIB=

dnl Provide the option to use ATLAS
dnl Set the ATLAS variable to the given directory
AC_ARG_WITH(atlas,
[[  --with-atlas=[PREFIX]        Path for ATLAS library [/usr/local]]],
, with_atlas=/usr/local)

dnl Provide the option to use Performer.  Set the scene graph variable
dnl to indicate Performer if this option is chosen.
AC_ARG_WITH(performer,
[[  --with-performer        Use OpenGL Performer for graphics [yes]]],
SG=Performer, SG=Performer; with_performer=yes)

dnl Provide the option to use Open Scene Graph (and specify a path for it).
dnl Set the scene graph variable to indicate OSG if this option is chosen.
AC_ARG_WITH(osg,
[[  --with-osg=[PREFIX]     Use Open Scene Graph for graphics (if necessary,
                          the path to the libraries may be specified) [no]]],
SG=OSG, with_osg=no)

dnl Provide the path for the OpenThreads library. This is required for OSG
dnl operation. Defaults to /usr/local.
AC_ARG_WITH(openthreads,
[[  --with-openthreads=[PREFIX]     Path for OpenThreads library [/usr/local]]],
, with_openthreads="/usr/local")

dnl Provide the option to use osgNV for programmable shader support under
dnl Open Scene Graph
AC_ARG_WITH(osgnv,
[[  --with-osgnv=[PREFIX]   Path for osgNV library [no]]],, with_osgnv=no)


dnl Check to see if it can find ATLAS
dnl Check to make sure that the OpenThreads library is present. Use the path specified
dnl in --with-openthreads, or default to /usr/local if not specified. (Only checks for
dnl the existance of the library file, as there isn't any good C function in the library
dnl to check for.)
AC_CHECK_FILE($with_atlas/libatlas.so,,ATLAS_ERROR=yes)

dnl If there was an error, abort; this library is required for OSG to compile.
if test x$ATLAS_ERROR = xyes ; then
   AC_MSG_ERROR([

There is a problem with the ATLAS library.  ATLAS is required 
for VESS to work properly.

For further support, consult the VESS User's Guide or join the 
vess-users mailing list at:

http://lists.sourceforge.net/lists/listinfo/vess-users

]
   )
fi

ATLAS_INCLUDE_DIR="-I$x_includes -I$with_atlas/communication -I$with_atlas/container -I$with_atlas/foundation -I$with_atlas/math -I$with_atlas/xml -I$with_atlas/util"
ATLAS_LIB_DIR="-L$x_libraries -L$with_atlas"
ATLAS_LIB="-latlas"



dnl Figure out which graphics library we're using
AC_MSG_CHECKING([which graphics library to use])
case "$SG" in
    Performer)
        dnl Print that we're using Performer for graphics
        AC_MSG_RESULT(Performer)

        dnl Performer can only be in one place (/usr/include).  Configure 
        dnl the include and library directories accordingly.
        GFX_INCLUDE_DIR="$GFX_INCLUDE_DIR -I$x_includes"
        GFX_LIB_DIR="$GFX_LIB_DIR -L$x_libraries -L/usr/$LIBDIR -L/usr/$LIBDIR/libpfdb"
        GFX_LIB="$GFX_LIB -lpf -lpfflt -lpfutil -lpfdu -lpfui -lXi -lXm -lXt -lGLU -lGL -lXext -lXmu -lX11 -ldl"

        dnl If we're on an IRIX box, we also need to link libimage
        case "$target" in
            mips-sgi-irix*)
                GFX_LIB="$GFX_LIB -limage" ;;
            *) ;;
        esac

        dnl If we're using gcc (or g++), add -Wno-deprecated to the CXXFLAGS 
        dnl so we don't get a hundred warnings about using STL the wrong way
        if test x$CXX = gcc || test x$CXX = g++ ; then 
            AM_CXXFLAGS="$AM_CXXFLAGS -Wno-deprecated"
        fi

        dnl Look for Performer
        GFX_ERROR=no
        LIBS=
        AC_CHECK_LIB(pf, pfConfig,,GFX_ERROR=yes,-L$x_libraries -L/usr/$LIBDIR)
        AC_CHECK_LIB(pfdu, pfdLoadFile,,GFX_ERROR=yes,-L$x_libraries -L/usr/$LIBDIR -lpf -lpfutil)
        AC_CHECK_LIB(pfutil, pfuInit,,GFX_ERROR=yes,-L$x_libraries -L/usr/$LIBDIR -lpf -lpfdu)
        AC_CHECK_LIB(pfui, pfiPickMode,,GFX_ERROR=yes,-L$x_libraries -L/usr/$LIBDIR -lpf -lpfutil -lpfui)
        AC_CHECK_LIB(pfflt, pfdConverterMode_flt,,GFX_ERROR=yes,-L$x_libraries -L/usr/$LIBDIR $GFX_LIB_DIR -lpf -lpfdu)

        dnl Add the extra objects that Performer requires
        EXTRA_GRAPHICS_OBJECTS="$EXTRA_GRAPHICS_OBJECTS vsCallbackList.$OBJEXT"

        if test x$GFX_ERROR = xyes ; then
            AC_MSG_ERROR([

    There is a problem with OpenGL Performer.  Either Performer is not
    installed, or the libraries are installed in a non-standard location.

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

]
            )
        fi ;;
    OSG)
        dnl Print that we're using OSG for graphics
        AC_MSG_RESULT(OpenSceneGraph)

        dnl Check to see if the user specified a path to OSG, use
        dnl a default path of /usr/local if not
        AC_MSG_CHECKING([path for OpenSceneGraph])
        if test x$with_osg = xyes ; then
            OSG_PATH=/usr/local
        else
            OSG_PATH=$with_osg
        fi
        AC_MSG_RESULT($OSG_PATH)

        dnl Check to make sure that the OpenThreads library is present. Use the path specified
        dnl in --with-openthreads, or default to /usr/local if not specified. (Only checks for
        dnl the existance of the library file, as there isn't any good C function in the library
        dnl to check for.)
        AC_CHECK_FILE($with_openthreads/lib/libOpenThreads.so,,THREAD_ERROR=yes)

        dnl If there was an error, abort; this library is required for OSG to compile.
        if test x$THREAD_ERROR = xyes ; then
            AC_MSG_ERROR([

    There is a problem with the OpenThreads library.  OpenThreads is required 
    for Open Scene Graph (and VESS) to work properly.

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

]
            )
        fi

        dnl Set the graphics library directories
        GFX_INCLUDE_DIR="$GFX_INCLUDE_DIR -I$x_includes -I$OSG_PATH/include -I$with_openthreads/include"
        GFX_LIB_DIR="$GFX_LIB_DIR -L$x_libraries -L$OSG_PATH/lib -L$OSG_PATH/lib/osgPlugins -L$with_openthreads/lib"
        GFX_LIB="$GFX_LIB -lXi -lXm -lXt -lGLU -lGL -lOpenThreads -losg -losgDB -losgUtil -losgText -losgSim -lfreetype -lXext -lXmu -lX11 -ldl"

        dnl Check the OSG libraries to see if they're where they are
        dnl supposed to be.  Set an error flag if not.
        LIBS=
        AC_CHECK_LIB(osg, osgGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib -L$with_openthreads/lib -lOpenThreads)
        AC_CHECK_LIB(osgDB, osgDBGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib -L$with_openthreads/lib -losg -lOpenThreads)
        AC_CHECK_LIB(osgUtil, osgUtilGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib -L$with_openthreads/lib -losg -lOpenThreads)
        AC_CHECK_LIB(osgText, osgTextGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib -L$with_openthreads/lib -losg -lOpenThreads)
        AC_CHECK_LIB(osgSim, osgSimGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib -L$with_openthreads/lib -losg -lOpenThreads)

        dnl Look for freetype 2.  We need this for osgText.
        AC_MSG_CHECKING([path for freetype2])
        if test -e "/usr/include/freetype2/freetype/ftglyph.h" ; then
            dnl Look in /usr (standard directory for Linux)
            FREETYPE_INCLUDE_DIR="-I/usr/include/freetype2"
            FREETYPE_LIB_DIR="-L/usr/$LIBDIR"
            AC_MSG_RESULT(/usr)

        elif test -e "/usr/local/include/freetype2/freetype/ftglyph.h" ; then
            dnl Look in /usr/local (alternate directory for Linux)
            FREETYPE_INCLUDE_DIR="-I/usr/local/include/freetype2 -I/usr/local/include"
            FREETYPE_LIB_DIR="-L/usr/local/$LIBDIR"
            AC_MSG_RESULT(/usr/local)

        elif test -e "/usr/freeware/include/freetype2/freetype/ftglyph.h" ; then
            dnl Look in /usr/freeware (standard directory for IRIX)
            FREETYPE_INCLUDE_DIR="-I/usr/freeware/include/freetype2 -I/usr/freeware/include"
            FREETYPE_LIB_DIR="-L/usr/freeware/$LIBDIR"
            AC_MSG_RESULT(/usr/freeware)

        else
            AC_MSG_RESULT([not found])
            AC_MSG_ERROR([

    Configure was unable to find the Freetype 2 library.  Freetype 2 is 
    required for Open Scene Graph (and VESS) to work properly.

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

]
            )
        fi

        dnl Make sure freetype is where we think it is
        FT_ERROR=
        LIBS=
        AC_CHECK_LIB(freetype, FT_Load_Char,, FT_ERROR=yes, $FREETYPE_LIB_DIR)

        dnl Print an error and bail if freetype is messed up
        if test x$FT_ERROR = xyes ; then
            AC_MSG_ERROR([

    There is a problem with Freetype 2.  Freetype 2 is required for Open 
    Scene Graph (and VESS) to work properly.

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

]
            )
        fi

        dnl Add the freetype paths to the graphics library directories
        GFX_INCLUDE_DIR="$GFX_INCLUDE_DIR $FREETYPE_INCLUDE_DIR"
        GFX_LIB_DIR="$GFX_LIB_DIR $FREETYPE_LIB_DIR"
        GFX_LIBS="$GFX_LIBS -lfreetype"

        dnl Print an error and bail if there is a problem with OSG
        if test x$GFX_ERROR = xyes ; then
            AC_MSG_ERROR([

    There is a problem with Open Scene Graph.  Either it is not installed, 
    or the libraries are installed in a location other than the one
    specified.  Configure was looking for OpenSceneGraph in the following
    location:

    $OSG_PATH

    You can specify a path to the Open Scene Graph libraries using the
    --with-osg=PREFIX option to configure.

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

]
            )
        fi

        dnl Check if the osgNV is to be used.
        if test x$with_osgnv != xno ; then

            dnl If they said yes as an argument, then default to /usr/local.
            if test x$with_osgnv = xyes ; then
                with_osgnv=/usr/local
            fi

            dnl Check if the library is found.
            AC_CHECK_LIB(osgNV, osgNVGetVersion,,OSGNV_ERROR=yes,-L$with_osgnv/lib/Linux32/ -L$with_osgnv/lib)
            if test x$OSGNV_ERROR = xyes ; then
                AC_MSG_ERROR([
                                                                                
    There is a problem with the osgNV library. Check to make sure that it is
    installed correctly and in the specified location.
                                                                                
    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
            fi

            dnl Modify the GFX variables to use osgNV.
            GFX_INCLUDE_DIR="$GFX_INCLUDE_DIR -I$with_osgnv/include"
            GFX_LIB_DIR="$GFX_LIB_DIR -L$with_osgnv/lib/Linux32/ -L$with_osgnv/lib"
            GFX_LIB="$GFX_LIB -losgNVCg -losgNV"

            dnl Add the extra objects that OSG requires.
            EXTRA_GRAPHICS_OBJECTS="$EXTRA_GRAPHICS_OBJECTS vsCgMatrixParameter.$OBJEXT vsCgParameter.$OBJEXT vsCgParameterBlockAttribute.$OBJEXT vsCgShaderAttribute.$OBJEXT vsCgStateMatrixParameter.$OBJEXT vsCgTextureParameter.$OBJEXT vsCgVectorParameter.$OBJEXT"
        fi

        dnl Add the extra objects that OSG requires
        EXTRA_GRAPHICS_OBJECTS="$EXTRA_GRAPHICS_OBJECTS vsBillboardCallback.$OBJEXT vsClipAttribute.$OBJEXT vsDecalCallback.$OBJEXT vsIntersectTraverser.$OBJEXT vsLineWidthAttribute.$OBJEXT vsLocalLightCallback.$OBJEXT vsSequenceCallback.$OBJEXT"
        ;;
    *)
        dnl Unknown graphics library
        AC_MSG_RESULT(unknown)
        AC_MSG_ERROR(Unknown graphics library) ;;
esac

dnl Export the EXTRA_GRAPHICS_OBJECTS variable to the makefile
AC_SUBST(EXTRA_GRAPHICS_OBJECTS)

dnl Sound options************************************************************

dnl Provide the option to enable or disable sound
AC_ARG_ENABLE(sound, 
[[  --enable-sound
  --disable-sound         Enable/Disable VESS audio features [enabled]]],,
enable_sound=yes)

dnl Provide the option to use OpenAL (and specify a path for it)
AC_ARG_WITH(openal,
[[  --with-openal=PREFIX    Specify the path prefix for OpenAL [/usr/local]]],
SOUND_LIBRARY=openal, with_openal=/usr/local; SOUND_LIBRARY=openal)

dnl Provide the option to use ALUT (and specify a path for it)
AC_ARG_WITH(alut,
[[  --with-alut=PREFIX      Specify the path prefix for ALUT [/usr/local]]],, 
with_alut=/usr/local)

dnl Check if sound is enabled
AC_MSG_CHECKING([if sound is enabled])
if test x$enable_sound = xyes; then

    dnl Print that sound is enabled
    AC_MSG_RESULT(yes)

    dnl Determine which sound library is being used, and set up
    dnl the necessary flags for it
    AC_MSG_CHECKING([which sound library to use])
    case "$SOUND_LIBRARY" in
        openal)
            dnl Print that we're using OpenAL
            AC_MSG_RESULT(OpenAL)

            dnl See if we really have OpenAL available
            if test -e "$with_openal/include/AL/al.h" ; then
                SOUND_INCLUDE_DIR=-I$with_openal/include
            else
                AC_MSG_ERROR([
  
   The OpenAL header files were not found.  Either provide a path
   to the OpenAL library via the --with-openal=PREFIX option to 
   configure, or disable the sound features of VESS using the
   --disable-sound option to configure.

]
                )
            fi

            dnl See if we really have ALUT available
            if test -e "$with_alut/include/AL/alut.h" ; then
                SOUND_INCLUDE_DIR="$SOUND_INCLUDE_DIR -I$with_alut/include"
            else
                AC_MSG_ERROR([
  
   The ALUT header files were not found.  Either provide a path
   to the ALUT library via the --with-alut=PREFIX option to 
   configure, or disable the sound features of VESS using the
   --disable-sound option to configure.

]
                )
            fi

            dnl Add the OpenAL and supporting libraries to the link line
            SOUND_LIB="-lalut -lopenal -lpthread -lm -ldl"

            dnl If we're on an IRIX box, we also need to link dmedia's 
            dnl libaudio library
            DMEDIA_ERROR=no
            case "$target" in
                mips-sgi-irix*)

                    dnl See if the dmedia audio library is available
                    LIBS=
                    AC_CHECK_LIB(audio, alWriteFrames,,DMEDIA_ERROR=yes,) 

                    dnl add -laudio to the link line
                    SOUND_LIB="$SOUND_LIB -laudio" ;;
                *) ;;
            esac

            dnl Print a warning message if dmedia isn't installed
            if test x$DMEDIA_ERROR = xyes ; then
                AC_MSG_WARN([

  The dmedia libraries are not installed.  VESS will compile, but any
  applications that use the VESS Sound Library will most likely not
  compile or run.

  You may want to install the dmedia libraries, or specify
  --disable-sound on the command line to disable the VESS audio
  features

]
               )
            fi

            dnl See if OpenAL is installed or just compiled in-place
            AC_MSG_CHECKING([path for OpenAL libraries])
            if test -e "$with_openal/lib/libopenal.a" ; then
                AC_MSG_RESULT([$with_openal/lib])
                SOUND_LIB_DIR=-L$with_openal/lib
            elif test -e "$with_openal/linux/src/libopenal.a" ; then
                AC_MSG_RESULT([$with_openal/linux/src])
                SOUND_LIB_DIR=-L$with_openal/linux/src
            else
                AC_MSG_RESULT([not found])
                AC_MSG_WARN([  

  Unable to find the OpenAL libraries (libopenal.a), VESS will compile, 
  but any applications that use the VESS Sound Library will most likely 
  not compile or run.

]           
                )
            fi

            dnl Add ALUT path to the library search path
            SOUND_LIB_DIR="$SOUND_LIB_DIR -L$with_alut/lib"

            dnl Check the OpenAL libraries
            SOUND_ERROR=no
            LIBS=
            AC_CHECK_LIB(openal, alSourcei,,SOUND_ERROR=yes, $SOUND_LIB_DIR $SOUND_LIB)
            dnl Print a warning if the check fails
            if test x$SOUND_ERROR = xyes ; then
                AC_MSG_WARN([

  There is a problem with the OpenAL libraries.  Either they are not
  installed correctly, or they have not been compiled.  VESS will compile, 
  but any applications that use the VESS Sound Library will most likely 
  not compile or run.

]           
                )
            fi

            dnl Check ALUT
            AC_CHECK_LIB(alut, alutLoadMemoryFromFile,,SOUND_ERROR=yes, $SOUND_LIB_DIR $SOUND_LIB)

            dnl Print a warning if the check fails
            if test x$SOUND_ERROR = xyes ; then
                AC_MSG_WARN([

  There is a problem with the ALUT library.  Either it is not
  installed correctly, or it has not been compiled.  VESS will compile, 
  but any applications that use the VESS Sound Library will most likely 
  not compile or run.

]           
                )
            fi

            dnl Set the audio library environment variable to indicate
            dnl OpenAL
            AL=OpenAL ;;
        *)
            dnl Print that we're using an unknown sound library
            AC_MSG_RESULT(unknown)
            AC_MSG_ERROR(Unknown sound library!!) ;;
    esac
else
    dnl Print that sound isn't enabled
    AC_MSG_RESULT(no)
fi

dnl I/O options************************************************************
IO_INCLUDE_DIR=""
IO_LIB_DIR=""
IO_LIB=""

dnl Provide the option to use libdc1394, the digital video camera interface
dnl library. Also requires libraw1394, the base 1394 communication library.
dnl Enables the compilation of the vs1394Camera class. The argument (if
dnl present) should be the path to the library; assumes /usr/local if no
dnl argument is given.

AC_ARG_WITH(raw1394,
[[  --with-raw1394=[prefix]   Use the raw1394 library for 1394 communication
                              (if necessary, the path to the library may be
                              specified) [no]]],
, with_raw1394=no)
AC_ARG_WITH(dc1394,
[[  --with-dc1394=[prefix]   Use the dc1394 library for digital video camera
                             communication (if necessary, the path to the
                             library may be specified) [no]]],
, with_dc1394=no)

dnl Default to /usr/local if no argument given
if test x$with_raw1394 = xyes ; then
    with_raw1394=/usr/local
fi
if test x$with_dc1394 = xyes ; then
    with_dc1394=/usr/local
fi

dnl Verify the 1394 path data if present
if test x$with_raw1394 != xno ; then
    LIBS=
    AC_CHECK_LIB(raw1394, raw1394_get_libversion,,RAW1394_ERROR=yes,-L$with_raw1394/lib)
    if test x$RAW1394_ERROR = xyes ; then
        AC_MSG_ERROR([

    There is a problem with the raw1394 library. Check to make sure that it
    is installed correctly and in the specified location. 

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
    fi

    if test x$with_dc1394 != xno ; then
        LIBS=
        AC_CHECK_LIB(dc1394_control, dc1394_print_camera_info,,DC1394_ERROR=yes, -L$with_raw1394/lib -lraw1394 -L$with_dc1394/lib)
        if test x$DC1394_ERROR = xyes ; then
            AC_MSG_ERROR([

    There is a problem with the dc1394 library. Check to make sure that it
    is installed correctly and in the specified location. 

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
        fi

        dnl Add the 1394 path information to the variables that will eventually
        dnl get exported to the IO makefile.
        EXTRA_IO_OBJECTS="$EXTRA_IO_OBJECTS vs1394Camera.$OBJEXT"
        IO_INCLUDE_DIR="$IO_INCLUDE_DIR -I$with_raw1394/include -I$with_dc1394/include"
        IO_LIB_DIR="$IO_LIB_DIR -L$with_raw1394/lib -L$with_dc1394/lib"
        IO_LIB="$IO_LIB -lraw1394 -ldc1394_control"
    fi
fi


dnl Provide the option to use the VRPN library. Enables the compilation of the
dnl vsVRPNTrackingSystem and vsHiball classes. The argument (if present) should
dnl be the path to the library; assumes /usr/local if no argument is given.

AC_ARG_WITH(vrpn,
[[  --with-vrpn=[prefix]   Use the VRPN library for remote peripheral 
                           communication (if necessary, the path to the library
                           may be specified) [no]]],
, with_vrpn=no)

dnl Default to /usr/local if no argument given
if test x$with_vrpn = xyes ; then
    with_vrpn=/usr/local
fi

dnl Verify the VRPN path data if present
if test x$with_vrpn != xno ; then


    AC_MSG_CHECKING([path for OpenAL libraries])
    if test -e "$with_vrpn/libs/libvrpn.a" ; then
        AC_MSG_RESULT([$with_vrpn/libs])
        VRPN_LIB_DIR=-L$with_vrpn/libs
    else
        AC_MSG_RESULT([not found])
        AC_MSG_WARN([  

            Unable to find the VRPN libraries (libvrpn.a). VESS may compile,
            but any applications that use the VRPN peripherals will most likely
            not compile or run.

        ])
    fi

    dnl Add the VRPN path information to the variables that will eventually
    dnl get exported to the IO makefile.
    EXTRA_IO_OBJECTS="$EXTRA_IO_OBJECTS vsVRPNTrackingSystem.$OBJEXT vsHiball.$OBJEXT"
    IO_INCLUDE_DIR="$IO_INCLUDE_DIR -I$with_vrpn/include"
    IO_LIB_DIR="$IO_LIB_DIR -L$with_vrpn/libs"
    IO_LIB="$IO_LIB -lvrpn"
fi


dnl Provide the option to use the PhaseSpace OWL API.  This is required for
dnl VESS support of the PhaseSpace Motion Digitization tracking system.
AC_ARG_WITH(phasespace,
[[  --with-phasespace=[prefix]   Use the PhaseSpace OWL library for support for
                                 the PhaseSpace tracking system. [no]]],
, with_phasespace=no)

dnl Verify the PhaseSpace path data if present
if test x$with_phasespace != xno ; then
    LIBS=
    AC_CHECK_LIB(owlsock, owlInit,,OWL_ERROR=yes,-L$with_phasespace/lib)
    if test x$OWL_ERROR = xyes ; then
        AC_MSG_ERROR([

    There is a problem with the PhaseSpace OWL library. Check to make sure 
    that it is installed correctly and in the specified location. 

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
    fi

    dnl Add the OWL path information to the variables that will eventually
    dnl get exported to the IO makefile.
    EXTRA_IO_OBJECTS="$EXTRA_IO_OBJECTS vsPhaseSpaceMD.$OBJEXT"
    IO_INCLUDE_DIR="$IO_INCLUDE_DIR -I$with_phasespace/include"
    IO_LIB_DIR="$IO_LIB_DIR -L$with_phasespace/lib"
    IO_LIB="$IO_LIB -lowlsock"
fi

dnl Provide the option to use the InterSense API.  This is required for
dnl general VESS support for all Intersense devices (there is a separate 
dnl object for serial-only support for the IS-600).
AC_ARG_WITH(isense,
[[  --with-isense=[prefix]       Use the InterSense library for support for
                                 InterSense tracking systems. [no]]],
, with_isense=no)

dnl Verify the InterSense path data if present
if test x$with_isense != xno ; then
    LIBS=
    AC_CHECK_LIB(isense, ISD_OpenTracker,,IS_ERROR=yes,-L$with_isense -L$with_isense/lib)
    if test x$IS_ERROR = xyes ; then
        AC_MSG_ERROR([

    There is a problem with the InterSense library. Check to make sure 
    that it is installed correctly and in the specified location. 

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
    fi

    dnl Add the OWL path information to the variables that will eventually
    dnl get exported to the IO makefile.
    EXTRA_IO_OBJECTS="$EXTRA_IO_OBJECTS vsIntersenseTrackingSystem.$OBJEXT"
    IO_INCLUDE_DIR="$IO_INCLUDE_DIR -I$with_isense/include"
    IO_LIB_DIR="$IO_LIB_DIR -L$with_isense/lib"
    IO_LIB="$IO_LIB -lisense"
fi

dnl Environment options******************************************************

dnl This variable is used for any extra classes that need compiling aside
dnl from the standard VESS classes.
EXTRA_ENVIRONMENT_OBJECTS=

dnl Initialize the graphics include and library paths
ENV_INCLUDE_DIR=
ENV_LIB_DIR=
ENV_LIB=

dnl Provide the option to use ffmpeg, the video reader library. Enables the
dnl compilation of the vsMovieReader class. The argument should be the path to
dnl the library; assumes /usr/local if no argument is given.
AC_ARG_WITH(ffmpeg,
[[  --with-ffmpeg=[prefix]   Use the ffmpeg library for video file reading (if
                           necessary, the path to the library may be
                           specified) [no]]],
, with_ffmpeg=no)
dnl Default to /usr/local if no argument given
if test x$with_ffmpeg = xyes ; then
    with_ffmpeg=/usr/local
fi
dnl Verify the ffmpeg path data if present
if test x$with_ffmpeg != xno ; then
    LIBS=
    AC_CHECK_LIB(avutil, av_rescale,,VIDEO_ERROR=yes,-L$with_ffmpeg/lib)
    AC_CHECK_LIB(avcodec, avcodec_open,,VIDEO_ERROR=yes,-L$with_ffmpeg/lib)
    AC_CHECK_LIB(avformat, av_register_all,,VIDEO_ERROR=yes,-L$with_ffmpeg/lib)
    if test x$VIDEO_ERROR = xyes ; then
        AC_MSG_ERROR([

    There is a problem with the ffmpeg library. Check to make sure that it
    is installed correctly and in the specified location. 

    For further support, consult the VESS User's Guide or join the 
    vess-users mailing list at:

        http://lists.sourceforge.net/lists/listinfo/vess-users

])
    fi
    dnl Add the ffmpeg path information to the variables that will eventually
    dnl get exported to the graphics makefile.
    EXTRA_ENVIRONMENT_OBJECTS="$EXTRA_ENVIRONMENT_OBJECTS vsMovieReader.$OBJEXT"
    EXTRA_ENVIRONMENT_OBJECTS="$EXTRA_ENVIRONMENT_OBJECTS vsMovieWriter.$OBJEXT"
    ENV_INCLUDE_DIR="$ENV_INCLUDE_DIR -I$with_ffmpeg/include"
    ENV_LIB_DIR="$ENV_LIB_DIR -L$with_ffmpeg/lib"
    ENV_LIB="$ENV_LIB -lavcodec -lavformat -lavutil"
fi

dnl Export the EXTRA_ENVIRONMENT_OBJECTS variable to the makefile
AC_SUBST(EXTRA_ENVIRONMENT_OBJECTS)

dnl XML options**************************************************************
XML_INCLUDE_DIR="-I/usr/include/libxml2"
XML_LIB_DIR=""
XML_LIB="-lxml2"

dnl Provide the option to specify the location of the XML library.
AC_ARG_WITH(libxml2,
[[  --with-libxml2=PREFIX   Specify the location of libxml2 [/usr]]],, with_libxml2=/usr)
if test "x$with_libxml2" != "x/usr"; then
   XML_INCLUDE_DIR="-I$with_libxml2/include/libxml2"
   XML_LIB_DIR="-L$with_libxml2/$LIBDIR"
   XML_LIB="-lxml2"
fi

dnl Check the libxml2 libraries
XML_ERROR=no
LIBS=
AC_CHECK_LIB(xml2, xmlParseMemory,,XML_ERROR=yes, $XML_LIB_DIR $XML_LIB)

dnl Print a warning if the check fails
if test x$XML_ERROR = xyes ; then
   AC_MSG_ERROR([

  There is a problem with the libxml2 libraries.  Either they are not
  installed correctly, or they have not been compiled.  VESS will not
  compile without the libxml2 libraries.

  For further support, consult the VESS User's Guide or join the 
  vess-users mailing list at:

      http://lists.sourceforge.net/lists/listinfo/vess-users

])
fi

dnl JPEG options**************************************************************

dnl Provide the option to specify the location of the JPEG library.
AC_ARG_WITH(jpeg,
[[  --with-jpeg=PREFIX      Specify the location of libjpeg [/usr]]],, with_jpeg=/usr)
if test "x$with_jpeg" != "x/usr"; then
   GFX_INCLUDE_DIR="$GFX_INCLUDE_DIR -I$with_jpeg/include"
   GFX_LIB_DIR="$GFX_LIB_DIR -L$with_jpeg/lib"
fi
GFX_LIB="$GFX_LIB -ljpeg"

dnl Check the libjpeg libraries
JPEG_ERROR=no
LIBS=
AC_CHECK_LIB(jpeg, jpeg_read_header,,JPEG_ERROR=yes)

dnl Print a warning if the check fails
if test x$JPEG_ERROR = xyes ; then
   AC_MSG_ERROR([

  There is a problem with the libjpeg libraries.  Either they are not
  installed correctly, or they have not been compiled.  VESS will not
  compile without the libjpeg libraries.

  For further support, consult the VESS User's Guide or join the 
  vess-users mailing list at:

      http://lists.sourceforge.net/lists/listinfo/vess-users

])
fi


dnl Export all of the important variables
AC_SUBST(ATLAS_INCLUDE_DIR)
AC_SUBST(ATLAS_LIB_DIR)
AC_SUBST(ATLAS_LIB)
AC_SUBST(GFX_INCLUDE_DIR)
AC_SUBST(GFX_LIB_DIR)
AC_SUBST(GFX_LIB)
AC_SUBST(SOUND_INCLUDE_DIR)
AC_SUBST(SOUND_LIB_DIR)
AC_SUBST(SOUND_LIB)
AC_SUBST(IO_INCLUDE_DIR)
AC_SUBST(IO_LIB_DIR)
AC_SUBST(IO_LIB)
AC_SUBST(ENV_INCLUDE_DIR)
AC_SUBST(ENV_LIB_DIR)
AC_SUBST(ENV_LIB)
AC_SUBST(XML_INCLUDE_DIR)
AC_SUBST(XML_LIB_DIR)
AC_SUBST(XML_LIB)
AC_SUBST(OS)
AC_SUBST(WS)
AC_SUBST(SG)
AC_SUBST(AL)

dnl Checks for other libraries that we'll be needing (including OpenGL)
LIBS=
AC_CHECK_LIB(c, printf)
AC_CHECK_LIB(dl, dlopen)
AC_CHECK_LIB(m, fabs)
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(GL, glDrawPixels,,GL_ERROR=yes, -L$x_libraries -L/usr/$LIBDIR)
AC_CHECK_LIB(GLU, gluDisk,,GL_ERROR=yes,-L$x_libraries -lGL -L/usr/$LIBDIR)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(gettimeofday socket strdup)

dnl Set up what gets cleaned on a "make clean" and "make distclean"
CLEANFILES="config.h.in~ so_locations"
DISTCLEANFILES="makefile config.h.in~ so_locations"
AC_SUBST(CLEANFILES)
AC_SUBST(DISTCLEANFILES)

dnl If we've messed with the CFLAGS, restore them now
if test x$OLDCFLAGS != x ; then
    CFLAGS="$OLDCFLAGS"
    CXXFLAGS="$OLDCXXFLAGS"
fi

dnl Export the AM_C(XX)FLAGS variables
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_CXXFLAGS)
AC_SUBST(AM_LDFLAGS)

dnl Configure the the makefiles to write out.  Start with the ones that
dnl will always be used (everything except sound and test).
AC_CONFIG_FILES([makefile avatar/makefile graphics/makefile io/makefile])
AC_CONFIG_FILES([llio/makefile motion/makefile system/makefile util/makefile])
AC_CONFIG_FILES([scent/makefile environment/makefile menu/makefile])

dnl Configure the subdirectories to build
DIRECTORIES="system util graphics io llio motion avatar scent environment menu"

dnl List the individual library names.  These are used as dependencies
dnl for the monolithic libvess library.
VESS_LIBS="avatar/libvsAvatar.a avatar/libvsAvatar.so"
VESS_LIBS="$VESS_LIBS graphics/libvsGraphics.a graphics/libvsGraphics.so"
VESS_LIBS="$VESS_LIBS io/libvsIO.a io/libvsIO.so"
VESS_LIBS="$VESS_LIBS llio/libvsLLIO.a llio/libvsLLIO.so"
VESS_LIBS="$VESS_LIBS motion/libvsMotion.a motion/libvsMotion.so"
VESS_LIBS="$VESS_LIBS system/libvsSystem.a system/libvsSystem.so"
VESS_LIBS="$VESS_LIBS util/libvsUtil.a util/libvsUtil.so"
VESS_LIBS="$VESS_LIBS scent/libvsScent.a scent/libvsScent.so"
VESS_LIBS="$VESS_LIBS environment/libvsEnvironment.a"
VESS_LIBS="$VESS_LIBS environment/libvsEnvironment.so"
VESS_LIBS="$VESS_LIBS menu/libvsMenu.a menu/libvsMenu.so"

dnl See if sound is enabled.  If so add it to the list of subdirectories,
dnl add it to the VESS_LIBS list, and add its makefile to the output.
if test x$enable_sound = xyes; then
    DIRECTORIES="$DIRECTORIES sound"
    VESS_LIBS="$VESS_LIBS sound/libvsSound.a sound/libvsSound.so"
    AC_CONFIG_FILES([sound/makefile])
fi

dnl See if there is a test directory that we need to generate a makefile
dnl for as well.  This can be compiled with the top-level "make test"
dnl target.
if test -d test ; then
    AC_CONFIG_FILES([test/makefile])
fi

dnl Export the directories and libraries list variables
AC_SUBST(DIRECTORIES)
AC_SUBST(VESS_LIBS)

dnl Write out the new makefiles
AC_OUTPUT
