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

#include "atOSDefs.h++"
#include "atGlobals.h++"
#include <math.h>


// Functions under this access specifier are for VESS internal use
// only and should not be called by the end user.
#define VS_INTERNAL public


// Handle all OpenGL includes before other libraries get in the way
// Don't request function prototypes on Windows, we have to query them
// from the driver instead
#ifndef _WIN32
    #define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

// On Windows, glext.h is not automatically included, so we have to
// include it manually
#ifdef WIN32
    #include <GL/glext.h>
#endif


#endif
