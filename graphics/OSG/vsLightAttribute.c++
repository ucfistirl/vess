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
//    VESS Module:  vsLightAttribute.c++
//
//    Description:  Specifies that geometry should be drawn as if lit with
//                  the parameters in this object. Multiple lights can
//                  affect the same geometry.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include "vsLightAttribute.h++"
#include "vsComponent.h++"
#include "vsGraphicsState.h++"
#include <osg/StateSet>
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor - Creates the corresponding OSG objects and
// initializes the light settings
// ------------------------------------------------------------------------
vsLightAttribute::vsLightAttribute()
{
    // Initialize the scene to NULL.
    scene = NULL;

    // This specifies if the light has been placed in a lightList.
    active = false;

    // Initialize the lightHookGroup (the node to which this attribute
    // is attached) to NULL
    lightHookGroup = NULL;

    // Create and reference the OSG objects used by this light.
    lightNode = new osg::LightSource();
    lightNode->ref();
    lightObject = new osg::Light();
    lightObject->ref();

    // Set the lightsource's light to lightObject.
    lightNode->setLight(lightObject);

    // Set the initial colors of the light.
    setAmbientColor(0.0, 0.0, 0.0);
    setDiffuseColor(0.0, 0.0, 0.0);
    setSpecularColor(0.0, 0.0, 0.0);

    // Initialize the VESS light to off.
    lightOn = false;
 
    // Insure the OSG light is set to OFF, to match the VESS light.
    lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);

    // Initialize the light as a global light.
    lightScope = VS_LIGHT_MODE_GLOBAL;
    
    // Begin as unattached
    attachedCount = 0;
    parentNode = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the OSG objects by unrefing them.  OSG deletes them.
// ------------------------------------------------------------------------
vsLightAttribute::~vsLightAttribute()
{
    // Unreference the OSG light node and light object
    lightNode->unref();
    lightObject->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsLightAttribute::getClassName()
{
    return "vsLightAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of this attribute
// ------------------------------------------------------------------------
int vsLightAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_LIGHT;
}

// ------------------------------------------------------------------------
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsLightAttribute::clone()
{
    vsLightAttribute *newAttrib;
    double p1, p2, p3, p4;
 
    // Create another light attribute
    newAttrib = new vsLightAttribute();

    // Copy all parameters to the new attribute
    getAmbientColor(&p1, &p2, &p3);
    newAttrib->setAmbientColor(p1, p2, p3);
    getDiffuseColor(&p1, &p2, &p3);
    newAttrib->setDiffuseColor(p1, p2, p3);
    getSpecularColor(&p1, &p2, &p3);
    newAttrib->setSpecularColor(p1, p2, p3);
    getAttenuationVals(&p1, &p2, &p3);
    newAttrib->setAttenuationVals(p1, p2, p3);
    getPosition(&p1, &p2, &p3, &p4);
    newAttrib->setPosition(p1, p2, p3, p4);
    getSpotlightDirection(&p1, &p2, &p3);
    newAttrib->setSpotlightDirection(p1, p2, p3);
    getSpotlightValues(&p1, &p2);
    newAttrib->setSpotlightValues(p1, p2);
    newAttrib->setScope(getScope());

    // Turn the new light attribute on or off, as appropriate
    if (isOn())
        newAttrib->on();
    else
        newAttrib->off();

    // Return the clone
    return newAttrib;
}

// ------------------------------------------------------------------------
// Retrieves the category of this attribute
// ------------------------------------------------------------------------
int vsLightAttribute::getAttributeCategory()
{
    return VS_ATTRIBUTE_CATEGORY_OTHER;
}

// ------------------------------------------------------------------------
// Sets the ambient RGB color for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setAmbientColor(double r, double g, double b)
{
    osg::Vec4 colorVector;

    // Place the color values in an OSG vector and pass it to OSG.
    colorVector.set(r, g, b, 1.0f);
    lightObject->setAmbient(colorVector);
}

// ------------------------------------------------------------------------
// Retrieves the ambient RGB color for this light. NULL pointers may be
// passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getAmbientColor(double *r, double *g, double *b)
{
    osg::Vec4 colorVector;
    
    // Get the ambient colors from the light object
    colorVector = lightObject->getAmbient();
    
    // Return each color component if we were given a valid pointer 
    // for it
    if (r)
        *r = colorVector.x();
    if (g)
        *g = colorVector.y();
    if (b)
        *b = colorVector.z();
}

// ------------------------------------------------------------------------
// Sets the diffuse RGB color for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setDiffuseColor(double r, double g, double b)
{
    osg::Vec4 colorVector;

    // Place the color values in an OSG vector and pass it to OSG.
    colorVector.set(r, g, b, 1.0f);
    lightObject->setDiffuse(colorVector);
}

// ------------------------------------------------------------------------
// Retrieves the diffuse RGB color for this light. NULL pointers may be
// passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getDiffuseColor(double *r, double *g, double *b)
{
    osg::Vec4 colorVector;
    
    // Get the diffuse colors from the light object
    colorVector = lightObject->getDiffuse();
    
    // Return each color component if we were given a valid pointer 
    // for it
    if (r)
        *r = colorVector.x();
    if (g)
        *g = colorVector.y();
    if (b)
        *b = colorVector.z();
}

// ------------------------------------------------------------------------
// Sets the specular RGB color for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setSpecularColor(double r, double g, double b)
{
    osg::Vec4 colorVector;

    // Place the color values in an OSG vector and pass it to OSG.
    colorVector.set(r, g, b, 1.0f);
    lightObject->setSpecular(colorVector);
}

// ------------------------------------------------------------------------
// Retrieves the specular RGB color for this light. NULL pointers may be
// passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpecularColor(double *r, double *g, double *b)
{
    osg::Vec4 colorVector;
    
    // Get the specular colors from the light object
    colorVector = lightObject->getSpecular();
    
    // Return each color component if we were given a valid pointer 
    // for it
    if (r)
        *r = colorVector.x();
    if (g)
        *g = colorVector.y();
    if (b)
        *b = colorVector.z();
}

// ------------------------------------------------------------------------
// Sets the constants in the light attenuation equation for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setAttenuationVals(double quadratic, double linear, 
                                          double constant)
{
    // Pass the light coefficients to their respective OSG counterparts
    lightObject->setConstantAttenuation(constant);
    lightObject->setLinearAttenuation(linear);
    lightObject->setQuadraticAttenuation(quadratic);
}

// ------------------------------------------------------------------------
// Retrieves the constants for the light attenuation equation for this
// light. NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getAttenuationVals(double *quadratic, double *linear,
                                          double *constant)
{
    // Make sure we were given a valid pointer for each component, then
    // retrieve its corresponding value from the OSG light object
    if (quadratic)
        *quadratic = lightObject->getQuadraticAttenuation();
    if (linear)
        *linear = lightObject->getLinearAttenuation();
    if (constant)
        *constant = lightObject->getConstantAttenuation();
}

// ------------------------------------------------------------------------
// Sets the position of this light source. The fourth value, w, is a
// homogeneous coordinate scale; passing in 0 for w results in a light
// source that is infinitely far away from the viewer.
// ------------------------------------------------------------------------
void vsLightAttribute::setPosition(double x, double y, double z, double w)
{
    osg::Vec4 positionVector;

    // Store the location in an OSG vector and pass it to the Light object
    positionVector.set(x, y, z, w);
    lightObject->setPosition(positionVector);
}

// ------------------------------------------------------------------------
// Retrieves the position and coordinate scale for this light source. NULL
// pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getPosition(double *x, double *y, double *z, double *w)
{
    osg::Vec4 positionVector;
    
    // Retrieve the position from the OSG Light object
    positionVector = lightObject->getPosition();
    
    // Return each coordinate if we were given a valid pointer 
    // for it
    if (x)
        *x = positionVector.x();
    if (y)
        *y = positionVector.y();
    if (z)
        *z = positionVector.x();
    if (w)
        *w = positionVector.w();
}

// ------------------------------------------------------------------------
// Sets the direction that a spotlight shines its light in
// ------------------------------------------------------------------------
void vsLightAttribute::setSpotlightDirection(double dx, double dy, double dz)
{
    osg::Vec3 directionVector;

    // Store the direction coordinates in an OSG vector and pass it to
    // the Light object
    directionVector.set(dx, dy, dz);
    lightObject->setDirection(directionVector);
}

// ------------------------------------------------------------------------
// Retrieves the direction that a spotlight is shining its light in. NULL
// pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpotlightDirection(double *dx, double *dy, double *dz)
{
    osg::Vec3 directionVector;
    
    // Retrieve the direction from the OSG Light object
    directionVector = lightObject->getDirection();
    
    // Return each coordinate if we were given a valid pointer 
    // for it
    if (dx)
        *dx = directionVector.x();
    if (dy)
        *dy = directionVector.y();
    if (dz)
        *dz = directionVector.z();
}

// ------------------------------------------------------------------------
// Sets the constants used in the spotlight intensity calculation
// ------------------------------------------------------------------------
void vsLightAttribute::setSpotlightValues(double exponent, double cutoffDegrees)
{
    // Pass the values to the OSG Light object
    lightObject->setSpotExponent(exponent);
    lightObject->setSpotCutoff(cutoffDegrees);
}

// ------------------------------------------------------------------------
// Retrieves the constants used in the spotlight intensity calculation.
// NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpotlightValues(double *exponent,
                                          double *cutoffDegrees)
{
    // Make sure we were given a valid pointer for each value, then
    // retrieve its corresponding value from the OSG Light object
    if (exponent)
        *exponent = lightObject->getSpotExponent();
    if (cutoffDegrees)
        *cutoffDegrees = AT_RAD2DEG(lightObject->getSpotCutoff());
}

// ------------------------------------------------------------------------
// Sets the scope of this light source. The scope of a light determines
// whether the light source affects only objects under it in the node tree,
// or every object in the scene.
// ------------------------------------------------------------------------
void vsLightAttribute::setScope(int scope)
{
    // If the scope parameter matches the light attribute's current scope
    // setting, do nothing
    if (lightScope == scope)
        return;

    // Interpret the scope parameter
    switch (scope)
    {
        case VS_LIGHT_MODE_GLOBAL:
            // Only add the light to the scene if it is on.
            if (lightOn)
            {
                addToScene();

                // Turn lighting on on the light node's StateSet
                lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
            }
            else
            {
                // Turn lighting off on the light node's StateSet
                lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
            }
            break;

        case VS_LIGHT_MODE_LOCAL:
            // Remove the light from the scene's list and turn it off.
            removeFromScene();
            lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
            break;

        default:
            printf("vsLightAttribute::setScope: Unrecognized scope constant\n");
            return;
    }

    // Remember the scope setting
    lightScope = scope;

    // If we're attached to a node, mark it and its ancestors and descendants
    // dirty
    if (parentNode)
        parentNode->dirty();
}

// ------------------------------------------------------------------------
// Retrieves the scope of this light source
// ------------------------------------------------------------------------
int vsLightAttribute::getScope()
{
    return lightScope;
}

// ------------------------------------------------------------------------
// Activates this light source
// ------------------------------------------------------------------------
void vsLightAttribute::on()
{
    // Flag the light as turned on
    lightOn = true;

    // If it is a global light, attempt to add it to the scene now.
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
    {
        addToScene();

        // Turn lighting on on the light node's StateSet
        lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
    }
}

// ------------------------------------------------------------------------
// Deactivates this light source
// ------------------------------------------------------------------------
void vsLightAttribute::off()
{
    // Flag the light as turned off
    lightOn = false;

    // If it is a global light, attempt to remove it to the scene now.
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
    {
        removeFromScene();

        // Turn lighting off on the light node's StateSet
        lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
    }
}

// ------------------------------------------------------------------------
// Returns a flag indicating if this light source is currently active
// ------------------------------------------------------------------------
bool vsLightAttribute::isOn()
{
    return lightOn;
}

// ------------------------------------------------------------------------
// Internal function
// Perform specified action during the VESS traversal
// This adds the local light to the vsGraphicsState
// ------------------------------------------------------------------------
void vsLightAttribute::apply()
{
    vsGraphicsState *graphicsState = vsGraphicsState::getInstance();

    // If the scene is not set.
    if (scene == NULL)
    {
        // Get and set the scene node that this light is part of.
        setScene(graphicsState->getCurrentScene());

        // If the light is global, try to add it to the list.
        if ((lightScope == VS_LIGHT_MODE_GLOBAL) && (!active))
        {
            // Add the light to the scene if it is on.
            if (lightOn)
            {
                addToScene();

                // Turn lighting on on the light node's StateSet
                lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
            }
            else
            {
                // Turn lighting off on the light node's StateSet
                lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
            }
        }
    }

    // If this is a local light, add it to the graphics state.
    if (lightScope == VS_LIGHT_MODE_LOCAL)
    {
        graphicsState->addLocalLight(this);
    }
}

// ------------------------------------------------------------------------
// Internal function
// This removes the local light from the local light listing kept by
// the vsGraphicsState
// ------------------------------------------------------------------------
void vsLightAttribute::restoreSaved()
{
    vsGraphicsState *graphicsState = vsGraphicsState::getInstance();

    // If this is a local light, remove it from the graphics state.
    if (lightScope == VS_LIGHT_MODE_LOCAL)
    {
        graphicsState->removeLocalLight(this);
    }
}

// ------------------------------------------------------------------------
// Internal function
// Set the root scene object which this light is part of.
// ------------------------------------------------------------------------
void vsLightAttribute::setScene(vsScene *newScene)
{
    // If the argument scene is the same as the one we have, do nothing.
    if (scene == newScene)
        return;

    // If this light's scene is not NULL, unref() it because it will change.
    if (scene != NULL)
    {
        // Remove it from the old scene's light list.
        removeFromScene();
    }

    // Store a refernece to the new scene
    scene = newScene;
}

// ------------------------------------------------------------------------
// Internal function
// Get the root scene object which this light is part of.
// ------------------------------------------------------------------------
vsScene *vsLightAttribute::getScene()
{
    return(scene);
}

// ------------------------------------------------------------------------
// Internal function
// Enables the local light.  This is used by the draw callback.
// ------------------------------------------------------------------------
void vsLightAttribute::enableLocalLight(osg::State *state)
{
    // If the light is local and it is on, add it to the light list.
    if ((lightScope == VS_LIGHT_MODE_LOCAL) && lightOn)
    {
        // If it was added to the list successfully, enable it.
        if (addToScene())
        {
            // Apply the light values for this light to the OpenGL, light.
            lightObject->apply(*state);

            // Turn it on.
            state->applyMode(GL_LIGHT0+lightObject->getLightNum(),
                osg::StateAttribute::ON);
        }
    }
}

// ------------------------------------------------------------------------
// Internal function
// Disable the local light.  This is used by the draw callback.
// ------------------------------------------------------------------------
void vsLightAttribute::disableLocalLight(osg::State *state)
{
    // If the light is local, remove it from the light list.
    if ((lightScope == VS_LIGHT_MODE_LOCAL) && (lightOn))
    {
        // If it was removed from the light list.
        if (removeFromScene())
        {
            // Turn it off.
            state->applyMode(GL_LIGHT0+lightObject->getLightNum(),
                osg::StateAttribute::OFF);
        }
    }
}

// ------------------------------------------------------------------------
// Internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsLightAttribute::canAttach()
{
    // If we're already attached to a node, don't allow any further 
    // attachments
    if (attachedCount)
        return false;

    // Otherwise, we can be attached
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLightAttribute::attach(vsNode *theNode)
{
    vsNode  *searchNode;

    // If we're already attached to a node, bail out
    if (attachedCount)
    {
        printf("vsLightAttribute::attach: Attribute is already attached\n");
        return;
    }

    // If it is a component, add it to the component.
    if (theNode->getNodeType() == VS_NODE_TYPE_COMPONENT)
    {
        lightHookGroup = ((vsComponent *)theNode)->getLightHook();
        lightHookGroup->addChild(lightNode);
    }
    // If it is a scene, add it to the scene.
    else if (theNode->getNodeType() == VS_NODE_TYPE_SCENE)
    {
        lightHookGroup = ((vsScene *)theNode)->getBaseLibraryObject();
        lightHookGroup->addChild(lightNode);
    }
    // Else it is not a valid node to put a light on.
    else
    {
        printf("vsLightAttribute::attach: Can only attach light "
            "attributes to vsComponents or vsScenes\n");
        return;
    }

    // Now Search for the root scene object of this node.
    // Initialize the search node to the given node.
    searchNode = theNode;

    // Go up to the root of the given node.
    while (searchNode->getParent(0) != NULL)
    {
        searchNode = searchNode->getParent(0);
    }

    // If the root node is a Scene, store a reference to it and attempt to
    // add the light to it.
    if (searchNode->getNodeType() == VS_NODE_TYPE_SCENE)
    {
        // Store the a pointer to the scene root.
        setScene((vsScene *) searchNode);

        // If this is a global light, simply add the light to the scene.
        if (lightScope == VS_LIGHT_MODE_GLOBAL)
        {
            // Add the light to the scene if it is on.
            if (lightOn)
            {
                addToScene();

                // Turn lighting on on the light node's StateSet
                lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
            }
            else
            {
                // Turn lighting off on the light node's StateSet
                lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
            }
        }
        // Else if it is local, mark the node dirty.
        else if (lightScope == VS_LIGHT_MODE_LOCAL)
        {
            theNode->dirty();
        }
    }
    // Else there is no root scene, so simply mark the parent dirty.
    else
    {
        theNode->dirty();
    }

    // Mark the attribute as attached and remember the node we're attached to
    attachedCount = 1;
    parentNode = theNode;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLightAttribute::detach(vsNode *theNode)
{
    // If we're not attached, we don't need to do anything
    if (!attachedCount)
    {
        printf("vsLightAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Remove the actual light node from the scene.
    lightHookGroup->removeChild(lightNode);
    lightHookGroup = NULL;

    // Lose the reference to the scene this light used to belong to.
    setScene(NULL);

    // If the light is a local light, mark the parent dirty so it can
    // modify the local light callback to not contain this light.
    if (lightScope == VS_LIGHT_MODE_LOCAL)
    {
        parentNode->dirty();
    }

    // Mark the node as unattached, and forget the node we were attached to
    attachedCount = 0;
    parentNode = NULL;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsLightAttribute::attachDuplicate(vsNode *theNode)
{
    // Add a clone of this attribute to the given node
    theNode->addAttribute(this->clone());
}

// ------------------------------------------------------------------------
// Private function
// Adds this light to the scene's list of lights.
// Returns true if it actually adds the light to the scene.
// ------------------------------------------------------------------------
bool vsLightAttribute::addToScene()
{
    int index;

    // If the scene is currently NULL, return false.
    if (scene == NULL)
    {
        return(false);
    }

    // Add the light to the scene.
    if ((index = scene->addLight(this)) != -1)
    {
        // Set the light number of the OSG light to the returned available one.
        lightObject->setLightNum(index);

        // Specify that the light has been successfully placed in the scene.
        active = true;
    }

    // Return the active state of the light.
    return(active);
}

// ------------------------------------------------------------------------
// Private function
// Removes this light from the scene's list of lights.
// Returns true if it actually removed the light from the scene.
// ------------------------------------------------------------------------
bool vsLightAttribute::removeFromScene()
{
    // If we have no scene, then set the active state to false.
    if (scene == NULL)
    {
        active = false;
    }

    // If the light has been placed in the lightList, remove it.
    if (active)
    {
        // Remove the light from the scene.
        scene->removeLight(this);

        // Set its active state to false now that is it removed.
        active = false;

        // Return true to indicate that the light was removed.
        return true;
    }

    // Return false to indicate that the light was not removed.
    return false;
}
