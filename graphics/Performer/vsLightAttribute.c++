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
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsLightAttribute.h++"

#include "vsComponent.h++"
#include "vsScene.h++"
#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Constructor - Creates the corresponding Performer objects and
// initializes the light settings
// ------------------------------------------------------------------------
vsLightAttribute::vsLightAttribute()
{
    // Create the Performer light and light source objects, and clear the
    // light hook pointer
    lightHookGroup = NULL;
    lightNode = new pfLightSource();
    lightNode->ref();
    lightObject = new pfLight();
    lightObject->ref();
    
    // Default colors are all black
    setAmbientColor(0.0, 0.0, 0.0);
    setDiffuseColor(0.0, 0.0, 0.0);
    setSpecularColor(0.0, 0.0, 0.0);
    
    // Light start out off
    lightOn = false;
    
    // Performer light starts out off
    lightNode->off();
    
    // Default scope is global
    lightScope = VS_LIGHT_MODE_GLOBAL;
    
    // Attribute starts out unattached
    attachedFlag = 0;
    parentNode = NULL;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the Performer objects
// ------------------------------------------------------------------------
vsLightAttribute::~vsLightAttribute()
{
    lightNode->unref();
    pfDelete(lightNode);
    lightObject->unref();
    pfDelete(lightObject);
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
    // Set the ambient color in both Performer light objects
    lightNode->setColor(PFLT_AMBIENT, r, g, b);
    lightObject->setColor(PFLT_AMBIENT, r, g, b);
}

// ------------------------------------------------------------------------
// Retrieves the ambient RGB color for this light. NULL pointers may be
// passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getAmbientColor(double *r, double *g, double *b)
{
    float red, green, blue;
    
    // Get the ambient color from the Performer light source
    lightNode->getColor(PFLT_AMBIENT, &red, &green, &blue);
    
    // Return the desired data values
    if (r)
        *r = red;
    if (g)
        *g = green;
    if (b)
        *b = blue;
}

// ------------------------------------------------------------------------
// Sets the diffuse RGB color for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setDiffuseColor(double r, double g, double b)
{
    // Set the diffuse color in both Performer light objects
    lightNode->setColor(PFLT_DIFFUSE, r, g, b);
    lightObject->setColor(PFLT_DIFFUSE, r, g, b);
}

// ------------------------------------------------------------------------
// Retrieves the diffuse RGB color for this light. NULL pointers may be
// passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getDiffuseColor(double *r, double *g, double *b)
{
    float red, green, blue;
    
    // Get the diffuse color from the Performer light source
    lightNode->getColor(PFLT_DIFFUSE, &red, &green, &blue);
    
    // Return the desired data values
    if (r)
        *r = red;
    if (g)
        *g = green;
    if (b)
        *b = blue;
}

// ------------------------------------------------------------------------
// Sets the specular RGB color for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setSpecularColor(double r, double g, double b)
{
    // Set the specular color in both Performer light objects
    lightNode->setColor(PFLT_SPECULAR, r, g, b);
    lightObject->setColor(PFLT_SPECULAR, r, g, b);
}

// ------------------------------------------------------------------------
// Retrieves the specular RGB color for this light. NULL pointers may be
// passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpecularColor(double *r, double *g, double *b)
{
    float red, green, blue;
    
    // Get the specular color from the Performer light source
    lightNode->getColor(PFLT_SPECULAR, &red, &green, &blue);
    
    // Return the desired data values
    if (r)
        *r = red;
    if (g)
        *g = green;
    if (b)
        *b = blue;
}

// ------------------------------------------------------------------------
// Sets the constants in the light attenuation equation for this light
// ------------------------------------------------------------------------
void vsLightAttribute::setAttenuationVals(double quadratic, double linear, 
                                          double constant)
{
    // Set the attenuation parameters in both Performer light objects
    lightNode->setAtten(constant, linear, quadratic);
    lightObject->setAtten(constant, linear, quadratic);
}

// ------------------------------------------------------------------------
// Retrieves the constants for the light attenuation equation for this
// light. NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getAttenuationVals(double *quadratic, double *linear,
                                          double *constant)
{
    float quadVal, linVal, constVal;

    // Get the attenuation parameters from the Performer light source
    lightNode->getAtten(&constVal, &linVal, &quadVal);

    // Return the desired data values
    if (quadratic)
        *quadratic = quadVal;
    if (linear)
        *linear = linVal;
    if (constant)
        *constant = constVal;
}

// ------------------------------------------------------------------------
// Sets the position of this light source. The fourth value, w, is a
// homogeneous coordinate scale; passing in 0 for w results in a light
// source that is infinitely far away from the viewer.
// ------------------------------------------------------------------------
void vsLightAttribute::setPosition(double x, double y, double z, double w)
{
    // Set the light position in both Performer light objects
    lightNode->setPos(x, y, z, w);
    lightObject->setPos(x, y, z, w);
}

// ------------------------------------------------------------------------
// Retrieves the position and coordinate scale for this light source. NULL
// pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getPosition(double *x, double *y, double *z, double *w)
{
    float xPos, yPos, zPos, wHomogeneous;
    
    // Get the light position from the Performer light source
    lightNode->getPos(&xPos, &yPos, &zPos, &wHomogeneous);
    
    // Return the desired data values
    if (x)
        *x = xPos;
    if (y)
        *y = yPos;
    if (z)
        *z = zPos;
    if (w)
        *w = wHomogeneous;
}

// ------------------------------------------------------------------------
// Sets the direction that a spotlight shines its light in
// ------------------------------------------------------------------------
void vsLightAttribute::setSpotlightDirection(double dx, double dy, double dz)
{
    // Set the spotlight direction in both Performer light objects
    lightNode->setSpotDir(dx, dy, dz);
    lightObject->setSpotDir(dx, dy, dz);
}

// ------------------------------------------------------------------------
// Retrieves the direction that a spotlight is shining its light in. NULL
// pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpotlightDirection(double *dx, double *dy, double *dz)
{
    float xDir, yDir, zDir;
    
    // Get the spotlight direction from the Performer light source
    lightNode->getSpotDir(&xDir, &yDir, &zDir);
    
    // Return the desired data values
    if (dx)
        *dx = xDir;
    if (dy)
        *dy = yDir;
    if (dz)
        *dz = zDir;
}

// ------------------------------------------------------------------------
// Sets the constants used in the spotlight intensity calculation
// ------------------------------------------------------------------------
void vsLightAttribute::setSpotlightValues(double exponent, double cutoffDegrees)
{
    // Set the spotlight parameters in both Performer light objects
    lightNode->setSpotCone(exponent, cutoffDegrees);
    lightObject->setSpotCone(exponent, cutoffDegrees);
}

// ------------------------------------------------------------------------
// Retrieves the constants used in the spotlight intensity calculation.
// NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getSpotlightValues(double *exponent,
                                          double *cutoffDegrees)
{
    float expVal, cutDeg;
    
    // Get the spotlight parameters from the Performer light source
    lightNode->getSpotCone(&expVal, &cutDeg);
    
    // Return the desired data values
    if (exponent)
        *exponent = expVal;
    if (cutoffDegrees)
        *cutoffDegrees = cutDeg;
}

// ------------------------------------------------------------------------
// Sets the scope of this light source. The scope of a light determines
// whether the light source affects only objects under it in the node tree,
// or every object in the scene.
// ------------------------------------------------------------------------
void vsLightAttribute::setScope(int scope)
{
    // Don't change the scope if the specified scope value is equal to
    // the current scope value
    if (lightScope == scope)
        return;

    // Interpret the scope constant
    switch (scope)
    {
        case VS_LIGHT_MODE_GLOBAL:
	    // For global light sources, turn the Performer light source
	    // node on
            if (lightOn)
                lightNode->on();
            break;
        case VS_LIGHT_MODE_LOCAL:
	    // For local light sources, keep the light source node off.
	    // A pfLight will be added to the locally-lit nodes' geostates
	    // instead.
            lightNode->off();
            break;
        default:
            printf("vsLightAttribute::setScope: Unrecognized scope constant\n");
            return;
    }
    
    // Save the scope value
    lightScope = scope;

    // Dirty the node that this attribute is attached to, if any
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
    // Set this light as on
    lightOn = true;

    // Global lights are simply turned off; local lights must be
    // activated by a pass of the VESS traverser
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
        lightNode->on();
    else if (parentNode)
        parentNode->dirty();
}

// ------------------------------------------------------------------------
// Deactivates this light source
// ------------------------------------------------------------------------
void vsLightAttribute::off()
{
    // Set this light as off
    lightOn = false;

    // Global lights are simply turned off; local lights must be
    // deactivated by a pass of the VESS traverser
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
        lightNode->off();
    else if (parentNode)
        parentNode->dirty();
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
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
bool vsLightAttribute::canAttach()
{
    // This attribute is not available to be attached if it is already
    // attached to another node
    if (attachedFlag)
        return false;

    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLightAttribute::attach(vsNode *theNode)
{
    // Verify that we're not already attached to something
    if (attachedFlag)
    {
        printf("vsLightAttribute::attach: Attribute is already attached\n");
        return;
    }

    // Light attributes may not be attached to geometry nodes
    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsLightAttribute::attach: Can't attach light attributes to "
            "geometry nodes\n");
        return;
    }

    // If the node is a scene, then use its base library object as
    // a light hook.
    if (theNode->getNodeType() == VS_NODE_TYPE_SCENE)
    {
        lightHookGroup = (pfGroup *)
            ((vsScene *)theNode)->getBaseLibraryObject();
    }
    // Else it is a Component, so use its Light Hook.
    else
    {
        lightHookGroup = ((vsComponent *)theNode)->getLightHook();
    }

    lightHookGroup->addChild(lightNode);

    // Mark this attribute as attached
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
    // Can't detach an attribute that is not attached
    if (!attachedFlag)
    {
        printf("vsLightAttribute::detach: Attribute is not attached\n");
        return;
    }

    // Remove our Performer light source from the Performer group,
    // and clear the group pointer
    lightHookGroup->removeChild(lightNode);
    lightHookGroup = NULL;

    // Mark this attribute as unattached
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
    
    // Create a duplicate light attribute
    newAttrib = new vsLightAttribute();
    
    // Copy all of the light parameters and modes
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

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// Internal function
// Turns this light on if it is a local light source
// ------------------------------------------------------------------------
void vsLightAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // If this is a local light source, add it to the current light state
    if ((lightScope == VS_LIGHT_MODE_LOCAL) && lightOn)
        gState->addLight(this);
}

// ------------------------------------------------------------------------
// Internal function
// Turns this light off if it is a local light source
// ------------------------------------------------------------------------
void vsLightAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // If this is a local light source, remove it from the current light
    // state
    if ((lightScope == VS_LIGHT_MODE_LOCAL) && lightOn)
        gState->removeLight(this);
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsLightAttribute::setState(pfGeoState *state)
{
    int loop;
    pfLight **lightList;
    pfGStateFuncType preFunc, postFunc;
    void *data;

    // Get the light of active pfLights from the geostate
    state->getFuncs(&preFunc, &postFunc, &data);
    lightList = (pfLight **)data;

    // Add the Performer light object to the first available (empty)
    // position in the current geostate's light list
    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        if (lightList[loop] == NULL)
        {
            lightList[loop] = lightObject;
            return;
        }
}
