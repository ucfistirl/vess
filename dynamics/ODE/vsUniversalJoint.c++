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
//    VESS Module:  vsUniversalJoint.c++
//
//    Description:  This vsDynamicJoint subclass represents a universal
//                  joint. It attempts to lock its attached units to the
//                  same relative position as at the time of attachment,
//                  and allows rotation around two orthogonal axes.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsUniversalJoint.h++"

// ------------------------------------------------------------------------
// Constructor - Default
// ------------------------------------------------------------------------
vsUniversalJoint::vsUniversalJoint(vsDynamicWorld *world, bool feedback) :
    vsDynamicJoint(feedback)
{
    // Create the ODE joint and store its ID.
    odeJointID = dJointCreateUniversal(world->getODEWorldID(), 0);

    // See if feedback forces are to be monitored for this joint.
    if (feedback)
    {
        // Associate the feedback structure with the newly-created joint.
        dJointSetFeedback(odeJointID, odeJointFeedback);
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsUniversalJoint::~vsUniversalJoint()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsUniversalJoint::getClassName()
{
    return "vsUniversalJoint";
}

// ------------------------------------------------------------------------
// This method specifies, in world coordinates, a point to which the two
// bodies of this joint should take as their anchor. The hinge joint will
// attempt to satisfy its constraints at this position relative to those
// bodies at the time this method is called.
// ------------------------------------------------------------------------
void vsUniversalJoint::setAnchor(const atVector &anchor)
{
    dJointSetUniversalAnchor(odeJointID, anchor[0], anchor[1], anchor[2]);
}

// ------------------------------------------------------------------------
// This method specifies in world coordinates an axis that two bodies
// should rotate around. The hinge joint will attempt to satisfy its
// constraints with respect to this axis relative to those bodies at the
// time this method is called.
// ------------------------------------------------------------------------
void vsUniversalJoint::setAxis1(const atVector &axis)
{
    dJointSetUniversalAxis1(odeJointID, axis[0], axis[1], axis[2]);
}

// ------------------------------------------------------------------------
// This method specifies in world coordinates an axis that two bodies
// should rotate around. The hinge joint will attempt to satisfy its
// constraints with respect to this axis relative to those bodies at the
// time this method is called.
// ------------------------------------------------------------------------
void vsUniversalJoint::setAxis2(const atVector &axis)
{
    dJointSetUniversalAxis2(odeJointID, axis[0], axis[1], axis[2]);
}

// ------------------------------------------------------------------------
// This method specifies, in radians, the minimum angle that this joint is
// allowed to reach. Joint angles are measured from -pi to +pi, so a value
// of less than -pi will allow the joint to move without a low constraint.
// ------------------------------------------------------------------------
void vsUniversalJoint::setMinimumAngle(double angle)
{
    dJointSetUniversalParam(odeJointID, dParamLoStop, angle);
}

// ------------------------------------------------------------------------
// This method specifies, in radians, the maximum angle that this joint is
// allowed to reach. Joint angles are measured from -pi to +pi, so a value
// of more than +pi will allow the joint to move without a high constraint.
// ------------------------------------------------------------------------
void vsUniversalJoint::setMaximumAngle(double angle)
{
    dJointSetUniversalParam(odeJointID, dParamHiStop, angle);
}

// ------------------------------------------------------------------------
// This method specifies the behavior of the hinge joint when it reaches
// one of its limits as a bounciness value measured from 0.0 to 1.0. A
// value of 0.0 means the joint will come to a halt when it reaches the
// limit, while a value of 1.0 will cause it to rebound at its incident
// velocity.
// ------------------------------------------------------------------------
void vsUniversalJoint::setLimitBounce(double bounce)
{
    dJointSetUniversalParam(odeJointID, dParamBounce, bounce);
}
