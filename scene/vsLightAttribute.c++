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

#include "vsSystem.h++"
#include "vsComponent.h++"

// ------------------------------------------------------------------------
// Constructor - Creates the corresponding Performer objects and
// initializes the light settings
// ------------------------------------------------------------------------
vsLightAttribute::vsLightAttribute()
{
    lightHookGroup = NULL;
    lightNode = new pfLightSource();
    lightNode->ref();
    lightObject = new pfLight();
    lightObject->ref();
    
    setAmbientColor(0.0, 0.0, 0.0);
    setDiffuseColor(0.0, 0.0, 0.0);
    setSpecularColor(0.0, 0.0, 0.0);
    
    lightOn = VS_FALSE;
    
    lightNode->off();
    
    lightScope = VS_LIGHT_MODE_GLOBAL;
    
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
    
    lightNode->getColor(PFLT_AMBIENT, &red, &green, &blue);
    
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
    
    lightNode->getColor(PFLT_DIFFUSE, &red, &green, &blue);
    
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
    
    lightNode->getColor(PFLT_SPECULAR, &red, &green, &blue);
    
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
    lightNode->setAtten(quadratic, linear, constant);
    lightObject->setAtten(quadratic, linear, constant);
}

// ------------------------------------------------------------------------
// Retrieves the constants for the light attenuation equation for this
// light. NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsLightAttribute::getAttenuationVals(double *quadratic, double *linear,
                                          double *constant)
{
    float aVal, bVal, cVal;
    
    lightNode->getAtten(&aVal, &bVal, &cVal);
    
    if (quadratic)
        *quadratic = aVal;
    if (linear)
        *linear = bVal;
    if (constant)
        *constant = cVal;
}

// ------------------------------------------------------------------------
// Sets the position of this light source. The fourth value, w, is a
// homogeneous coordinate scale; passing in 0 for w results in a light
// source that is infinitely far away from the viewer.
// ------------------------------------------------------------------------
void vsLightAttribute::setPosition(double x, double y, double z, double w)
{
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
    
    lightNode->getPos(&xPos, &yPos, &zPos, &wHomogeneous);
    
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
    
    lightNode->getSpotDir(&xDir, &yDir, &zDir);
    
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
    
    lightNode->getSpotCone(&expVal, &cutDeg);
    
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
    if (lightScope == scope)
        return;

    switch (scope)
    {
        case VS_LIGHT_MODE_GLOBAL:
            if (lightOn)
                lightNode->on();
            break;
        case VS_LIGHT_MODE_LOCAL:
            lightNode->off();
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
    lightOn = VS_FALSE;
    if (lightScope == VS_LIGHT_MODE_GLOBAL)
        lightNode->off();
    else if (parentNode)
        parentNode->dirty();
}

// ------------------------------------------------------------------------
// Returns a flag indicating if this light source is currently active
// ------------------------------------------------------------------------
int vsLightAttribute::isOn()
{
    return lightOn;
}

// ------------------------------------------------------------------------
// VESS internal function
// Returns if this attribute is available to be attached to a node
// ------------------------------------------------------------------------
int vsLightAttribute::canAttach()
{
    if (attachedFlag)
        return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// VESS internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsLightAttribute::attach(vsNode *theNode)
{
    if (attachedFlag)
    {
        printf("vsLightAttribute::attach: Attribute is already attached\n");
        return;
    }

    if ((theNode->getNodeType() == VS_NODE_TYPE_GEOMETRY) ||
        (theNode->getNodeType() == VS_NODE_TYPE_DYNAMIC_GEOMETRY))
    {
        printf("vsLightAttribute::attach: Can't attach light attributes to "
            "geometry nodes\n");
        return;
    }

    lightHookGroup = ((vsComponent *)theNode)->getLightHook();
    lightHookGroup->addChild(lightNode);

    attachedFlag = 1;
    parentNode = theNode;
}

// ------------------------------------------------------------------------
// VESS internal function
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

    lightHookGroup->removeChild(lightNode);
    lightHookGroup = NULL;

    attachedFlag = 0;
    parentNode = NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
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
// VESS internal function
// Turns this light on if it is a local light source
// ------------------------------------------------------------------------
void vsLightAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    if ((lightScope == VS_LIGHT_MODE_LOCAL) && lightOn)
        gState->addLight(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Turns this light off if it is a local light source
// ------------------------------------------------------------------------
void vsLightAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    if ((lightScope == VS_LIGHT_MODE_LOCAL) && lightOn)
        gState->removeLight(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsLightAttribute::setState(pfGeoState *state)
{
    int loop;
    pfLight **lightList;
    pfGStateFuncType preFunc, postFunc;
    void *data;

    state->getFuncs(&preFunc, &postFunc, &data);
    lightList = (pfLight **)data;

    for (loop = 0; loop < PF_MAX_LIGHTS; loop++)
        if (lightList[loop] == NULL)
        {
            lightList[loop] = lightObject;
            return;
        }
}
