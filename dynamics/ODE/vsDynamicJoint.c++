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
//    VESS Module:  vsDynamicJoint.c++
//
//    Description:  This abstract class represents a joint in a dynamic
//                  environment.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsDynamicJoint.h++"

// ------------------------------------------------------------------------
// Constructor
// This class is abstract, so the constructor does nothing.
// ------------------------------------------------------------------------
vsDynamicJoint::vsDynamicJoint(bool feedback)
{
    // See if feedback forces are to be monitored for this joint.
    if (feedback)
    {
        // Create the structure. The subclass constructor is responsible for
        // associating it with the joint itself.
        odeJointFeedback = (dJointFeedback *)malloc(sizeof(dJointFeedback));
    }
    else
    {
       // Initialize the structure to NULL so attempts are not made to use it.
       odeJointFeedback = NULL;
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDynamicJoint::~vsDynamicJoint()
{
    // Destroy the joint, as long as its ID is legitimate.
    if (odeJointID != 0)
    {
        dJointDestroy(odeJointID);
    }

    // Free the feedback structure if it has been created.
    if (odeJointFeedback)
    {
        free(odeJointFeedback);
    }
}

// ------------------------------------------------------------------------
// This method attaches two dynamic units together. If one of the pointers
// is NULL, this will fix the dynamic unit in the other pointer to the
// static environment. If both are NULL, the joint will simply have no
// effect upon the world. These conditions are consistent for most joint
// types.
// ------------------------------------------------------------------------
void vsDynamicJoint::attach(vsDynamicUnit *unit1, vsDynamicUnit *unit2)
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
}

// ------------------------------------------------------------------------
// This method sets the input parameters to the appropriate quantities for
// this joint as of the most recent world step, setting the force and
// torque the joint applied to each of its two bodies. Any of the
// parameters may be NULL if the data is not needed.
// ------------------------------------------------------------------------
void vsDynamicJoint::getFeedback(atVector *force1, atVector *torque1,
    atVector *force2, atVector *torque2)
{
    // First, make sure the feedback structure has been allocated.
    if (odeJointFeedback)
    {
        if (force1)
            force1->set(3, odeJointFeedback->f1);

        if (torque1)
            torque1->set(3, odeJointFeedback->t1);

        if (force2)
            force2->set(3, odeJointFeedback->f2);

        if (torque2)
            torque2->set(3, odeJointFeedback->t2);
    }
}

// ------------------------------------------------------------------------
// VESS Internal Function
// This method returns the ODE joint ID of this dynamic joint.
// ------------------------------------------------------------------------
dJointID vsDynamicJoint::getODEJointID()
{
    return odeJointID;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// This method returns the ODE body ID of the index-th attached body to
// this joint. As joints may only be connected to two bodies, only indices
// of 0 or 1 are valid. A body ID of 0 is returned in the case of an
// invalid index, or if the joint is attached to the static environment.
// ------------------------------------------------------------------------
dBodyID vsDynamicJoint::getAttachedODEBodyID(int index)
{
    // Check the index for validity before making ODE calls.
    if ((index == 0) || (index == 1))
        return dJointGetBody(odeJointID, index);

    // Return 0 in the case of an invalid index.
    return 0;
}
