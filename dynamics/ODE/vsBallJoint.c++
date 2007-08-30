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
//    VESS Module:  vsBallJoint.c++
//
//    Description:  This vsDynamicJoint subclass represents a ball joint.
//                  It takes an anchor position and attempts to lock its
//                  attached units to the same relative radius around that
//                  position.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsBallJoint.h++"

// ------------------------------------------------------------------------
// Constructor - Default
// ------------------------------------------------------------------------
vsBallJoint::vsBallJoint(vsDynamicWorld *world, bool feedback) :
    vsDynamicJoint(feedback)
{
    // Create the ODE joint and store its ID.
    odeJointID = dJointCreateBall(world->getODEWorldID(), 0);

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
vsBallJoint::~vsBallJoint()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsBallJoint::getClassName()
{
    return "vsBallJoint";
}

// ------------------------------------------------------------------------
// This method specifies, in world coordinates, a point to which the two
// bodies of this joint should take as their anchor. The joint will attempt
// to satisfy its constraints at this position relative to those bodies at
// the time this method is called.
// ------------------------------------------------------------------------
void vsBallJoint::setAnchor(const atVector &anchor)
{
    dJointSetBallAnchor(odeJointID, anchor[0], anchor[1], anchor[2]);
}
