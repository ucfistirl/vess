dnl Process this file with autoconf to produce a configure script.

AC_INIT(vess, 2.0.0, vess@ist.ucf.edu)
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
   i*86-pc-linux*)
   AM_CFLAGS="$AM_CFLAGS -g -ansi -D__linux__ -D_MISC_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE -Wno-deprecated"
   AM_CXXFLAGS="$AM_CXXFLAGS -g -ansi -D__linux__ -D_MISC_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE -Wno-deprecated" ;;
   mips-sgi-irix*)
   AM_CFLAGS="$AM_CFLAGS -g -xansi -DIRIX -n32 -mips3 -DSGI"
   AM_CXXFLAGS="$AM_CXXFLAGS -g -xansi -DIRIX -n32 -mips3 -DSGI"
   AM_LDFLAGS="-n32 -mips3" ;;
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
dnl and requires -n32 and -mips3 (or -mips4) for compiler arguments, we'll 
dnl need to know this for the library tests.  Complicating this is the fact
dnl that the MIPSPro 7.3 compiler needs additional options to specify the
dnl language conventions (-LANG:std for STL support, required for Open 
dnl Scene Graph version), whereas 7.2.1 does not support these features at 
dnl all. We have to set the CFLAGS for this, so we'll save the current 
dnl CFLAGS in a temporary variable, and restore them when we're done with 
dnl our tests.
OLDCFLAGS=
OLDCXXFLAGS=

dnl Additionally, we add the vsLinuxJoystickSystem class dnl when under Linux.
dnl Shared libraries are built differently on different systems, so we set 
dnl up variables to handle that (since we're not using libtool).  Finally, 
dnl we set the operating system variable (OS) to indicate the appropriate 
dnl operating system.
case "$target" in
   i*86-pc-linux*)
       LIBDIR="lib"
       SO_LINK="-Wl,--whole-archive"
       SO_NO_LINK="-Wl,--no-whole-archive"
       EXTRA_IO_OBJECTS="vsLinuxJoystickSystem.$OBJEXT"
       OS=Linux ;;

   mips-sgi-irix*)
       CC=cc
       CXX=CC
       LIBDIR="lib32"
       SO_LINK="-all"
       SO_NO_LINK="-notall"
       EXTRA_IO_OBJECTS=
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
    
    E-mail vess@ist.ucf.edu if you think you've received this message in
    error.

])
       fi

       dnl Add the n32 options to the CFLAGS so the library tests will
       dnl work correctly
       CFLAGS="$CFLAGS -n32 -mips3"
       CXXFLAGS="$CXXFLAGS -n32 -mips3" 

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
AC_SUBST(EXTRA_IO_OBJECTS)
AC_SUBST(SO_LINK)
AC_SUBST(SO_NO_LINK)

dnl Window system**********************************************************

dnl Only X supported at the moment.  Set the window system variable to
dnl indicate X.
WS=X

dnl Where is X??  Find it and export the include and lib directories
AC_PATH_X
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
dnl from the standard VESS classes.  OSG makes use of this.
EXTRA_GRAPHICS_OBJECTS=

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

dnl Figure out which graphics library we're using
AC_MSG_CHECKING([which graphics library to use])
case "$SG" in
    Performer)
        dnl Print that we're using Performer for graphics
        AC_MSG_RESULT(Performer)

        dnl Performer can only be in one place.  Configure the include and
        dnl library directories accordingly.
        GFX_INCLUDE_DIR=-I/usr/include
        GFX_LIB_DIR="-L/usr/$LIBDIR -L/usr/$LIBDIR/libpfdb"
        GFX_LIB="-lpf -lpfflt -lpfutil -lpfdu -lpfui -lXi -lXm -lXt -lGLU -lGL -lXext -lXmu -lX11 -ldl"

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

        if test x$GFX_ERROR = xyes ; then
            AC_MSG_ERROR([

    There is a problem with OpenGL Performer.  Either Performer is not
    installed, or the libraries are installed in a non-standard location.

    E-mail vess@ist.ucf.edu if you think you've received this message in
    error.

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

        dnl Set the graphics library directories
        GFX_INCLUDE_DIR="-I$OSG_PATH/include -I$OSG_PATH/src/osgText"
        GFX_LIB_DIR="-L$OSG_PATH/lib -L$OSG_PATH/lib/osgPlugins"
        GFX_LIB="-lXi -lXm -lXt -lGLU -lGL -losg -losgDB -losgUtil -losgText -lfreetype -lXext -lXmu -lX11 -ldl"
        
        dnl Check the OSG libraries to see if they're where they are
        dnl supposed to be.  Set an error flag if not.
        LIBS=
        AC_CHECK_LIB(osg, osgGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib)
        AC_CHECK_LIB(osgDB, osgDBGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib)
        AC_CHECK_LIB(osgUtil, osgUtilGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib)
        AC_CHECK_LIB(osgText, osgTextGetVersion,,GFX_ERROR=yes,-L$OSG_PATH/lib)

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

    E-mail vess@ist.ucf.edu if you think you've received this message in
    error.

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

    E-mail vess@ist.ucf.edu if you think you've received this message in
    error.

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

    E-mail vess@ist.ucf.edu if you think you've received this message in
    error.

]
            )
        fi

        dnl Add the extra objects that OSG requires
        EXTRA_GRAPHICS_OBJECTS="vsBillboardCallback.$OBJEXT vsDecalCallback.$OBJEXT vsIntersectTraverser.$OBJEXT vsLocalLightCallback.$OBJEXT"
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

            dnl Add the OpenAL and supporting libraries to the link line
            SOUND_LIB="-lopenal -lpthread -lm -ldl"

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


dnl Export all of the important variables
AC_SUBST(GFX_INCLUDE_DIR)
AC_SUBST(GFX_LIB_DIR)
AC_SUBST(GFX_LIB)
AC_SUBST(SOUND_INCLUDE_DIR)
AC_SUBST(SOUND_LIB_DIR)
AC_SUBST(SOUND_LIB)
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
AC_CONFIG_FILES([motion/makefile system/makefile util/makefile])

dnl Configure the subdirectories to build
DIRECTORIES="system util graphics io motion avatar"

dnl List the individual library names.  These are used as dependencies
dnl for the monolithic libvess library.
VESS_LIBS="avatar/libvsAvatar.a avatar/libvsAvatar.so"
VESS_LIBS="$VESS_LIBS graphics/libvsGraphics.a graphics/libvsGraphics.so"
VESS_LIBS="$VESS_LIBS io/libvsIO.a io/libvsIO.so"
VESS_LIBS="$VESS_LIBS motion/libvsMotion.a motion/libvsMotion.so"
VESS_LIBS="$VESS_LIBS system/libvsSystem.a system/libvsSystem.so"
VESS_LIBS="$VESS_LIBS util/libvsUtil.a util/libvsUtil.so"

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
