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
//    VESS Module:  vsPhantom.c++
//
//    Description:  A class for storing and returning the state of a
//                  PHANToM
//
//    Author(s):    Duvan Cope, Jason Daly
//
//------------------------------------------------------------------------

#include "vsPhantom.h++"

// ------------------------------------------------------------------------
// Initialize the PHANToM.
// ------------------------------------------------------------------------
vsPhantom::vsPhantom(void)
               : vs6DInputDevice()
{
    int i;
    
    for (i = 0; i < VS_PHANTOM_BUTTONS; i++)
    {
        // Create the button for the phantom's stylus.
        button[i] = new vsInputButton();
    }
}

// ------------------------------------------------------------------------
// Release the PHANToM device.
// ------------------------------------------------------------------------
vsPhantom::~vsPhantom(void)
{
    int i;
    
    for (i = 0; i < VS_PHANTOM_BUTTONS; i++)
    {
        if (button[i])
        {
            delete button[i];
        }
    }
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name.
// ------------------------------------------------------------------------
const char *vsPhantom::getClassName()
{
    return "vsPhantom";
}

// ------------------------------------------------------------------------
// Set the position of the PHANToM
// ------------------------------------------------------------------------
void vsPhantom::setPosition(atVector posVec)
{
    int i;

    // Set the position axes to the values specified in the position vector.
    for (i = 0; i < 3; i++)
    {
        position[i].setPosition(posVec.getValue(i));
    }
}

// ------------------------------------------------------------------------
// Set the velocity of the PHANToM's motion
// ------------------------------------------------------------------------
void vsPhantom::setVelocity(atVector velVec)
{
    velocity = velVec;
}

// ------------------------------------------------------------------------
// Set the orientation of the PHANToM stylus.
// ------------------------------------------------------------------------
void vsPhantom::setOrientation(atVector ornVec,
                                 atMathEulerAxisOrder axisOrder)
{
    // Set the stylus' orientation to the Euler angles specified in the
    // orientation vector, using the given axis order.
    orientation.setEulerRotation(axisOrder, ornVec.getValue(0),
                                 ornVec.getValue(1), ornVec.getValue(2));
}

// ------------------------------------------------------------------------
// Set the orientation of the PHANToM stylus.
// ------------------------------------------------------------------------
void vsPhantom::setOrientation(atMatrix ornMat)
{
    // Set the stylus' orientation to the given rotation matrix.
    orientation.setMatrixRotation(ornMat);
}

// ------------------------------------------------------------------------
// Set the orientation of the PHANToM stylus.
// ------------------------------------------------------------------------
void vsPhantom::setOrientation(atQuat ornQuat)
{
    // Set the stylus' orientation to the given quaternion.
    orientation = ornQuat;
}

// ------------------------------------------------------------------------
// Return the number of buttons on the PHANToM.
// ------------------------------------------------------------------------
int vsPhantom::getNumButtons()
{
    return VS_PHANTOM_BUTTONS;
}

// ------------------------------------------------------------------------
// Return the button at the given index.
// ------------------------------------------------------------------------
vsInputButton *vsPhantom::getButton(int index)
{
    // Make sure the index is valid.
    if ((index >= 0) && (index < VS_PHANTOM_BUTTONS))
    {
        // Return the specified button.
        return button[index];
    }
    else
    {
        // Invalid button specified.
        return NULL;
    }
}

atVector vsPhantom::getVelocityVec(void)
{
    return(velocity);
}

