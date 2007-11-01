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

    // vsDynamics library
    #ifdef VS_DYNAMICS_EXPORTS
        #define VS_DYNAMICS_DLL  __declspec(dllexport)
    #else
        #define VS_DYNAMICS_DLL  __declspec(dllimport)
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

    // vsDynamo library
    #ifdef VS_DYNAMO_EXPORTS
        #define VS_DYNAMO_DLL  __declspec(dllexport)
    #else
        #define VS_DYNAMO_DLL  __declspec(dllimport)
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

	// vsMenu library
    #ifdef VS_MENU_EXPORTS
        #define VS_MENU_DLL  __declspec(dllexport)
    #else
        #define VS_MENU_DLL  __declspec(dllimport)
    #endif

    // vsSystem library
    #ifdef VS_SYSTEM_EXPORTS
        #define VS_SYSTEM_DLL  __declspec(dllexport)
    #else
        #define VS_SYSTEM_DLL  __declspec(dllimport)
    #endif

#else

     // Define all tokens to be nothing
     #define VS_UTIL_DLL
     #define VS_LLIO_DLL
     #define VS_GRAPHICS_DLL
     #define VS_DYNAMICS_DLL
     #define VS_IO_DLL
     #define VS_SOUND_DLL
     #define VS_SCENT_DLL
     #define VS_MOTION_DLL
     #define VS_DYNAMO_DLL
     #define VS_AVATAR_DLL
     #define VS_ENVIRONMENT_DLL
     #define VS_SYSTEM_DLL
     #define VS_MENU_DLL
     
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
