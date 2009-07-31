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
//                  component in the scene graph
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_KINEMATICS_HPP
#define VS_KINEMATICS_HPP

#include "vsUpdatable.h++"
#include "vsComponent.h++"
#include "vsTransformAttribute.h++"
#include "atVector.h++"
#include "atQuat.h++"

class VESS_SYM vsKinematics : public vsUpdatable
{
private:

    // Associated component and transform attribute
    vsComponent             *component;
    vsTransformAttribute    *transform;

    // Current physical state
    atVector                position;
    atQuat                  orientation;

    atVector                velocity;
    atVector                angularVelocity;

    // Inertia on or off
    bool                    inertia;

    // Orientation constraints
    bool                    constrainOnUpdate;
    atVector                constraintAxis[3];
    double                  constraintMinAngle[3];
    double                  constraintMaxAngle[3];

    // Whether or not an update is required
    bool                    updateRequired;
    
    double                  constrainAngle(double value,
                                           double minDegrees,
                                           double maxDegrees);
    double                  calculateAxisRotation(atQuat rotation,
                                                  atVector axis);

public:

    // Constructor/destructor
                          vsKinematics(vsComponent *theComponent);
    virtual               ~vsKinematics();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Turns inertia on or off.  When inertia is on, the velocities
    // are preserved between frames.  When off, the velocities are
    // set to zero at the beginning of each frame.
    void                  enableInertia();
    void                  disableInertia();
    bool                  isInertiaEnabled();

    // Adjust the current position
    void                  setPosition(atVector newPosition);
    atVector              getPosition();
    void                  modifyPosition(atVector deltaPosition);

    // Adjust the current orientation
    void                  setOrientation(atQuat newOrientation);
    atQuat                getOrientation();
    void                  preModifyOrientation(atQuat deltaOrientation);
    void                  postModifyOrientation(atQuat deltaOrientation);

    // Adjust the current linear velocity
    void                  setVelocity(atVector newVelocity);
    atVector              getVelocity();
    void                  modifyVelocity(atVector deltaVelocity);

    // Adjust the current angular velocity
    void                  setAngularVelocity(atVector rotAxis, double degreesPerSec);
    atVector              getAngularVelocity();
    void                  modifyAngularVelocity(atVector rotAxis, 
                                                double degreesPerSec);

    // Adjust the center of mass for rotational purposes
    void                  setCenterOfMass(atVector newCenter);
    atVector              getCenterOfMass();

    // Adjust/apply the orientation constraints
    void                  setConstraint(int idx, atVector axis,
                                        double minAngle, double maxAngle);
    void                  getConstraint(int idx, atVector *axis,
                                        double *minAngle, double *maxAngle);

    void                  enableConstrainOnUpdate();
    void                  disableConstrainOnUpdate();
    bool                  isConstrainOnUpdateEnabled();
    void                  applyConstraints();

    // Return the associated vsComponent object
    vsComponent           *getComponent();

    // Compute new position/orientation based on current velocities and
    // elapsed time
    void                  update();

    // Compute new position/orientation based on current velocities and
    // specified time interval
    void                  update(double deltaTime);
};

#endif
