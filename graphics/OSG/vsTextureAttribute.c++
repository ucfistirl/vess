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
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osgDB/ReadFile>
#include "vsTextureAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the Performer texture objects and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute()
{
    // Create and reference new OSG Texture2D and TexEnv objects
    osgTexture = new osg::Texture2D();
    osgTexture->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();

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
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute(osg::Texture2D *texObject,
    osg::TexEnv *texEnvObject)
{
    // Save and reference the Texture2D and TexEnv objects
    osgTexture = texObject;
    osgTexture->ref();
    osgTexEnv = texEnvObject;
    osgTexEnv->ref();
    osgTexImage = osgTexture->getImage();
    osgTexImage->ref();
    
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
    osgTexEnv->unref();

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
        GL_UNSIGNED_BYTE, imageData);
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
            printf("vsTextureAttribute::setApplyMode: Bad apply mode value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the application mode of the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getApplyMode()
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

    // Set the Texture and TexEnv attributes and the StateAttribute mode
    // on the node's StateSet
    osgStateSet->setTextureAttributeAndModes(0, osgTexture, attrMode);
    osgStateSet->setTextureAttributeAndModes(0, osgTexEnv, attrMode);
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
    osgStateSet->setTextureAttributeAndModes(0, osgTexture,
        osg::StateAttribute::INHERIT);
    osgStateSet->setTextureAttributeAndModes(0, osgTexEnv,
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
int vsTextureAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTextureAttribute *attr;
    unsigned char *image1, *image2;
    int xval1, yval1, xval2, yval2, val1, val2;
    
    // Make sure the given attribute is valid
    if (!attribute)
        return VS_FALSE;

    // Check to see if we're comparing this attribute to itself
    if (this == attribute)
        return VS_TRUE;
    
    // Make sure the given attribute is a texture attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TEXTURE)
        return VS_FALSE;

    // Cast the given attribute to a texture attribute
    attr = (vsTextureAttribute *)attribute;

    // Compare image data and settings (note that both attributes must
    // point to the _same_ image data for them to be considered equivalent.
    getImage(&image1, &xval1, &yval1, &val1);
    attr->getImage(&image2, &xval2, &yval2, &val2);
    if ((image1 != image2) || (xval1 != xval2) || (yval1 != val2) ||
        (val1 != val2))
        return VS_FALSE;

    // Compare S boundary modes
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    if (val1 != val2)
        return VS_FALSE;

    // Compare T boundary modes
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    if (val1 != val2)
        return VS_FALSE;

    // Compare apply modes
    val1 = getApplyMode();
    val2 = attr->getApplyMode();
    if (val1 != val2)
        return VS_FALSE;

    // Compare magnification filter modes
    val1 = getMagFilter();
    val2 = attr->getMagFilter();
    if (val1 != val2)
        return VS_FALSE;

    // Compare minification filter modes
    val1 = getMinFilter();
    val2 = attr->getMinFilter();
    if (val1 != val2)
        return VS_FALSE;

    // If all pass, the attribute is equivalent
    return VS_TRUE;
}
