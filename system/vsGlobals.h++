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


// Commonly used constants
enum
{
    VS_FALSE = 0,
    VS_TRUE = 1
};

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

#define VS_MATH_DEFAULT_TOLERANCE    (1E-6)


// Various useful macros

#define VS_SQR(x)  ( (x) * (x) )

// Convert from degrees to radians
#define VS_DEG2RAD(x)  ( (x) * VS_PI / 180.0 )

// Convert from radians to degrees
#define VS_RAD2DEG(x)  ( (x) * 180.0 / VS_PI )

// Determine if two floating-point values are close enough to be equal
#define VS_EQUAL(x,y)  ( fabs((x) - (y)) < 1E-6 )


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



#endif
