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
//    VESS Module:  vsTextureRectangleAttribute.h++
//
//    Description:  Attribute that specifies which texture should be used
//                  to cover geometry. Works on textures that don't have
//                  power-of-two dimensions.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osgDB/ReadFile>
#include "vsTextureRectangleAttribute.h++"
#include "vsNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the Performer texture objects and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute()
{
    // Create and reference new OSG TextureRectangle and TexEnv objects
    osgTexture = new osg::TextureRectangle();
    osgTexture->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexGen = NULL;

    // Initialize the TexGen remove flag to false.
    removeTexGen = false;

    // Start with no image data
    osgTexImage = NULL;

    //Initialize the osg::TextureRectangle
    osgTexture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTexture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setApplyMode(VS_TEXTURE_APPLY_DECAL);
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute(
    osg::TextureRectangle *texObject, osg::TexEnv *texEnvObject,
    osg::TexGen *texGenObject)
{
    // Save and reference the TextureRectangle and TexEnv objects
    osgTexture = texObject;
    osgTexture->ref();
    osgTexEnv = texEnvObject;
    osgTexEnv->ref();
    osgTexGen = texGenObject;
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
vsTextureRectangleAttribute::~vsTextureRectangleAttribute()
{
    // Unreference the TextureRectangle and TexEnv objects
    osgTexture->unref();
    osgTexEnv->unref();
    if (osgTexGen)
        osgTexGen->unref();

    // Unreference the texture image data if it exists
    if (osgTexImage != NULL)
        osgTexImage->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTextureRectangleAttribute::getClassName()
{
    return "vsTextureRectangleAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of the attribute
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE;
}

// ------------------------------------------------------------------------
// Sets the image data that this texture will display
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setImage(unsigned char *imageData,
    int xSize, int ySize, int dataFormat)
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
            printf("vsTextureRectangleAttribute::setImage: "
                "Bad data format value");
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
void vsTextureRectangleAttribute::getImage(unsigned char **imageData,
    int *xSize, int *ySize, int *dataFormat)
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
void vsTextureRectangleAttribute::loadImageFromFile(char *filename)
{
    // Unreference the current texture image (if any)
    if (osgTexImage)
        osgTexImage->unref();

    // Read the image file into a new osg::Image object
    osgTexImage = osgDB::readImageFile(filename);

    // If successful, set the TextureRectangle to use the new image and 
    // reference the image locally
    if (osgTexImage)
    {
        osgTexture->setImage(osgTexImage);
        osgTexImage->ref();
    }
    else
        printf("vsTextureRectangleAttribute::loadImageFromFile: "
            "Unable to load image\n");
}

// ------------------------------------------------------------------------
// Notifies the texture attribute that the texture data has been changed by
// some outside source, and forces it to retransfer the data to the
// graphics hardware.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::reloadTextureData()
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
void vsTextureRectangleAttribute::setBoundaryMode(int whichDirection,
    int boundaryMode)
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
int vsTextureRectangleAttribute::getBoundaryMode(int whichDirection)
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
void vsTextureRectangleAttribute::setApplyMode(int applyMode)
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
            printf("vsTextureRectangleAttribute::setApplyMode: "
                "Bad apply mode value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the application mode of the texture
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getApplyMode()
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
// Sets the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setGenMode(int genMode)
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
            printf("vsTextureRectangleAttribute::setGenMode: "
                "Bad generation mode value\n");
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
int vsTextureRectangleAttribute::getGenMode()
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
        }
    }
    else
        return VS_TEXTURE_GEN_OFF;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsTextureRectangleAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet for the given node
    osgStateSet = getOSGStateSet(node);

    // Set the Texture and TexEnv attributes and the StateAttribute mode
    // on the node's StateSet
    osgStateSet->setTextureAttributeAndModes(0, osgTexture, attrMode);
    osgStateSet->setTextureAttributeAndModes(0, osgTexEnv, attrMode);
    if (osgTexGen)
    {
        if (removeTexGen)
        {
            osgStateSet->setTextureAttributeAndModes(0, osgTexGen,
                osg::StateAttribute::INHERIT);
            osgTexGen->unref();
            osgTexGen = NULL;
            removeTexGen = false;
        }
        else
            osgStateSet->setTextureAttributeAndModes(0, osgTexGen, attrMode);
    }
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::attach(vsNode *node)
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
void vsTextureRectangleAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the osg::StateSet for this node
    osgStateSet = getOSGStateSet(node);

    // Reset the Texture and TexEnv states to INHERIT
    osgStateSet->setTextureAttributeAndModes(0, osgTexture,
        osg::StateAttribute::INHERIT);
    osgStateSet->setTextureAttributeAndModes(0, osgTexEnv,
        osg::StateAttribute::INHERIT);
    if (osgTexGen)
        osgStateSet->setTextureAttributeAndModes(0, osgTexGen,
            osg::StateAttribute::INHERIT);

    // Finish with standard StateAttribute detaching
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::attachDuplicate(vsNode *theNode)
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
bool vsTextureRectangleAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTextureRectangleAttribute *attr;
    unsigned char *image1, *image2;
    int xval1, yval1, xval2, yval2, val1, val2;
    
    // Make sure the given attribute is valid
    if (!attribute)
        return false;

    // Check to see if we're comparing this attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a texture rectangle attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
        return false;

    // Cast the given attribute to a texture rectangle attribute
    attr = (vsTextureRectangleAttribute *)attribute;

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
void vsTextureRectangleAttribute::setOSGImage(osg::Image *osgImage)
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
osg::TextureRectangle *vsTextureRectangleAttribute::getBaseLibraryObject()
{
    return osgTexture;
}