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
//    VESS Module:  vsMenuSwitchButton.c++
//
//    Description:  This sub-class of the vsMenuButton moves through a
//                  series of graphically different states when activated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuSwitchButton.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes blank menu switch button.
// ------------------------------------------------------------------------
vsMenuSwitchButton::vsMenuSwitchButton()
{
    // By default, the component and kinematics of this object are null
    menuComponent = new vsComponent();
    menuKinematics = new vsKinematics(menuComponent);

    // Initialize the button press state
    pressedState = false;

    // Set up the switch attribute
    switchAttr = new vsSwitchAttribute();
    menuComponent->addAttribute(switchAttr);
    switchState = 0;

    // Reference everything so it can't be deleted without our consent
    switchAttr->ref();
    menuComponent->ref();
    menuKinematics->ref();
}

// ------------------------------------------------------------------------
// Destructor - The destructor does nothing
// ------------------------------------------------------------------------
vsMenuSwitchButton::~vsMenuSwitchButton()
{
    vsObject::unrefDelete(switchAttr);
    vsObject::unrefDelete(menuComponent);
    vsObject::unrefDelete(menuKinematics);

    // Set the variables to NULL so they aren't deleted again later
    menuComponent = NULL;
    menuKinematics = NULL;
}

// ------------------------------------------------------------------------
// Virtual function
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsMenuSwitchButton::getClassName()
{
    return "vsMenuSwitchButton";
}

// ------------------------------------------------------------------------
// Add and reference a new child of the switch node
// ------------------------------------------------------------------------
void vsMenuSwitchButton::addChild(vsComponent *child)
{
    menuComponent->addChild(child);
}

// ------------------------------------------------------------------------
// Set the active child on the switch, but only if the new state is valid
// ------------------------------------------------------------------------
void vsMenuSwitchButton::setSwitchState(int state)
{
    if ((state >= 0) && (state < menuComponent->getChildCount()))
    {
        // Disable the old element
        switchAttr->disableOne(switchState);

        // Update the new element and activate it
        switchState = state;
        switchAttr->enableOne(switchState);
    }
}

// ------------------------------------------------------------------------
// Return the current active item on the switch
// ------------------------------------------------------------------------
int vsMenuSwitchButton::getSwitchState()
{
    return switchState;
}

// ------------------------------------------------------------------------
// Virtual function
// Updates the menu object according to the signal it received from the
// indicated menu frame.
// ------------------------------------------------------------------------
void vsMenuSwitchButton::update(vsMenuSignal signal, vsMenuFrame *frame)
{
    // Decide what to do based on the signal received
    switch (signal)
    {
        case VS_MENU_SIGNAL_IDLE:
        {
            // Reset the press state to false to indicate no change
            pressedState = false;

            // Update the kinematics object if it exists
            if (menuKinematics)
                menuKinematics->update();
        }
        break;

        case VS_MENU_SIGNAL_ACTIVATE:
        {
            // If the object is activated, set the press state to true
            pressedState = true;

            // Move to the next element on the switch, cycling back around to
            // zero once the last state is reached
            setSwitchState((switchState + 1) % menuComponent->getChildCount());
        }
        break;

        case VS_MENU_SIGNAL_INCREASE:
        {
            // If the object is activated, set the press state to true
            pressedState = true;

            // Move to the next element on the switch, cycling back around to
            // zero once the last state is reached
            setSwitchState((switchState + 1) % menuComponent->getChildCount());
        }
        break;

        case VS_MENU_SIGNAL_DECREASE:
        {
            // If the object is activated, set the press state to true
            pressedState = true;

            // Move to the previous element on the switch, cycling back around
            // to the last element when the first is passed
            setSwitchState((switchState + menuComponent->getChildCount() - 1) %
                menuComponent->getChildCount());
        }
        break;
    }
}

