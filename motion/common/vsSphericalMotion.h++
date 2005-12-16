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
//    VESS Module:  vsSphericalMotion.h++
//
//    Description:  Motion model that provides user-controlled spherical 
//                  motion with respect to a point or another component.  
//                  That is, the controlled component will orbit the 
//                  target point/component on a circumscribed sphere.  
//                  The radius of the sphere (how close the controlled 
//                  component orbits w.r.t. the target point/component) is
//                  user-controlled as well.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SPHERICAL_MOTION_HPP
#define VS_SPHERICAL_MOTION_HPP

// Useful for tracking and examining objects or entities moving within a 
// larger virtual scene (viewpoint control).  Also good for creating 
// secondary motions of smaller objects around a larger object.
//
// This motion model is not designed to interoperate with other motion
// models on the same object/kinematics.

#include "vsMotionModel.h++"
#include "vsMouse.h++"
#include "vsKinematics.h++"

// Default constraints for the sphere
#define VS_SPHM_DEFAULT_MIN_RADIUS 0.0
#define VS_SPHM_DEFAULT_MAX_RADIUS 1000000.0

// Default number of database units moved (or degrees rotated) per 
// normalized unit of input movement
#define VS_SPHM_DEFAULT_ORBIT_CONST 180.0
#define VS_SPHM_DEFAULT_ZOOM_CONST  10.0

// Minimum orbit radius
#define VS_SPHM_MINIMUM_RADIUS 0.1

// Constants for noting whether we're targeting a point or component
enum  vsSphericalMotionTargetMode
{
    VS_SPHM_TARGET_POINT,
    VS_SPHM_TARGET_COMPONENT
};

class VS_MOTION_DLL vsSphericalMotion : public vsMotionModel
{
protected:

    // Input axes
    vsInputAxis                    *horizontal;
    vsInputAxis                    *vertical;

    // Input buttons
    vsInputButton                  *orbitButton;
    vsInputButton                  *zoomButton;

    // Kinematics
    vsKinematics                   *kinematics;

    // Targets
    vsVector                       targetPoint;
    vsComponent                    *targetComp;

    // Targeting mode
    vsSphericalMotionTargetMode    targetMode;

    // Previous input values used to calculate velocities
    double                         lastHorizontal, lastVertical;

    // Translation/rotation constants
    double                         orbitConst;
    double                         zoomConst;

    // Radius constraints
    double                         minRadius;

public:

    // Constructors (see the source file or documentation for a description
    // of each form).
                          vsSphericalMotion(vsMouse *mouse, vsKinematics *kin);

                          vsSphericalMotion(vsMouse *mouse, 
                                            int orbitButtonIndex,
                                            int zoomButtonIndex, 
                                            vsKinematics *kin);

                          vsSphericalMotion(vsInputAxis *horizAxis, 
                                            vsInputAxis *vertAxis,
                                            vsInputButton *orbitBtn, 
                                            vsInputButton *zoomBtn, 
                                            vsKinematics *kin);

    // Destructor
    virtual               ~vsSphericalMotion();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Targeting functions
    void                  setTargetPoint(vsVector targetPt);
    vsVector              getTargetPoint();

    void                  setTargetComponent(vsComponent *targetCmp);
    vsComponent           *getTargetComponent();

    // Returns the current target mode
    vsSphericalMotionTargetMode    getTargetMode();

    // Constraints on the sphere's radius
    void                  setMinimumRadius(double minDist);
    double                getMinimumRadius();

    // Movement constant accessors
    void                  setZoomConstant(double newConst);
    double                getZoomConstant();

    void                  setOrbitConstant(double newConst);
    double                getOrbitConstant();
 
    // Update function
    virtual void          update();
};

#endif
