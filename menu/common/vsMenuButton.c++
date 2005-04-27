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

    // Set whether this frame can be repeat-activated by default.
    canRepeat = true;
    previousState = false;

    // Set whether idle causes state reversion to true by default.
    idleReverts = true;
    idleState = false;

    // Initialize the button press state.
    pressedState = false;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu button from the
// component and kinematics of an existing vsMenuObject.
// ------------------------------------------------------------------------
vsMenuButton::vsMenuButton(vsMenuObject *object)
{
    // Store the component and kinematics for later use
    menuComponent = (vsComponent *)object->getComponent()->cloneTree();
    menuKinematics = new vsKinematics(menuComponent);

    // Reference the component and kinematics objects so they won't be
    // accidentally deleted
    if (menuComponent)
        menuComponent->ref();
    if (menuKinematics)
        menuKinematics->ref();

    // Set whether this frame can be repeat-activated by default.
    canRepeat = true;
    previousState = false;

    // Set whether idle causes state reversion to true by default.
    idleReverts = true;
    idleState = false;

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

    // Set whether this frame can be repeat-activated by default.
    canRepeat = true;
    previousState = false;

    // Set whether idle causes state reversion to true by default.
    idleReverts = true;
    idleState = false;

    // Initialize the button press state
    pressedState = false;
}

// ------------------------------------------------------------------------
// Destructor - The destructor does nothing
// ------------------------------------------------------------------------
vsMenuButton::~vsMenuButton()
{
    if (menuKinematics)
    {
        vsObject::unrefDelete(menuKinematics);
        menuKinematics = NULL;
    }

    if (menuComponent)
    {
        vsObject::unrefDelete(menuComponent);
        menuComponent = NULL;
    }
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

            // Store the previous state of the button.
            previousState = pressedState;

            // Cause the button to revert if it is set to do so.
            if (idleReverts)
                pressedState = idleState;
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
// Store whether this button can be activated in consecutive frames by
// activation signals or whether two idle signals must be received for it
// to allow re-activation.
// ------------------------------------------------------------------------
void vsMenuButton::setRepeatable(bool repeat)
{
    canRepeat = repeat;
}

// ------------------------------------------------------------------------
// Return whether this button can be activated in consecutive frames by
// activation signals or whether two idle signals must be received for it
// to allow re-activation.
// ------------------------------------------------------------------------
bool vsMenuButton::isRepeatable()
{
    return canRepeat;
}

// ------------------------------------------------------------------------
// Store whether the idle signal will cause this button to revert to a
// default state, storing that state as well.
// ------------------------------------------------------------------------
void vsMenuButton::setIdleReversion(bool reverts, bool state)
{
    idleReverts = reverts;
    idleState = state;
}

// ------------------------------------------------------------------------
// Returns whether or not an idle signal to this button will cause it to
// revert to a default state.
// ------------------------------------------------------------------------
bool vsMenuButton::revertsOnIdle()
{
    return idleReverts;
}

// ------------------------------------------------------------------------
// Returns the state that this menu button will revert to on idle if its
// reversion flag is set to true.
// ------------------------------------------------------------------------
bool vsMenuButton::getRevertState()
{
    return idleState;
}

// ------------------------------------------------------------------------
// Sets the state of the button (updating the previous state).
// ------------------------------------------------------------------------
void vsMenuButton::setState(bool pressed)
{
    previousState = pressedState;
    pressedState = pressed;
}

// ------------------------------------------------------------------------
// Returns whether the menu button was pressed on the last update
// ------------------------------------------------------------------------
bool vsMenuButton::isPressed()
{
    if (canRepeat)
    {
        return pressedState;
    }
    else
    {
        // Basically, if the pressed and previous states are equal, then the
        // final state is always false. Otherwise, the pressed state is valid
        // whether it is true or false.
        if (pressedState == previousState)
            return false;
        return pressedState;
    }
}

