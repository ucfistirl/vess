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
//    VESS Module:  vsMenuButton.c++
//
//    Description:  The vsMenuButton is a clickable object. It has a state
//                  describing whether it is currently activated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuButton.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes blank menu button.
// ------------------------------------------------------------------------
vsMenuButton::vsMenuButton()
{
    // By default, the component and kinematics of this object are null
    menuComponent = NULL;
    menuKinematics = NULL;

    // Initialize the button press state
    pressedState = false;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu button from the
// component and kinematics of an existing vsMenuObject.
// ------------------------------------------------------------------------
vsMenuButton::vsMenuButton(vsMenuObject *object)
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
vsMenuButton::vsMenuButton(vsComponent *component, vsKinematics *kinematics)
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
vsMenuButton::~vsMenuButton()
{
    if (menuKinematics)
        vsObject::unrefDelete(menuKinematics);

    if (menuComponent)
        vsObject::unrefDelete(menuComponent);
}

// ------------------------------------------------------------------------
// Virtual function
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsMenuButton::getClassName()
{
    return "vsMenuButton";
}

// ------------------------------------------------------------------------
// Virtual function
// Updates the menu object according to the signal it received from the
// indicated menu frame.
// ------------------------------------------------------------------------
void vsMenuButton::update(vsMenuSignal signal, vsMenuFrame *frame)
{
    // Decide what to do based on the signal received
    switch (signal)
    {
        // If the button is idle, it should not be pressed
        case VS_MENU_SIGNAL_IDLE:
        {
            // Update the kinematics object if it exists
            if (menuKinematics)
                menuKinematics->update();

            // Reset the button press state to false
            pressedState = false;
        }
        break;

        case VS_MENU_SIGNAL_ACTIVATE:
        {
            // If the object is activated, mark it as pressed
            pressedState = true;
        }
        break;
    }
}

// ------------------------------------------------------------------------
// Sets the state of the button
// ------------------------------------------------------------------------
void vsMenuButton::setState(bool pressed)
{
    pressedState = pressed;
}

// ------------------------------------------------------------------------
// Returns whether the menu button was pressed on the last update
// ------------------------------------------------------------------------
bool vsMenuButton::isPressed()
{
    return pressedState;
}

