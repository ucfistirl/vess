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
//    VESS Module:  vsMenuToggleButton.c++
//
//    Description:  This sub-class of the vsMenuButton toggles its press
//                  state when activated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuToggleButton.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes blank menu toggle button.
// ------------------------------------------------------------------------
vsMenuToggleButton::vsMenuToggleButton()
{
    // By default, the component and kinematics of this object are null
    menuComponent = NULL;
    menuKinematics = NULL;

    // Initialize the button press state
    pressedState = false;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a toggle button from the
// component and kinematics of an existing vsMenuObject.
// ------------------------------------------------------------------------
vsMenuToggleButton::vsMenuToggleButton(vsMenuObject *object)
{
    // Store the component and kinematics for later use
    menuComponent = object->getComponent();
    menuKinematics = object->getKinematics();

    // Reference the component and kinematics objects so they won't be
    // accidentally deleted
    if (menuComponent)
        menuComponent->ref();
    if (menuKinematics)
        menuKinematics->ref();

    // Initialize the button press state
    pressedState = false;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a button with the given menu
// component and kinematics object. The kinematics object may be null if
// you do not want the component manipulated automatically on updates.
// ------------------------------------------------------------------------
vsMenuToggleButton::vsMenuToggleButton(vsComponent *component,
    vsKinematics *kinematics)
{
    // Store the component and kinematics for later use
    menuComponent = component;
    menuKinematics = kinematics;

    // Reference the component and kinematics objects so they won't be
    // accidentally deleted
    if (menuComponent)
        menuComponent->ref();
    if (menuKinematics)
        menuKinematics->ref();

    // Initialize the button press state
    pressedState = false;
}

// ------------------------------------------------------------------------
// Destructor - The destructor does nothing
// ------------------------------------------------------------------------
vsMenuToggleButton::~vsMenuToggleButton()
{
}

// ------------------------------------------------------------------------
// Virtual function
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsMenuToggleButton::getClassName()
{
    return "vsMenuToggleButton";
}

// ------------------------------------------------------------------------
// Virtual function
// Updates the menu object according to the signal it received from the
// indicated menu frame.
// ------------------------------------------------------------------------
void vsMenuToggleButton::update(vsMenuSignal signal, vsMenuFrame *frame)
{
    // Decide what to do based on the signal received
    switch (signal)
    {
        case VS_MENU_SIGNAL_IDLE:
        {
            // Update the kinematics object if it exists
            if (menuKinematics)
                menuKinematics->update();
        }
        break;

        case VS_MENU_SIGNAL_ACTIVATE:
        {
            // If the object is activated, invert the press state
            pressedState = !pressedState;
        }
        break;
    }
}

