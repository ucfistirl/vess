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
//    VESS Module:  vsTextureAttribute.c++
//
//    Description:  Attribute that specifies which texture should be used
//                  to cover geometry
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osgDB/ReadFile>
#include "vsTextureAttribute.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the OSG texture objects for unit 0 and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute()
{
    // Default unit is 0, the first texture.
    textureUnit = 0;

    // Create and reference new OSG Texture2D and TexEnv objects
    osgTexture = new osg::Texture2D();
    osgTexture->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexGen = NULL;
    osgTexEnvCombine = NULL;

    // Initialize the TexGen remove flag to false.
    removeTexGen = false;

    // Start with no image data
    osgTexImage = NULL;

    //Initialize the osg::Texture2D
    osgTexture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTexture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setApplyMode(VS_TEXTURE_APPLY_DECAL);
    setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
    setMinFilter(VS_TEXTURE_MINFILTER_LINEAR);
}

// ------------------------------------------------------------------------
// Constructor - Creates the OSG texture objects for the specified unit
// and initializes default settings
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute(unsigned int unit)
{
    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureAttribute::vsTextureAttribute: Invalid texture unit, "
            "using default of 0\n");
        textureUnit = 0;
    }

    // Create and reference new OSG Texture2D and TexEnv objects
    osgTexture = new osg::Texture2D();
    osgTexture->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexGen = NULL;
    osgTexEnvCombine = NULL;

    // Initialize the TexGen remove flag to false.
    removeTexGen = false;

    // Start with no image data
    osgTexImage = NULL;

    //Initialize the osg::Texture2D
    osgTexture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTexture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
    setMinFilter(VS_TEXTURE_MINFILTER_LINEAR);

    // Initialize the apply mode to MODULATE, if this texture is on a
    // texture unit higher than zero.  This ensures that the textures are
    // blended together by default.
    if (unit > 0)
        setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    else
        setApplyMode(VS_TEXTURE_APPLY_DECAL);
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute(unsigned int unit,
    osg::Texture2D *texObject, osg::TexEnv *texEnvObject,
    osg::TexEnvCombine *texEnvCombineObject, osg::TexGen *texGenObject)
{
    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureAttribute::vsTextureAttribute: Invalid texture unit, "
            "using default of 0\n");
        textureUnit = 0;
    }

    // Save and reference the Texture2D and associated objects
    osgTexture = texObject;
    osgTexture->ref();
    osgTexEnv = texEnvObject;
    osgTexEnvCombine = texEnvCombineObject;
    osgTexGen = texGenObject;
    if (osgTexEnv)
        osgTexEnv->ref();
    if (osgTexEnvCombine)
        osgTexEnvCombine->ref();
    if (osgTexGen)
        osgTexGen->ref();
    osgTexImage = osgTexture->getImage();
    osgTexImage->ref();

    // Initialize the TexGen remove flag to false.
    removeTexGen = false;

    // Set the texture border color to black
    osgTexture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTextureAttribute::~vsTextureAttribute()
{
    // Unreference the Texture2D and TexEnv objects
    osgTexture->unref();
    if (osgTexEnv)
        osgTexEnv->unref();
    if (osgTexEnvCombine)
        osgTexEnvCombine->unref();
    if (osgTexGen)
        osgTexGen->unref();

    // Unreference the texture image data if it exists
    if (osgTexImage != NULL)
        osgTexImage->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTextureAttribute::getClassName()
{
    return "vsTextureAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of the attribute
// ------------------------------------------------------------------------
int vsTextureAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_TEXTURE;
}

// ------------------------------------------------------------------------
// Sets the image data that this texture will display
// ------------------------------------------------------------------------
void vsTextureAttribute::setImage(unsigned char *imageData, int xSize,
    int ySize, int dataFormat)
{
    int format;

    // Create and reference an osg::Image if none exists
    if (!osgTexImage)
    {
        osgTexImage = new osg::Image();
        osgTexImage->ref();
        osgTexture->setImage(osgTexImage);
    }

    // Translate the image format into an OSG-friendly value
    switch (dataFormat)
    {
        case VS_TEXTURE_DFORMAT_INTENSITY:
            format = GL_LUMINANCE;
            break;
        case VS_TEXTURE_DFORMAT_INTENSITY_ALPHA:
            format = GL_LUMINANCE_ALPHA;
            break;
        case VS_TEXTURE_DFORMAT_RGB:
            format = GL_RGB;
            break;
        case VS_TEXTURE_DFORMAT_RGBA:
            format = GL_RGBA;
            break;
        default:
            printf("vsTextureAttribute::setImage: Bad data format value");
            return;
    }

    // Pass the image data and settings to the osg::Image
    osgTexImage->setImage(xSize, ySize, 1, GL_RGBA, format,
        GL_UNSIGNED_BYTE, imageData, osg::Image::USE_MALLOC_FREE, 1);
}

// ------------------------------------------------------------------------
// Retrieves a pointer to the image data that this texture is set to
// display, as well as its size and format. NULL pointers may be passed in
// for undesired values.
// ------------------------------------------------------------------------
void vsTextureAttribute::getImage(unsigned char **imageData, int *xSize,
    int *ySize, int *dataFormat)
{
    int format;

    // If no image exists, return NULL image data, zero sizes, and -1 for 
    // format
    if (!osgTexImage)
    {
        // Only set values for pointers that are valid
        if (imageData)
            *imageData = NULL;
        if (xSize)
            *xSize = 0;
        if (ySize)
            *ySize = 0;
        if (dataFormat)
            *dataFormat = -1;
        return;
    }

    // Translate the image format into a VESS value
    switch (osgTexImage->getPixelFormat())
    {
        case GL_LUMINANCE:
            format = VS_TEXTURE_DFORMAT_INTENSITY;
            break;
        case GL_LUMINANCE_ALPHA:
            format = VS_TEXTURE_DFORMAT_INTENSITY_ALPHA;
            break;
        case GL_RGB:
            format = VS_TEXTURE_DFORMAT_RGB;
            break;
        case GL_RGBA:
            format = VS_TEXTURE_DFORMAT_RGBA;
            break;
        default:
            format = -1;
            break;
    }

    // Fetch and return image data values for all parameters that have valid 
    // pointers
    if (imageData)
        *imageData = osgTexImage->data();
    if (xSize)
        *xSize = osgTexImage->s();
    if (ySize)
        *ySize = osgTexImage->t();
    if (dataFormat)
        *dataFormat = format;
}

// ------------------------------------------------------------------------
// Loads texture image data from the file with the indicated name
// ------------------------------------------------------------------------
void vsTextureAttribute::loadImageFromFile(char *filename)
{
    // Unreference the current texture image (if any)
    if (osgTexImage)
        osgTexImage->unref();

    // Read the image file into a new osg::Image object
    osgTexImage = osgDB::readImageFile(filename);

    // If successful, set the Texture2D to use the new image and 
    // referenc the image locally
    if (osgTexImage)
    {
        osgTexture->setImage(osgTexImage);
        osgTexImage->ref();
    }
    else
        printf("vsTextureAttribute::loadImageFromFile: Unable to load image\n");
}

// ------------------------------------------------------------------------
// Notifies the texture attribute that the texture data has been changed by
// some outside source, and forces it to retransfer the data to the
// graphics hardware.
// ------------------------------------------------------------------------
void vsTextureAttribute::reloadTextureData()
{
    // The pointer to the texture data is in the hands of the OSG Image
    // object. Tell the image object that it's data has changed, and tell
    // the texture object to get the new data from the image object.
    osgTexImage->dirty();
    osgTexture->dirtyTextureObject();
}

// ------------------------------------------------------------------------
// Sets the boundary mode for the one axis of the texture. The boundary
// mode affects how texture coordinates that are out of the standard
// 0.0-1.0 bounds are treated.
// ------------------------------------------------------------------------
void vsTextureAttribute::setBoundaryMode(int whichDirection, int boundaryMode)
{
    osg::Texture::WrapMode wrapType;

    // Translate the new boundary mode value into its OSG counterpart
    if (boundaryMode == VS_TEXTURE_BOUNDARY_REPEAT)
        wrapType = osg::Texture::REPEAT;
    else
        wrapType = osg::Texture::CLAMP;

    // Apply the new boundary mode to the given direction(s)
    switch (whichDirection)
    {
        case VS_TEXTURE_DIRECTION_S:
            osgTexture->setWrap(osg::Texture::WRAP_S, wrapType);
            break;

        case VS_TEXTURE_DIRECTION_T:
            osgTexture->setWrap(osg::Texture::WRAP_T, wrapType);
            break;

        case VS_TEXTURE_DIRECTION_ALL:
            osgTexture->setWrap(osg::Texture::WRAP_S, wrapType);
            osgTexture->setWrap(osg::Texture::WRAP_T, wrapType);
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the boundary mode for one axis of the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getBoundaryMode(int whichDirection)
{
    int wrapType;

    // Fetch the OSG WrapType for the given direction
    if (whichDirection == VS_TEXTURE_DIRECTION_T)
        wrapType = osgTexture->getWrap(osg::Texture::WRAP_T);
    else
        wrapType = osgTexture->getWrap(osg::Texture::WRAP_S);

    // Translate the WrapType into a VESS value
    if (wrapType == osg::Texture::REPEAT)
        return VS_TEXTURE_BOUNDARY_REPEAT;
    else
        return VS_TEXTURE_BOUNDARY_CLAMP;
}

// ------------------------------------------------------------------------
// Sets the application mode of the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setApplyMode(int applyMode)
{
    // See if we're using an osg::TexEnv or an osg::TexEnvCombine
    if (osgTexEnv)
    {
        // Translate the applyMode to an OSG value and set it on the TexEnv
        switch (applyMode)
        {
            case VS_TEXTURE_APPLY_DECAL:
                osgTexEnv->setMode(osg::TexEnv::DECAL);
                break;
            case VS_TEXTURE_APPLY_MODULATE:
                osgTexEnv->setMode(osg::TexEnv::MODULATE);
                break;
            case VS_TEXTURE_APPLY_REPLACE:
                osgTexEnv->setMode(osg::TexEnv::REPLACE);
                break;
            default:
                printf("vsTextureAttribute::setApplyMode: Bad apply mode "
                    "value\n");
                return;
        }
    }
    else
    {
        // Translate the applyMode to an OSG value and set it on the 
        // TexEnvCombine
        switch (applyMode)
        {
            case VS_TEXTURE_APPLY_DECAL:
                osgTexEnvCombine->
                    setCombine_RGB(osg::TexEnvCombine::INTERPOLATE);
                osgTexEnvCombine->
                    setCombine_Alpha(osg::TexEnvCombine::REPLACE);
                break;
            case VS_TEXTURE_APPLY_MODULATE:
                osgTexEnvCombine->setCombine_RGB(osg::TexEnvCombine::MODULATE);
                osgTexEnvCombine->
                    setCombine_Alpha(osg::TexEnvCombine::MODULATE);
                break;
            case VS_TEXTURE_APPLY_REPLACE:
                osgTexEnvCombine->setCombine_RGB(osg::TexEnvCombine::REPLACE);
                osgTexEnvCombine->
                    setCombine_Alpha(osg::TexEnvCombine::REPLACE);
                break;
            default:
                printf("vsTextureAttribute::setApplyMode: Bad apply mode "
                    "value\n");
                return;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves the application mode of the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getApplyMode()
{
    // See if we're using an osg::TexEnv or an osg::TexEnvCombine
    if (osgTexEnv)
    {
        // Fetch and translate the osg::TexEnv's apply mode.  Return the
        // translated value.
        switch (osgTexEnv->getMode())
        {
            case osg::TexEnv::DECAL:
                return VS_TEXTURE_APPLY_DECAL;
            case osg::TexEnv::MODULATE:
                return VS_TEXTURE_APPLY_MODULATE;
            case osg::TexEnv::REPLACE:
                return VS_TEXTURE_APPLY_REPLACE;
        }
    }
    else
    {
        // Fetch and translate the osg::TexEnvCombine's combine mode.  
        // Return the translated value.
        switch (osgTexEnvCombine->getCombine_RGB())
        {
            case osg::TexEnvCombine::INTERPOLATE:
                return VS_TEXTURE_APPLY_DECAL;
            case osg::TexEnvCombine::MODULATE:
                return VS_TEXTURE_APPLY_MODULATE;
            case osg::TexEnvCombine::REPLACE:
                return VS_TEXTURE_APPLY_REPLACE;
        }
    }

    // Return -1 if we don't recognize the TexEnv's mode
    return -1;
}

// ------------------------------------------------------------------------
// Sets the magnification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setMagFilter(int newFilter)
{
    // Translate the new filter mode to its OSG counterpart and apply it
    // to the Texture2D object
    switch (newFilter)
    {
        case VS_TEXTURE_MAGFILTER_NEAREST:
            osgTexture->setFilter(osg::Texture::MAG_FILTER,
                osg::Texture::NEAREST);
            break;
        case VS_TEXTURE_MAGFILTER_LINEAR:
            osgTexture->setFilter(osg::Texture::MAG_FILTER,
                osg::Texture::LINEAR);
            break;
        default:
            printf("vsTextureAttribute::setMagFilter: Bad filter value\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the magnification filter used by the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getMagFilter()
{
    // Translate the current MagFilter mode on the osg::Texture2D into
    // a VESS value and return it
    switch (osgTexture->getFilter(osg::Texture::MAG_FILTER))
    {
        case osg::Texture::NEAREST:
            return VS_TEXTURE_MAGFILTER_NEAREST;
        case osg::Texture::LINEAR:
            return VS_TEXTURE_MAGFILTER_LINEAR;
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the minification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setMinFilter(int newFilter)
{
    // Translate the new filter mode to its OSG counterpart and apply it
    // to the Texture2D object
    switch (newFilter)
    {
        case VS_TEXTURE_MINFILTER_NEAREST:
            osgTexture->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::NEAREST);
            break;
        case VS_TEXTURE_MINFILTER_LINEAR:
            osgTexture->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::LINEAR);
            break;
        case VS_TEXTURE_MINFILTER_MIPMAP_NEAREST:
            osgTexture->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::NEAREST_MIPMAP_NEAREST);
            break;
        case VS_TEXTURE_MINFILTER_MIPMAP_LINEAR:
            osgTexture->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::LINEAR_MIPMAP_LINEAR);
            break;
        default:
            printf("vsTextureAttribute::setMinFilter: Bad filter value\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the minification filter used by the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getMinFilter()
{
    // Translate the current MinFilter mode on the osg::Texture2D into
    // a VESS value and return it
    switch (osgTexture->getFilter(osg::Texture::MIN_FILTER))
    {
        case osg::Texture::NEAREST:
            return VS_TEXTURE_MINFILTER_NEAREST;
        case osg::Texture::LINEAR:
            return VS_TEXTURE_MINFILTER_LINEAR;
        case osg::Texture::NEAREST_MIPMAP_NEAREST:
            return VS_TEXTURE_MINFILTER_MIPMAP_NEAREST;
        case osg::Texture::LINEAR_MIPMAP_LINEAR:
            return VS_TEXTURE_MINFILTER_MIPMAP_LINEAR;
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setGenMode(int genMode)
{
    bool update;

    update = false;

    // Translate the genMode to an OSG value and set it on the TexGen
    switch (genMode)
    {
        case VS_TEXTURE_GEN_OBJECT_LINEAR:
            if (osgTexGen == NULL)
            {
                osgTexGen = new osg::TexGen();
                osgTexGen->ref();
                update = true;
            }
            osgTexGen->setMode(osg::TexGen::OBJECT_LINEAR);
            break;
        case VS_TEXTURE_GEN_EYE_LINEAR:
            if (osgTexGen == NULL)
            {
                osgTexGen = new osg::TexGen();
                osgTexGen->ref();
                update = true;
            }
            osgTexGen->setMode(osg::TexGen::EYE_LINEAR);
            break;
        case VS_TEXTURE_GEN_SPHERE_MAP:
            if (osgTexGen == NULL)
            {
                osgTexGen = new osg::TexGen();
                osgTexGen->ref();
                update = true;
            }
            osgTexGen->setMode(osg::TexGen::SPHERE_MAP);
            break;
        case VS_TEXTURE_GEN_NORMAL_MAP:
            if (osgTexGen == NULL)
            {
                osgTexGen = new osg::TexGen();
                osgTexGen->ref();
                update = true;
            }
            osgTexGen->setMode(osg::TexGen::NORMAL_MAP);
            break;
        case VS_TEXTURE_GEN_REFLECTION_MAP:
            if (osgTexGen == NULL)
            {
                osgTexGen = new osg::TexGen();
                osgTexGen->ref();
                update = true;
            }
            osgTexGen->setMode(osg::TexGen::REFLECTION_MAP);
            break;
        case VS_TEXTURE_GEN_OFF:
            if (osgTexGen)
            {
                removeTexGen = true;
                update = true;
            }
            break;
        default:
            printf("vsTextureAttribute::setGenMode: Bad generation mode "
                "value\n");
            return;
    }

    // If we need to update the Attr modes due to the texgen, do so.
    if (update)
    {
        markOwnersDirty();
        setAllOwnersOSGAttrModes();
    }
}

// ------------------------------------------------------------------------
// Retrieves the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getGenMode()
{
    if (osgTexGen)
    {
        // Translate the current texture coordinate generation mode on the
        // osg::TexGen into a VESS value and return it
        switch (osgTexGen->getMode())
        {
            case osg::TexGen::OBJECT_LINEAR:
                return VS_TEXTURE_GEN_OBJECT_LINEAR;
            case osg::TexGen::EYE_LINEAR:
                return VS_TEXTURE_GEN_EYE_LINEAR;
            case osg::TexGen::SPHERE_MAP:
                return VS_TEXTURE_GEN_SPHERE_MAP;
            case osg::TexGen::NORMAL_MAP:
                return VS_TEXTURE_GEN_NORMAL_MAP;
            case osg::TexGen::REFLECTION_MAP:
                return VS_TEXTURE_GEN_REFLECTION_MAP;
            default:
                return VS_TEXTURE_GEN_OFF;
        }
    }
    else
        return VS_TEXTURE_GEN_OFF;
}

// ------------------------------------------------------------------------
// Return the texture unit for this texture attribute
// ------------------------------------------------------------------------
unsigned int vsTextureAttribute::getTextureUnit()
{
    return textureUnit;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsTextureAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsTextureAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet for the given node
    osgStateSet = getOSGStateSet(node);

    // Set the Texture and related attributes and the StateAttribute mode
    // on the node's StateSet
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexture, attrMode);
    if (osgTexEnv)
        osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexEnv, 
            attrMode);
    if (osgTexEnvCombine)
        osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexEnvCombine,
            attrMode);   
    if (osgTexGen)
    {
        // See if the removeTexGen flag has been set
        if (removeTexGen)
        {
            // If the removeTexGen flag is set, reset the osgTexGen mode to 
            // inherit to stop using the texture coordinate generator
            osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexGen,
                osg::StateAttribute::INHERIT);
            osgTexGen->unref();
            osgTexGen = NULL;
            removeTexGen = false;
        }
        else
        {
            // Otherwise, just set the mode on the StateSet as usual
            osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexGen,
                attrMode);   
        }
    }
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTextureAttribute::attach(vsNode *node)
{
    // Do standard vsStateAttribute attaching
    vsStateAttribute::attach(node);

    // Handle the texture attributes for this node
    setOSGAttrModes(node);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being removed from the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTextureAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the osg::StateSet for this node
    osgStateSet = getOSGStateSet(node);

    // Reset the Texture and TexEnv states to INHERIT
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexture,
        osg::StateAttribute::INHERIT);
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexEnv,
        osg::StateAttribute::INHERIT);
    if (osgTexGen)
        osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexGen,
            osg::StateAttribute::INHERIT);
    if (osgTexEnvCombine)
        osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexEnvCombine,
            osg::StateAttribute::INHERIT);

    // Finish with standard StateAttribute detaching
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTextureAttribute::attachDuplicate(vsNode *theNode)
{
    // Do NOT duplicate the texture attribute; just point to the one we
    // have already. We don't want multiple texture objects with
    // repetitive data floating around the scene graph.
    theNode->addAttribute(this);
}

// ------------------------------------------------------------------------
// Internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
bool vsTextureAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTextureAttribute *attr;
    unsigned char *image1, *image2;
    int xval1, yval1, xval2, yval2, val1, val2;
    
    // Make sure the given attribute is valid
    if (!attribute)
        return false;

    // Check to see if we're comparing this attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a texture attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TEXTURE)
        return false;

    // Cast the given attribute to a texture attribute
    attr = (vsTextureAttribute *)attribute;

    // Compare image data and settings (note that both attributes must
    // point to the _same_ image data for them to be considered equivalent.
    getImage(&image1, &xval1, &yval1, &val1);
    attr->getImage(&image2, &xval2, &yval2, &val2);
    if ((image1 != image2) || (xval1 != xval2) || (yval1 != val2) ||
        (val1 != val2))
        return false;

    // Compare S boundary modes
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    if (val1 != val2)
        return false;

    // Compare T boundary modes
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    if (val1 != val2)
        return false;

    // Compare apply modes
    val1 = getApplyMode();
    val2 = attr->getApplyMode();
    if (val1 != val2)
        return false;

    // Compare texture coordinate generation modes
    val1 = getGenMode();
    val2 = attr->getGenMode();
    if (val1 != val2)
        return false;

    // Compare magnification filter modes
    val1 = getMagFilter();
    val2 = attr->getMagFilter();
    if (val1 != val2)
        return false;

    // Compare minification filter modes
    val1 = getMinFilter();
    val2 = attr->getMinFilter();
    if (val1 != val2)
        return false;

    // Compare texture unit
    val1 = getTextureUnit();
    val2 = attr->getTextureUnit();
    if (val1 != val2)
        return false;

    // If all pass, the attribute is equivalent
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Directly sets the osg::Image object to be used by this texture
// attribute. Deletes the current Image object, if any. (Deleting the
// image object will delete its image data as well; this should probably
// be changed as some point, as it's not always desirable to do that.)
// ------------------------------------------------------------------------
void vsTextureAttribute::setOSGImage(osg::Image *osgImage)
{
    // Release the existing image
    if (osgTexImage)
        osgTexImage->unref();

    // Store and reference the new image
    osgTexImage = osgImage;
    osgTexImage->ref();

    // Instruct the OSG texture object to use the new image
    osgTexture->setImage(osgTexImage);
}

// ------------------------------------------------------------------------
// Return the base OSG object used to represent this texture.
// ------------------------------------------------------------------------
osg::Texture2D *vsTextureAttribute::getBaseLibraryObject()
{
    return osgTexture;
}
