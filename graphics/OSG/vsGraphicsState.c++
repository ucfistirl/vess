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
//    VESS Module:  vsGraphicsState.c++
//
//    Description:  Object used internally by VESS to track the current
//                  graphics state during a scene graph traversal
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsGraphicsState.h++"
#include <stdio.h>

vsGraphicsState *vsGraphicsState::classInstance = NULL;

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsGraphicsState::vsGraphicsState()
{
    // Create the array of active locally-scoped lights
    localLights = new vsArray();

    // Initialize the scene pointer to NULL
    scene = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsGraphicsState::~vsGraphicsState()
{
    // Clean up the lights array
    delete localLights;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsGraphicsState::getClassName()
{
    return "vsGraphicsState";
}

// ------------------------------------------------------------------------
// Static function
// Returns the currently active vsGraphicsState object
// ------------------------------------------------------------------------
vsGraphicsState *vsGraphicsState::getInstance()
{
    // Create the singleton instance if necessary
    if (!classInstance)
    {
        classInstance = new vsGraphicsState();
        classInstance->ref();
    }

    // Return the vsGraphicsState instance
    return classInstance;
}

// ------------------------------------------------------------------------
// Static function
// Destroys the currently active vsGraphicsState object, if it exists
// ------------------------------------------------------------------------
void vsGraphicsState::deleteInstance()
{
    // If the singleton instance exists, destroy it and set the instance
    // pointer to NULL
    if (classInstance)
    {
        vsObject::unrefDelete(classInstance);
        classInstance = NULL;
    }
}

// ------------------------------------------------------------------------
// Clears the internal graphics state and sets the graphics library state
// to default values
// ------------------------------------------------------------------------
void vsGraphicsState::clearState()
{
    vsLightAttribute *light;

    // Clear the current transparency attribute and override flag
    transparencyAttr = NULL;
    transparencyLock = NULL;

    // Clear the current scene pointer
    scene = NULL;

    // Remove all local light sources, being careful to not let them get
    // deleted (This keeps the original behavior of not reference counting
    // anything, otherwise, we could just call removeAllEntries())
    while (localLights->getNumEntries() > 0)
    {
        // Get the first entry
        light = (vsLightAttribute *) localLights->getEntry(0);

        // Reference the entry temporarily (if it's valid)
        if (light != NULL)
           light->ref();
      
        // Remove the entry at the head of the list (even if it's NULL)
        localLights->removeEntryAtIndex(0);

        // Unreference the entry (if it's valid)
        if (light != NULL)
           light->unref();
    }
}

// ------------------------------------------------------------------------
// Internal function
// Packages the current state into an OSG StateSet
// ------------------------------------------------------------------------
void vsGraphicsState::applyState(osg::StateSet *stateSet)
{
    // Set the given OSG StateSet to inherit all of its state attributes
    // from it's node's parents
    stateSet->clear();

    // Apply transparency if one exists in the current graphics state
    if (transparencyAttr)
        transparencyAttr->setState(stateSet);
}

// ------------------------------------------------------------------------
// Internal function
// Adds the light attribute to the graphics state list of local lights
// ------------------------------------------------------------------------
void vsGraphicsState::addLocalLight(vsLightAttribute *lightAttrib)
{
    // Add the given light attribute to the end of the local light array
    // and increment the local light count
    localLights->addEntry(lightAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Removes the light attribute from the graphics state list of local lights
// ------------------------------------------------------------------------
void vsGraphicsState::removeLocalLight(vsLightAttribute *lightAttrib)
{
    int index;

    // Initialize the index to 0.
    index = 0;

    // Try to remove the light from the list (keep a temporary reference to
    // it to keep it from getting deleted)
    lightAttrib->ref();
    localLights->removeEntry(lightAttrib);
    lightAttrib->unref();
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the array of local lights to add
// ------------------------------------------------------------------------
vsArray *vsGraphicsState::getLocalLightsArray()
{
    return localLights;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the current number of local lights to add
// ------------------------------------------------------------------------
int vsGraphicsState::getLocalLightsCount()
{
    return localLights->getNumEntries();
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current root scene object which applies to the current
// traversal.
// ------------------------------------------------------------------------
void vsGraphicsState::setCurrentScene(vsScene *newScene)
{
    scene = newScene;
}

// ------------------------------------------------------------------------
// Internal function
// Gets the current root scene object which applies to the current
// traversal.
// ------------------------------------------------------------------------
vsScene *vsGraphicsState::getCurrentScene()
{
    return(scene);
}

// ------------------------------------------------------------------------
// Sets the attribute that contains the desired transparency state
// ------------------------------------------------------------------------
void vsGraphicsState::setTransparency(vsTransparencyAttribute *newAttrib)
{
    // Only allow the transparency attribute to be set if the override
    // flag is not set
    if (!transparencyLock)
        transparencyAttr = newAttrib;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the attribute that contains the current transparency state
// ------------------------------------------------------------------------
vsTransparencyAttribute *vsGraphicsState::getTransparency()
{
    return transparencyAttr;
}

// ------------------------------------------------------------------------
// Locks the current transparency attribute, using the given address as a
// 'key'. The transparency attribute cannot be changed again until it is
// unlocked with this key address.
// ------------------------------------------------------------------------
void vsGraphicsState::lockTransparency(void *lockAddr)
{
    // Only allow the transparency override flag to be set if it is
    // currently not set
    if (!transparencyLock)
        transparencyLock = lockAddr;
}

// ------------------------------------------------------------------------
// Unlocks the current transparency attribute, using the given address as a
// 'key'; this key must match the key that the attribute was locked with or
// the function will not work.
// ------------------------------------------------------------------------
void vsGraphicsState::unlockTransparency(void *lockAddr)
{
    if (transparencyLock == lockAddr)
        transparencyLock = NULL;
}
