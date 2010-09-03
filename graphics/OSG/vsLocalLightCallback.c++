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
//    VESS Module:  vsLocalLightCallback
//
//    Description:  OSG CullVisitor callback function used to allocate
//                  and activate OpenGL lights for VESS light attributes
//                  that are configured as local lights.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsLocalLightCallback.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor - Creates the callback OSG objects and
// gives it the first local light it will work with
// ------------------------------------------------------------------------
vsLocalLightCallback::vsLocalLightCallback(vsLightAttribute *la)
                     : osg::Drawable::DrawCallback()
{
    // Create the array
    localLightList = new vsArray();

    // Set the first value in the list to the given attribute.
    localLightList->addEntry(la);
}

vsLocalLightCallback::vsLocalLightCallback(vsArray *lightArray)
                    : osg::Drawable::DrawCallback()
{
    int i;

    // Create the growable array, with initial size of 8 and increment of 1.
    localLightList = new vsArray();

    // Go through the given light array and add every light in it to our
    // local list
    for (i = 0; i < lightArray->getNumEntries(); i++)
        localLightList->setEntry(i, lightArray->getEntry(i));
}

// ------------------------------------------------------------------------
// Destructor - Deletes any allocated data
// ------------------------------------------------------------------------
vsLocalLightCallback::~vsLocalLightCallback()
{
    delete localLightList;
}

// ------------------------------------------------------------------------
// Set this objects local light array to the given local light array
// ------------------------------------------------------------------------
int vsLocalLightCallback::setLocalLights(vsArray *lightArray)
{
    vsLightAttribute *light;
    int i;

    // Remove all entries in the current light list, being careful to not
    // let them get deleted.  (This keeps the original growable array
    // behavior of not reference counting anything, otherwise, we could
    // just call removeAllEntries())
    while (localLightList->getNumEntries() > 0)
    {
        // Get the first entry
        light = (vsLightAttribute *) localLightList->getEntry(0);
    
        // Reference the entry temporarily (if it's valid)
        if (light != NULL)
           light->ref();

        // Remove the entry at the head of the list (even if it's NULL)
        localLightList->removeEntryAtIndex(0);

        // Unreference the entry (if it's valid)
        if (light != NULL)
           light->unref();
    }

    // Go through the given light array and add every light in it to our
    // local list.
    for (i = 0; i < lightArray->getNumEntries(); i++)
        localLightList->setEntry(i, lightArray->getEntry(i));

    // Return the new light count
    return localLightList->getNumEntries();
}

// ------------------------------------------------------------------------
// Add the given local light array to this objects local light array
// ------------------------------------------------------------------------
int vsLocalLightCallback::addLocalLights(vsArray *lightArray)
{
    int loop;

    // Go through the array and add every light in it.
    for (loop = 0; loop < lightArray->getNumEntries(); loop++)
    {
        addLocalLight((vsLightAttribute *) lightArray->getEntry(loop));
    }

    // Return the new local light count
    return localLightList->getNumEntries();
}

// ------------------------------------------------------------------------
// Adds a new vsLightAttribute which will be used as a local light
// ------------------------------------------------------------------------
int vsLocalLightCallback::addLocalLight(vsLightAttribute *la)
{
    // Add the light to the list, it may pass the light limit but that is
    // handled by the light attribute when turning the light on.
    localLightList->addEntry(la);

    // Return the new local light count.
    return localLightList->getNumEntries();
}

// ------------------------------------------------------------------------
// Removes the vsLightAttribute in the given array which will no longer
// be used as a local light.
// ------------------------------------------------------------------------
int vsLocalLightCallback::removeLocalLights(vsArray *lightArray)
{
    int loop;

    // Go through the given light array and remove every light in it from
    // our local light list
    for (loop = 0; loop < lightArray->getNumEntries(); loop++)
    {
        removeLocalLight((vsLightAttribute *) lightArray->getEntry(loop));
    }

    // Return the number of lights remaining in our light list
    return localLightList->getNumEntries();
}

// ------------------------------------------------------------------------
// Removes the given vsLightAttribute which will no longer be used as a
// local light.
// ------------------------------------------------------------------------
int vsLocalLightCallback::removeLocalLight(vsLightAttribute *la)
{
    // Try to remove the given light source from our list (keep a temporary
    // reference to it to keep it from getting deleted)
    la->ref();
    localLightList->removeEntry(la);
    la->unref();

    // Return the number of lights remaining in our light list
    return localLightList->getNumEntries();
}

// ------------------------------------------------------------------------
// Retrieves the count of local lights of this callback
// ------------------------------------------------------------------------
int vsLocalLightCallback::getLocalLightCount()
{
    return localLightList->getNumEntries();
}

// ------------------------------------------------------------------------
// This is the virtually defined function which OSG uses as the actual
// callback.  This will enable the given local lights, draw, and then
// disable them.
// ------------------------------------------------------------------------
void vsLocalLightCallback::drawImplementation(osg::RenderInfo &info, const
                                              osg::Drawable *drawable) const
{
    int  loop;
    vsLightAttribute *lightAttr;
    vsArray * tempLightList;

    // We need to cast away the const-ness of the local object's
    // light array, in order to perform some necessary functions on it.
    // It's safe to do this, since we are not modifying the array itself.
    // (See Stroustrup (2nd ed.), pg. 149)
    tempLightList = (vsArray *) localLightList;

    // Enable the local lights this callback handles.
    for (loop = 0; loop < tempLightList->getNumEntries(); loop++)
    {
        // Get the next light attribute from the array
        lightAttr = (vsLightAttribute *) tempLightList->getEntry(loop);

        // Enable the local light
        lightAttr->enableLocalLight(info.getState());
    }

    // Draw the drawable with this local light on.
    drawable->drawImplementation(info);

    // Disable the local light.
    for (loop = 0; loop < tempLightList->getNumEntries(); loop++)
    {
        // Get the next light attribute from the array
        lightAttr = (vsLightAttribute *) tempLightList->getEntry(loop);

        // Disable the local light 
        lightAttr->disableLocalLight(info.getState());
    }
}
