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
//    VESS Module:  vsAxisRotation.h++
//
//    Description:  Motion model to translate input from two vsInputAxis
//                  objects into heading and pitch rotations.  Maximum
//                  rotation extents and rotation speeds are configurable.
//                  A "reset button" is also available to re-center the
//                  orientation at any time.
//
//    Author(s):    Carlos Rosas-Anderson
//
//------------------------------------------------------------------------

#ifndef VS_AXIS_ROTATION_HPP
#define VS_AXIS_ROTATION_HPP

#include "vsMotionModel.h++"
#include "vsKinematics.h++"

#include "vsInputAxis.h++"
#include "vsInputButton.h++"

#define VS_AR_DEFAULT_HEADING_WIDTH 90.0
#define VS_AR_DEFAULT_PITCH_WIDTH   90.0

#define VS_AR_DEFAULT_HEADING_SPEED 250.0
#define VS_AR_DEFAULT_PITCH_SPEED   250.0

#define VS_AR_DOUBLE_TOLERANCE      0.1

class VESS_SYM vsAxisRotation : public vsMotionModel
{
private:

    // The control axes
    vsInputAxis      *headingAxis;
    vsInputAxis      *pitchAxis;

    // A button to reset the orientation to a default
    // value
    vsInputButton    *resetButton;

    // The kinematics to be rotated
    vsKinematics     *kinematics;

    // Maximum rotation extends
    double           headingHalfWidth;
    double           pitchHalfWidth;

    // Maximum rotation speed
    double           headingSpeed;
    double           pitchSpeed;

    // Default orientation
    atQuat           startingOrientation;

public:

    // Constructors/Destructor
                            vsAxisRotation(vsInputAxis *hAxis, 
                                           vsInputAxis *pAxis, 
                                           vsKinematics *kin);
                            vsAxisRotation(vsInputAxis *hAxis, 
                                           vsInputAxis *pAxis, 
                                           vsInputButton *rButton, 
                                           vsKinematics *kin);
    virtual                 ~vsAxisRotation();

    // Class name
    virtual const char      *getClassName();

    // vsMotionModel inherited methods
    virtual void            update();

    // Returns the orientation to that specified by startingOrientation
    virtual void            center();

    // Accessors
    virtual void            setHeadingWidth(double hWidth);
    virtual double          getHeadingWidth();

    virtual void            setPitchWidth(double pWidth);
    virtual double          getPitchWidth();

    virtual void            setHeadingSpeed(double hSpeed);
    virtual double          getHeadingSpeed();

    virtual void            setPitchSpeed(double pSpeed);
    virtual double          getPitchSpeed();

    virtual void            setStartingOrientation(atQuat orientation);
    virtual atQuat          getStartingOrientation();
};

#endif
