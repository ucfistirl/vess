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
//    VESS Module:  vsIODevice.c++
//
//    Description:  Abstract base class for all VESS input devices
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsIODevice.h++"

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vsIODevice::vsIODevice(void)
{

}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsIODevice::~vsIODevice(void)
{

}

// ------------------------------------------------------------------------
// Each frame, the vsIOSystem responsible for this device should call this
// update function.
// ------------------------------------------------------------------------
void vsIODevice::update()
{
    int i;

    // Update all axes associated with this device
    for( i=0; i<getNumAxes(); i++ )
    {
        vsInputAxis * axis = getAxis(i);

        if( axis )
            axis->update();
    }

    // Update all buttons associated with this device
    for( i=0; i<getNumButtons(); i++ )
    {
        vsInputButton * button = getButton(i);

        if( button )
            button->update();
    }
}
