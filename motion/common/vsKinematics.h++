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
//    VESS Module:  vsKinematics.h++
//
//    Description:  Main object for associating a motion model with a
//		    component in the scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_KINEMATICS_HPP
#define VS_KINEMATICS_HPP

#include "vsUpdatable.h++"
#include "vsComponent.h++"
#include "vsTransformAttribute.h++"
#include "vsVector.h++"
#include "vsQuat.h++"

class vsKinematics : public vsUpdatable
{
private:

    // Associated component and transform attribute
    vsComponent             *component;
    vsTransformAttribute    *transform;

    // Current physical state
    vsVector		    position;
    vsQuat		    orientation;

    vsVector                velocity;
    vsVector	            angularVelocity;

    // Inertia on or off
    int                     inertia;
    
public:

    // Constructor/destructor
                          vsKinematics(vsComponent *theComponent);
		          ~vsKinematics();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Turns inertia on or off.  When inertia is on, the velocities
    // are preserved between frames.  When off, the velocities are
    // set to zero at the beginning of each frame.
    void                  enableInertia();
    void                  disableInertia();

    // Adjust the current position
    void	          setPosition(vsVector newPosition);
    vsVector              getPosition();
    void                  modifyPosition(vsVector deltaPosition);

    // Adjust the current orientation
    void                  setOrientation(vsQuat newOrientation);
    vsQuat                getOrientation();
    void                  preModifyOrientation(vsQuat deltaOrientation);
    void                  postModifyOrientation(vsQuat deltaOrientation);

    // Adjust the current linear velocity
    void                  setVelocity(vsVector newVelocity);
    vsVector              getVelocity();
    void                  modifyVelocity(vsVector deltaVelocity);

    // Adjust the current angular velocity
    void                  setAngularVelocity(vsVector rotAxis, double degreesPerSec);
    vsVector              getAngularVelocity();
    void                  modifyAngularVelocity(vsVector rotAxis, 
                                                double degreesPerSec);

    // Adjust the center of mass for rotational purposes
    void                  setCenterOfMass(vsVector newCenter);
    vsVector              getCenterOfMass();

    // Return the associated vsComponent object
    vsComponent           *getComponent();

    // Compute new position/orientation based on current velocities and
    // elapsed time
    void	          update();

    // Compute new position/orientation based on current velocities and
    // specified time interval
    void	          update(double deltaTime);
};

#endif
