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

#include <stdio.h>
#include <osg/StateSet>
#include "vsLightAttribute.h++"
#include "vsComponent.h++"
#include "vsGraphicsState.h++"

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
    lightOn = VS_FALSE;
 
    // Insure the OSG light is set to OFF, to match the VESS light.
    lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);

    // Initialize the light as a global light.
    lightScope = VS_LIGHT_MODE_GLOBAL;
    
    attachedFlag = 0;
    parentNode = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the OSG objects by unrefing them.  OSG deletes them.
// ------------------------------------------------------------------------
vsLightAttribute::~vsLightAttribute()
{
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

    // Set the temporary vector value and pass it to OSG.
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
    
    colorVector = lightObject->getAmbient();
    
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

    // Set the temporary vector value and pass it to OSG.
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
    
    colorVector = lightObject->getDiffuse();
    
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

    // Set the temporary vector value and pass it to OSG.
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
    
    colorVector = lightObject->getSpecular();
    
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

    // Set the temporary vector value and pass it to OSG.
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
    
    positionVector = lightObject->getPosition();
    
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

    // Set the temporary vector value and pass it to OSG.
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
    
    directionVector = lightObject->getDirection();
    
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
    lightObject->setSpotExponent(exponent);
    lightObject->setSpotCutoff(VS_DEG2RAD(cutoffDegrees));
}

// ------------------------------------------------------------------------
// Retrieves the constants used in the spotlight intensity calculation.
// NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpotlightValues(double *exponent,
                                          double *cutoffDegrees)
{
    if (exponent)
        *exponent = lightObject->getSpotExponent();
    if (cutoffDegrees)
        *cutoffDegrees = VS_RAD2DEG(lightObject->getSpotCutoff());
}

// ------------------------------------------------------------------------
// Sets the scope of this light source. The scope of a light determines
// whether the light source affects only objects under it in the node tree,
// or every object in the scene.
// ------------------------------------------------------------------------
void vsLightAttribute::setScope(int scope)
{
    if (lightScope == scope)
        return;

    switch (scope)
    {
        case VS_LIGHT_MODE_GLOBAL:
            // Add the light to the scene if it is on.
            if (lightOn)
            {
                addToScene();
                lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
            }
            else
            {
                lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
            }
            break;

        case VS_LIGHT_MODE_LOCAL:
            // Remove the light from the scene's list, turn it off.
            removeFromScene();

            lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
            break;

        default:
            printf("vsLightAttribute::setScope: Unrecognized scope constant\n");
            return;
    }

    lightScope = scope;
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
    lightOn = VS_TRUE;

    // If it is a global light, attempt to add it to the scene now.
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
    {
        addToScene();
        lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
    }
}

// ------------------------------------------------------------------------
// Deactivates this light source
// ------------------------------------------------------------------------
void vsLightAttribute::off()
{
    lightOn = VS_FALSE;

    // If it is a global light, attempt to remove it to the scene now.
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
    {
        removeFromScene();
        lightNode->setLocalStateSetModes(osg::StateAttribute::OFF);
    }
}

// ------------------------------------------------------------------------
// Returns a flag indicating if this light source is currently active
// ------------------------------------------------------------------------
int vsLightAttribute::isOn()
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
                lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
            }
            else
            {
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
        graphicsState->removeLocalLight(this);
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

        scene->unref();
    }

    // Store a refernece to the new scene and ref() it.
    scene = newScene;
    if (scene != NULL)
    {
        scene->ref();
    }
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
int vsLightAttribute::canAttach()
{
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLightAttribute::attach(vsNode *theNode)
{
    vsNode  *searchNode;

    if (attachedFlag)
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
// JPD: Do vsScene's need three OSG::Group's (like Components),
// or is one OK???
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
                lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
            }
            else
            {
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

    attachedFlag = 1;
    parentNode = theNode;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLightAttribute::detach(vsNode *theNode)
{
    if (!attachedFlag)
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

    attachedFlag = 0;
    parentNode = NULL;
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsLightAttribute::attachDuplicate(vsNode *theNode)
{
    vsLightAttribute *newAttrib;
    double p1, p2, p3, p4;
 
    newAttrib = new vsLightAttribute();

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

    if (isOn())
        newAttrib->on();
    else
        newAttrib->off();

    theNode->addAttribute(newAttrib);
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
    int index;

    // If we have no scene, then insure we set the active state to false.
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
        return (true);
    }

    // Return false to indicate that the light was not removed.
    return (false);
}
