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
    // Create the growable array, with initial size of 8 and increment of 1.
    localLightList = new vsGrowableArray(8, 1);

    // Set the first value in the list to the given attribute.
    localLightList->setData(0, la);

    // Initialize the count of light attributes to 1.
    localLightCount = 1;
}

vsLocalLightCallback::vsLocalLightCallback(vsGrowableArray *lightArray,
                                           int length)
                     : osg::Drawable::DrawCallback()
{
    // Create the growable array, with initial size of 8 and increment of 1.
    localLightList = new vsGrowableArray(8, 1);

    // Go through the given light array and add every light in it to our
    // local list
    for (localLightCount = 0; localLightCount < length; localLightCount++)
    {
        localLightList->setData(localLightCount,
            lightArray->getData(localLightCount));
    }
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
int vsLocalLightCallback::setLocalLights(vsGrowableArray *lightArray,
                                         int length)
{
    // Go through the given light array and add every light in it to our
    // local list.
    for (localLightCount = 0; localLightCount < length; localLightCount++)
    {
        localLightList->setData(localLightCount,
            lightArray->getData(localLightCount));
    }

    // Return the new light count
    return (localLightCount);
}

// ------------------------------------------------------------------------
// Add the given local light array to this objects local light array
// ------------------------------------------------------------------------
int vsLocalLightCallback::addLocalLights(vsGrowableArray *lightArray,
                                         int length)
{
    int loop;

    // Go through the array and add every light in it.
    for (loop = 0; loop < length; loop++)
    {
        addLocalLight((vsLightAttribute *)
            lightArray->getData(loop));
    }

    // Return the new local light count
    return (localLightCount);
}

// ------------------------------------------------------------------------
// Adds a new vsLightAttribute which will be used as a local light
// ------------------------------------------------------------------------
int vsLocalLightCallback::addLocalLight(vsLightAttribute *la)
{
    // Add the light to the list, it may pass the light limit but that is
    // handled by the light attribute when turning the light on.
    localLightList->setData(localLightCount++, la);

    // Return the new local light count.
    return(localLightCount);
}

// ------------------------------------------------------------------------
// Removes the vsLightAttribute in the given array which will no longer
// be used as a local light.
// ------------------------------------------------------------------------
int vsLocalLightCallback::removeLocalLights(vsGrowableArray *lightArray,
                                         int length)
{
    int loop;

    // Go through the given light array and remove every light in it from
    // our local light list
    for (loop = 0; loop < length; loop++)
    {
        removeLocalLight((vsLightAttribute *)
            lightArray->getData(loop));
    }

    // Return the number of lights remaining in our light list
    return (localLightCount);
}

// ------------------------------------------------------------------------
// Removes the given vsLightAttribute which will no longer be used as a
// local light.
// ------------------------------------------------------------------------
int vsLocalLightCallback::removeLocalLight(vsLightAttribute *la)
{
    int index;

    // Initialize the index to 0.
    index = 0;

    // Search our local list for the given light attribute
    for (index = 0; index < localLightCount; index++)
    {
        // If we are on the matching light, set it to NULL.
        if (localLightList->getData(index) == la)
        {
            // Decrement the count to reflect the removed light.
            // Place what was the last element into the removed slot.
            localLightList->setData(index,
                localLightList->getData(--localLightCount));

            // Return the number of lights remaining in our light list
            return (localLightCount);
        }
    }

    // Return the number of lights in our light list
    return (localLightCount);
}

// ------------------------------------------------------------------------
// Retrieves the count of local lights of this callback
// ------------------------------------------------------------------------
int vsLocalLightCallback::getLocalLightCount()
{
    return (localLightCount);
}

// ------------------------------------------------------------------------
// This is the virtually defined function which OSG uses as the actual
// callback.  This will enable the given local lights, draw, and then
// disable them.
// ------------------------------------------------------------------------
void vsLocalLightCallback::drawImplementation(osg::State &state, const
                                              osg::Drawable *drawable) const
{
    int  loop;
    vsLightAttribute *lightAttr;

    // Enable the local lights this callback handles.
    for (loop = 0; loop < localLightCount; loop++)
    {
        // Get the next light attribute from the array.  We have to cast
        // the array because it is being made const by the callback method
        // declaration.  It's safe to do this since we're not modifying
        // the array.  (See Stroustrup (2nd ed.), pg. 149)
        lightAttr = (vsLightAttribute *)
            ((vsGrowableArray *)localLightList)->getData(loop);

        // Enable the local light
        lightAttr->enableLocalLight(&state);
    }

    // Draw the drawable with this local light on.
    drawable->drawImplementation(state);

    // Disable the local light.
    for (loop = 0; loop < localLightCount; loop++)
    {
        // Get the next light attribute from the array (same method as above).
        lightAttr = (vsLightAttribute *)
            ((vsGrowableArray *)localLightList)->getData(loop);

        // Disable the local light 
        lightAttr->disableLocalLight(&state);
    }
}
