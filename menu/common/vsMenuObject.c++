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
//    VESS Module:  vsMenuObject.c++
//
//    Description:  This is the base class for all of the objects that
//                  the vsMenuSystem visually represents. It derives from
//                  vsObject for its reference counting features.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMenuObject.h++"

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a menu object with no visual
// representation.
// ------------------------------------------------------------------------
vsMenuObject::vsMenuObject()
{
    objectName = NULL;
    inputAccel = NULL;
    selectable = true;
    enabled = true;

    menuComponent = NULL;
    menuKinematics = NULL;
}

// ------------------------------------------------------------------------
// Constructor - This constructor initializes a button with the given menu
// component and kinematics object. The kinematics object may be null if
// you do not want the component manipulated automatically on updates.
// ------------------------------------------------------------------------
vsMenuObject::vsMenuObject(vsComponent *component, vsKinematics *kinematics)
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

    objectName = NULL;
    inputAccel = NULL;
    selectable = true;
    enabled = true;
}

// ------------------------------------------------------------------------
// Destructor - The destructor frees the memory used by the object name
// ------------------------------------------------------------------------
vsMenuObject::~vsMenuObject()
{
    if (objectName)
        free(objectName);

    if (menuComponent)
    {
        vsObject::unrefDelete(menuComponent);
        menuComponent = NULL;
    }

    if (menuKinematics)
    {
        vsObject::unrefDelete(menuKinematics);
        menuKinematics = NULL;
    }
}

// ------------------------------------------------------------------------
// Returns the name of this class
// ------------------------------------------------------------------------
const char *vsMenuObject::getClassName()
{
    return "vsMenuObject";
}

// ------------------------------------------------------------------------
// Virtual Function
// Updates the menu object according to the signal it received from the
// indicated menu frame.
// ------------------------------------------------------------------------
void vsMenuObject::update(vsMenuSignal signal, vsMenuFrame *frame)
{
/*
    switch (signal)
    {
        case VS_MENU_SIGNAL_IDLE:
        {
            // Update the kinematics object if it exists
            if (menuKinematics)
                menuKinematics->update();
        }
        break;
    }
*/
}

// ------------------------------------------------------------------------
// Returns the component used for visualizing this object
// ------------------------------------------------------------------------
vsComponent *vsMenuObject::getComponent()
{
    return menuComponent;
}

// ------------------------------------------------------------------------
// Returns the kinematics associated with this object's rendering component
// ------------------------------------------------------------------------
vsKinematics *vsMenuObject::getKinematics()
{
    return menuKinematics;
}

// ------------------------------------------------------------------------
// Change the name of this menu object to the new name, making a new copy
// of the string in local memory and freeing the old name if necessary
// ------------------------------------------------------------------------
void vsMenuObject::setName(char *name)
{
    // Free the old name if memory had been allocated for it
    if (objectName)
        free(objectName);

    // If the new argument is null, it must be handled separately
    if (name == NULL)
    {
        // Simply set the name to null, as there is no data to set
        objectName = NULL;
    }
    else
    {
        // Allocate memory to hold the new string plus its terminating null,
        // then copy the string into our variable
        objectName = (char *)malloc((strlen(name) + 1) * sizeof(char));
        strcpy(objectName, name);
    }
}

// ------------------------------------------------------------------------
// Return the string name of this menu object
// ------------------------------------------------------------------------
char *vsMenuObject::getName()
{
    return objectName;
}

// ------------------------------------------------------------------------
// Sets the button that can be used to automatically activate this menu
// object from the menu system
// ------------------------------------------------------------------------
void vsMenuObject::setAccelerator(vsInputButton *accelerator)
{
    inputAccel = accelerator;
}

// ------------------------------------------------------------------------
// Returns the object used to automatically activate this menu item
// ------------------------------------------------------------------------
vsInputButton *vsMenuObject::getAccelerator()
{
    return inputAccel;
}

// ------------------------------------------------------------------------
// Sets whether or not the object can be selected or not
// ------------------------------------------------------------------------
void vsMenuObject::setSelectable(bool canSelect)
{
    selectable = canSelect;
}

// ------------------------------------------------------------------------
// Returns whether the object can be selected or not
// ------------------------------------------------------------------------
bool vsMenuObject::isSelectable()
{
    return selectable;
}

// ------------------------------------------------------------------------
// Sets whether or not the object will be expressed in a menu system.
// ------------------------------------------------------------------------
void vsMenuObject::setEnabled(bool state)
{
    enabled = state;
}

// ------------------------------------------------------------------------
// Returns whether or not the object is expressed in a menu system.
// ------------------------------------------------------------------------
bool vsMenuObject::isEnabled()
{
    return enabled;
}
