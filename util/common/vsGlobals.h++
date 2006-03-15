//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsGlobals.h++
//
//    Description:  Global header file for defining commonly-used
//                  constants and macros
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_GLOBALS_HPP
#define VS_GLOBALS_HPP

#include <math.h>

// Functions under this access specifier are for VESS internal use
// only and should not be called by the end user.
#define VS_INTERNAL public


enum
{
    VS_X = 0,
    VS_Y = 1,
    VS_Z = 2,
    VS_W = 3,
    VS_H = 0,
    VS_P = 1,
    VS_R = 2
};

#define VS_PI    (3.14159265358979)

#define VS_DEFAULT_TOLERANCE    (1E-12)

// Various useful macros

#define VS_SQR(x)  ( (x) * (x) )

// Convert from degrees to radians
#define VS_DEG2RAD(x)  ( (x) * VS_PI / 180.0 )

// Convert from radians to degrees
#define VS_RAD2DEG(x)  ( (x) * 180.0 / VS_PI )

// Determine if two floating-point values are close enough to be equal
#define VS_EQUAL(x,y)  ( fabs((x) - (y)) < VS_DEFAULT_TOLERANCE )


// Constants for use in conversion to/from Euler rotations
// The three axes of rotation are specified in left to right order
// i.e. XYZ means rotate around the X-axis, then the Y-axis, finally the Z-axis
// The last letter ('S' or 'R') indicates static or relative rotation axes.
// With static axes, the coordinate axes stay fixed during rotations; each
// rotation around a particular axis rotates points the same way, regardless
// of what other rotations have been done. Relative coordinate axes move with
// each rotation; two X-axis rotations will move in different directions
// if there is an intervening Y or Z-axis rotation. The two types are opposites
// of each other: XYZ static produces the same effect as ZYX relative.
enum vsMathEulerAxisOrder
{
    VS_EULER_ANGLES_XYZ_S,
    VS_EULER_ANGLES_XZY_S,
    VS_EULER_ANGLES_YXZ_S,
    VS_EULER_ANGLES_YZX_S,
    VS_EULER_ANGLES_ZXY_S,
    VS_EULER_ANGLES_ZYX_S,
    
    VS_EULER_ANGLES_XYX_S,
    VS_EULER_ANGLES_XZX_S,
    VS_EULER_ANGLES_YXY_S,
    VS_EULER_ANGLES_YZY_S,
    VS_EULER_ANGLES_ZXZ_S,
    VS_EULER_ANGLES_ZYZ_S,

    VS_EULER_ANGLES_XYZ_R,
    VS_EULER_ANGLES_XZY_R,
    VS_EULER_ANGLES_YXZ_R,
    VS_EULER_ANGLES_YZX_R,
    VS_EULER_ANGLES_ZXY_R,
    VS_EULER_ANGLES_ZYX_R,
    
    VS_EULER_ANGLES_XYX_R,
    VS_EULER_ANGLES_XZX_R,
    VS_EULER_ANGLES_YXY_R,
    VS_EULER_ANGLES_YZY_R,
    VS_EULER_ANGLES_ZXZ_R,
    VS_EULER_ANGLES_ZYZ_R
};

// Performer coordinate axes are specified as heading-pitch-roll but applied
// as roll-pitch-heading. Additionally, the Performer coordinate axes are
// specified as 'forward' being positive Y. 
#define VS_EULER_ANGLES_PERFORMER    VS_EULER_ANGLES_ZXY_R

// Windows-specific DLL linkage directives.  Windows requires explicit export
// and import instructions for its shared libraries (DLL's).  The following
// sections define a token VS_*_DLL for each VESS library that specifies
// the linkage.  The token is defined conditionally. If the library itself 
// is being compiled the token expands into an export directive.  If a module 
// that is using the library is being compiled, the token expands into an
// import directive.  On platforms other than Windows, the tokens expand to
// nothing.
//
#ifdef WIN32

    // vsUtil library
    #ifdef VS_UTIL_EXPORTS
        #define VS_UTIL_DLL  __declspec(dllexport)
    #else
        #define VS_UTIL_DLL  __declspec(dllimport)
    #endif

    // vsLLIO library
    #ifdef VS_LLIO_EXPORTS
        #define VS_LLIO_DLL  __declspec(dllexport)
    #else
        #define VS_LLIO_DLL  __declspec(dllimport)
    #endif

    // vsGraphics library
    #ifdef VS_GRAPHICS_EXPORTS
        #define VS_GRAPHICS_DLL  __declspec(dllexport)
    #else
        #define VS_GRAPHICS_DLL  __declspec(dllimport)
    #endif

    // vsIO library
    #ifdef VS_IO_EXPORTS
        #define VS_IO_DLL  __declspec(dllexport)
    #else
        #define VS_IO_DLL  __declspec(dllimport)
    #endif

    // vsSound library
    #ifdef VS_SOUND_EXPORTS
        #define VS_SOUND_DLL  __declspec(dllexport)
    #else
        #define VS_SOUND_DLL  __declspec(dllimport)
    #endif

    // vsScent library
    #ifdef VS_SCENT_EXPORTS
        #define VS_SCENT_DLL  __declspec(dllexport)
    #else
        #define VS_SCENT_DLL  __declspec(dllimport)
    #endif

    // vsMotion library
    #ifdef VS_MOTION_EXPORTS
        #define VS_MOTION_DLL  __declspec(dllexport)
    #else
        #define VS_MOTION_DLL  __declspec(dllimport)
    #endif

    // vsAvatar library
    #ifdef VS_AVATAR_EXPORTS
        #define VS_AVATAR_DLL  __declspec(dllexport)
    #else
        #define VS_AVATAR_DLL  __declspec(dllimport)
    #endif

    // vsEnvironment library
    #ifdef VS_ENVIRONMENT_EXPORTS
        #define VS_ENVIRONMENT_DLL  __declspec(dllexport)
    #else
        #define VS_ENVIRONMENT_DLL  __declspec(dllimport)
    #endif

    // vsSystem library
    #ifdef VS_SYSTEM_EXPORTS
        #define VS_SYSTEM_DLL  __declspec(dllexport)
    #else
        #define VS_SYSTEM_DLL  __declspec(dllimport)
    #endif

	// vsMenu library
    #ifdef VS_MENU_EXPORTS
        #define VS_MENU_DLL  __declspec(dllexport)
    #else
        #define VS_MENU_DLL  __declspec(dllimport)
    #endif

#else

     // Define all tokens to be nothing
     #define VS_UTIL_DLL
     #define VS_LLIO_DLL
     #define VS_GRAPHICS_DLL
     #define VS_IO_DLL
     #define VS_SOUND_DLL
     #define VS_SCENT_DLL
     #define VS_MOTION_DLL
     #define VS_AVATAR_DLL
     #define VS_ENVIRONMENT_DLL
     #define VS_SYSTEM_DLL
     #define VS_MENU_DLL
     
#endif

// Under Windows, define the sleep() and usleep() functions as macros of
// the Windows Sleep() function
#ifdef _MSC_VER
    #include <windows.h>
    #include <wingdi.h>

    // Sleep() takes milliseconds so multiply x by 1000 for sleep()
    #define sleep(x)  Sleep((x) * 1000)

    // Sleep() takes milliseconds, so divide x by 1000 for usleep()
    // if the result of (x/1000) is zero, the thread will still sleep
    // (give up the processor).
    #define usleep(x) Sleep((x) / 1000)
#endif

// Handle all OpenGL includes before other libraries get in the way
// Don't request function prototypes on Windows, we have to query them
// from the driver instead
#ifndef WIN32
    #define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

// On Windows, glext.h is not automatically included, so we have to
// include it manually
#ifdef WIN32
    // Before we include glext.h for the various function prototypes and
    // symbols we use, we need to #define a few extensions as already
    // present.  Performer's opengl.h defines these extensions and macros,
    // but doesn't define the extension symbol itself as present.
    #define GL_EXT_polygon_offset 1
    #define GL_SGIS_point_line_texgen 1
    #define GL_SGIS_texture_lod 1
    #define GL_EXT_packed_pixels 1
    #define GL_SGIS_detail_texture 1
    #include <GL/glext.h>
#endif

#endif
