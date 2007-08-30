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
//    VESS Module:  vsFixedJoint.c++
//
//    Description:  This vsDynamicJoint subclass represents a fixed joint.
//                  It attempts to lock its attached units to the same
//                  relative position and orientation at the time of
//                  attachment.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsFixedJoint.h++"

// ------------------------------------------------------------------------
// Constructor - Default
// ------------------------------------------------------------------------
vsFixedJoint::vsFixedJoint(vsDynamicWorld *world, bool feedback) :
    vsDynamicJoint(feedback)
{
    // Create the ODE joint and store its ID.
    odeJointID = dJointCreateFixed(world->getODEWorldID(), 0);

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
vsFixedJoint::~vsFixedJoint()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsFixedJoint::getClassName()
{
    return "vsFixedJoint";
}

// ------------------------------------------------------------------------
// This method attaches two dynamic units together. If one of the pointers
// is NULL, this will fix the dynamic unit in the other pointer to the
// static environment. If both are NULL, the joint will simply have no
// effect upon the world. These conditions are consistent for most joint
// types.
// ------------------------------------------------------------------------
void vsFixedJoint::attach(vsDynamicUnit *unit1, vsDynamicUnit *unit2)
{
    dBodyID id1;
    dBodyID id2;

    // See if an ODE dynamic body ID may be extracted from this dynamic unit.
    if (unit1 != NULL)
    {
        id1 = unit1->getODEBodyID();
    }
    else
    {
        id1 = 0;
    }

    // See if an ODE dynamic body ID may be extracted from this dynamic unit.
    if (unit2 != NULL)
    {
        id2 = unit2->getODEBodyID();
    }
    else
    {
        id2 = 0;
    }

    // Attach the joint with the two IDs.
    dJointAttach(odeJointID, id1, id2);
    dJointSetFixed(odeJointID);
}

