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
//    VESS Module:  vsHandArticulation.c++
//
//    Description:  A class to allow the 22 degrees of freedom of the
//                  hand to be manipulated and updated as a unit.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsHandArticulation.h++"

// ------------------------------------------------------------------------
// Construct a vsHandArticulation object given an articulation glove and an
// array of kinematics for it to manipulate.  The nKinematics parameter
// specifies the number of vsKinematics objects in the handKinematics
// array.  The vsKinematics objects in the array align with the joints in
// the vsArticulationGlove class, so handKin[1] corresponds to
// vsArticulationGlove::joints[1], and can be indexed with the same
// VS_AG_JOINT_* symbol.  NULL values are permitted in the handKinematics
// array, if no rotation is desired at that joint.
// ------------------------------------------------------------------------
vsHandArticulation::vsHandArticulation(vsArticulationGlove *aGlove,
                                       int nKinematics,
                                       vsKinematics *handKinematics[])
{
    int i;
    
    // Save the glove parameter
    glove = aGlove;

    // Get the number of vsKinematics specified
    numKinematics = nKinematics;
 
    // Make sure the number of kinematics doesn't exceed the number of
    // joints in the vsArticulationGlove
    if (numKinematics > VS_AG_NUM_JOINTS)
        numKinematics = VS_AG_NUM_JOINTS;
 
    // Retrieve the kinematics objects
    for (i = 0; i < numKinematics; i++)
        handKin[i] = handKinematics[i];
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsHandArticulation::~vsHandArticulation()
{
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsHandArticulation::getClassName()
{
    return "vsHandArticulation";
}

// ------------------------------------------------------------------------
// Update all hand joint kinematics with the latest values from the
// articulation glove object
// ------------------------------------------------------------------------
void vsHandArticulation::update()
{
    int i;
    atQuat joint;

    // Update all hand joint kinematics based on the latest joint
    // orientations stored in the glove object
    for (i = 0; i < numKinematics; i++)
    {
        if (handKin[i] != NULL)
        {
            // Get the joint orientation from the glove
            joint = glove->getJoint(i);
            
            // Set the new orientation on the kinematics
            handKin[i]->setOrientation(joint);
        }
    }
}

