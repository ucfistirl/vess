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
//    VESS Module:  vsVest.c++
//
//    Description:  Interface to IST's vibrating vest
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#include "vsVest.h++"

#include <stdlib.h>

// ------------------------------------------------------------------------
// Initialize the vest - created by vsVestSystem
// ------------------------------------------------------------------------
vsVest::vsVest(int nButtons)
    : numButtons( 0 ), buttons( NULL )
{
    int memAllocationSize;

    if (nButtons > 0)
    {
        // allocate space for the buttons
        memAllocationSize = sizeof( vsInputButton * ) * nButtons;
        buttons = (vsInputButton **)malloc( memAllocationSize );

        // allocate each button
        for (int i = 0; i < nButtons; i++)
            buttons[i] = new vsInputButton();

        numButtons = nButtons;
    }
}

// ------------------------------------------------------------------------
// Cleanup the vest
// ------------------------------------------------------------------------
vsVest::~vsVest()
{
    if ( buttons != NULL )
    {
        for (int i = 0; i < numButtons; i++)
        {
            if (buttons[i])
                delete buttons[i];
        }

        free( buttons );
    }
}

// ------------------------------------------------------------------------
// What class is this?
// ------------------------------------------------------------------------
const char * vsVest::getClassName()
{
    return "vsVest";
}

// ------------------------------------------------------------------------
// There are no axes on the vest, just buttons
// ------------------------------------------------------------------------
int vsVest::getNumAxes()
{
    return 0;
}

// ------------------------------------------------------------------------
// Return how many buttons there are
// ------------------------------------------------------------------------
int vsVest::getNumButtons()
{
    return numButtons;
}

// ------------------------------------------------------------------------
// There are no axes so just return NULL
// ------------------------------------------------------------------------
vsInputAxis * vsVest::getAxis(int index)
{
    return NULL;
}

// ------------------------------------------------------------------------
// Return the given vsInputButton, if index is a valid button
// ------------------------------------------------------------------------
vsInputButton * vsVest::getButton(int index)
{
    if (buttons != NULL && index >= 0 && index < numButtons)
        return buttons[index];
    else
        return NULL;
}
