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
//    VESS Module:  vsTextureCubeAttribute.c++
//
//    Description:  Attribute that specifies a texture cube to use for
//                  effects like evironment mapping on geometry
//
//    Author(s):    Duvan Cope, Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <osgDB/ReadFile>
#include "vsTextureCubeAttribute.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the OSG texture objects and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureCubeAttribute::vsTextureCubeAttribute()
{
    int loop;

    // Default unit is 0, the first texture.
    textureUnit = 0;

    // Create and reference new OSG Texture2D and TexEnv objects
    osgTextureCube = new osg::TextureCubeMap();
    osgTextureCube->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexGen = new osg::TexGen();
    osgTexGen->ref();

    // Start with no image data
    for (loop = 0; loop < VS_TEXTURE_CUBE_SIDES; loop++)
    {
        osgTexImage[loop] = NULL;
    }

    //Initialize the osg::TextureCubeMap
    osgTextureCube->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTextureCube->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setApplyMode(VS_TEXTURE_APPLY_DECAL);
    setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
    setMinFilter(VS_TEXTURE_MINFILTER_LINEAR);
    setGenMode(VS_TEXTURE_GEN_REFLECTION_MAP);
}

// ------------------------------------------------------------------------
// Constructor - Creates the OSG texture cube objects and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureCubeAttribute::vsTextureCubeAttribute(unsigned int unit)
{
    int loop;

    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureCubeAttribute::vsTextureCubeAttribute: Invalid "
            "texture unit, using default of 0\n");
        textureUnit = 0;
    }

    // Create and reference new OSG Texture2D and TexEnv objects
    osgTextureCube = new osg::TextureCubeMap();
    osgTextureCube->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexGen = new osg::TexGen();
    osgTexGen->ref();

    // Start with no image data
    for (loop = 0; loop < VS_TEXTURE_CUBE_SIDES; loop++)
    {
        osgTexImage[loop] = NULL;
    }

    //Initialize the osg::TextureCubeMap
    osgTextureCube->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTextureCube->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setApplyMode(VS_TEXTURE_APPLY_DECAL);
    setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
    setMinFilter(VS_TEXTURE_MINFILTER_LINEAR);
    setGenMode(VS_TEXTURE_GEN_REFLECTION_MAP);
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureCubeAttribute::vsTextureCubeAttribute(unsigned int unit,
    osg::TextureCubeMap *texObject, osg::TexEnv *texEnvObject,
    osg::TexGen *texGenObject)
{
    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureCubeAttribute::vsTextureCubeAttribute: Invalid "
            "texture unit, using default of 0\n");
        textureUnit = 0;
    }

    // Save and reference the TextureCubeMap, TexEnv, and TexGen objects
    osgTextureCube = texObject;
    osgTextureCube->ref();
    osgTexEnv = texEnvObject;
    osgTexEnv->ref();
    osgTexGen = texGenObject;
    osgTexGen->ref();

    // Reference the texture image data
    osgTexImage[0] = osgTextureCube->getImage((osg::TextureCubeMap::Face) 0);
    osgTexImage[0]->ref();
    osgTexImage[1] = osgTextureCube->getImage((osg::TextureCubeMap::Face) 1);
    osgTexImage[1]->ref();
    osgTexImage[2] = osgTextureCube->getImage((osg::TextureCubeMap::Face) 2);
    osgTexImage[2]->ref();
    osgTexImage[3] = osgTextureCube->getImage((osg::TextureCubeMap::Face) 3);
    osgTexImage[3]->ref();
    osgTexImage[4] = osgTextureCube->getImage((osg::TextureCubeMap::Face) 4);
    osgTexImage[4]->ref();
    osgTexImage[5] = osgTextureCube->getImage((osg::TextureCubeMap::Face) 5);
    osgTexImage[5]->ref();

    // Set the texture border color to black
    osgTextureCube->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTextureCubeAttribute::~vsTextureCubeAttribute()
{
    // Unreference the Texture2D and TexEnv objects
    osgTextureCube->unref();
    osgTexEnv->unref();
    osgTexGen->unref();

    // Unreference the texture image data if it exists
    if (osgTexImage[0] != NULL) osgTexImage[0]->unref();
    if (osgTexImage[1] != NULL) osgTexImage[1]->unref();
    if (osgTexImage[2] != NULL) osgTexImage[2]->unref();
    if (osgTexImage[3] != NULL) osgTexImage[3]->unref();
    if (osgTexImage[4] != NULL) osgTexImage[4]->unref();
    if (osgTexImage[5] != NULL) osgTexImage[5]->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTextureCubeAttribute::getClassName()
{
    return "vsTextureCubeAttribute";
}

// ------------------------------------------------------------------------
// Retrieves the type of the attribute
// ------------------------------------------------------------------------
int vsTextureCubeAttribute::getAttributeType()
{
    return VS_ATTRIBUTE_TYPE_TEXTURE_CUBE;
}

// ------------------------------------------------------------------------
// Sets the image data that this texture will display
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::setImage(int face, unsigned char *imageData,
    int xSize, int ySize, int dataFormat)
{
    int format;

    // Insure the face is a valid index
    if ((face < 0) || (face > (VS_TEXTURE_CUBE_SIDES - 1)))
    {
        printf("vsTextureCubeAttribute::setImage: Index out of bounds\n");
        return;
    }

    // Create and reference an osg::Image if none exists
    if (!osgTexImage[face])
    {
        osgTexImage[face] = new osg::Image();
        osgTexImage[face]->ref();
        osgTextureCube->setImage((osg::TextureCubeMap::Face) face,
            osgTexImage[face]);
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
            printf("vsTextureCubeAttribute::setImage: Bad data format value");
            return;
    }

    // Pass the image data and settings to the osg::Image
    osgTexImage[face]->setImage(xSize, ySize, 1, GL_RGBA, format,
        GL_UNSIGNED_BYTE, imageData, osg::Image::USE_MALLOC_FREE, 1);
}

// ------------------------------------------------------------------------
// Retrieves a pointer to the image data that this texture is set to
// display, as well as its size and format. NULL pointers may be passed in
// for undesired values.
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::getImage(int face, unsigned char **imageData,
    int *xSize, int *ySize, int *dataFormat)
{
    int format;

    // Insure the face is a valid index
    if ((face < 0) || (face > (VS_TEXTURE_CUBE_SIDES - 1)))
    {
        printf("vsTextureCubeAttribute::getImage: Index out of bounds\n");
        return;
    }

    // If no image exists, return NULL image data, zero sizes, and -1 for 
    // format
    if (!osgTexImage[face])
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
    switch (osgTexImage[face]->getPixelFormat())
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
        *imageData = osgTexImage[face]->data();
    if (xSize)
        *xSize = osgTexImage[face]->s();
    if (ySize)
        *ySize = osgTexImage[face]->t();
    if (dataFormat)
        *dataFormat = format;
}

// ------------------------------------------------------------------------
// Loads texture image data from the file with the indicated name
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::loadImageFromFile(int face, char *filename)
{
    // Insure the face is a valid index
    if ((face < 0) || (face > (VS_TEXTURE_CUBE_SIDES - 1)))
    {
        printf("vsTextureCubeAttribute::loadImageFromFile: Index out of "
            "bounds\n");
        return;
    }

    // Unreference the current texture image (if any)
    if (osgTexImage[face])
        osgTexImage[face]->unref();

    // Read the image file into a new osg::Image object
    osgTexImage[face] = osgDB::readImageFile(filename);

    // If successful, set the Texture2D to use the new image and 
    // referenc the image locally
    if (osgTexImage[face])
    {
        osgTextureCube->setImage((osg::TextureCubeMap::Face) face,
            osgTexImage[face]);
        osgTexImage[face]->ref();
    }
    else
        printf("vsTextureCubeAttribute::loadImageFromFile: Unable to load image\n");
}

// ------------------------------------------------------------------------
// Notifies the texture attribute that the texture data has been changed by
// some outside source, and forces it to retransfer the data to the
// graphics hardware.
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::reloadTextureData(int face)
{
    // The pointer to the texture data is in the hands of the OSG Image
    // object. Tell the image object that it's data has changed, and tell
    // the texture object to get the new data from the image object.
    osgTexImage[face]->dirty();
    osgTextureCube->dirtyTextureObject();
}

// ------------------------------------------------------------------------
// Sets the boundary mode for the one axis of the texture. The boundary
// mode affects how texture coordinates that are out of the standard
// 0.0-1.0 bounds are treated.
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::setBoundaryMode(int whichDirection, int boundaryMode)
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
            osgTextureCube->setWrap(osg::Texture::WRAP_S, wrapType);
            break;

        case VS_TEXTURE_DIRECTION_T:
            osgTextureCube->setWrap(osg::Texture::WRAP_T, wrapType);
            break;

        case VS_TEXTURE_DIRECTION_ALL:
            osgTextureCube->setWrap(osg::Texture::WRAP_S, wrapType);
            osgTextureCube->setWrap(osg::Texture::WRAP_T, wrapType);
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the boundary mode for one axis of the texture
// ------------------------------------------------------------------------
int vsTextureCubeAttribute::getBoundaryMode(int whichDirection)
{
    int wrapType;

    // Fetch the OSG WrapType for the given direction
    if (whichDirection == VS_TEXTURE_DIRECTION_T)
        wrapType = osgTextureCube->getWrap(osg::Texture::WRAP_T);
    else
        wrapType = osgTextureCube->getWrap(osg::Texture::WRAP_S);

    // Translate the WrapType into a VESS value
    if (wrapType == osg::Texture::REPEAT)
        return VS_TEXTURE_BOUNDARY_REPEAT;
    else
        return VS_TEXTURE_BOUNDARY_CLAMP;
}

// ------------------------------------------------------------------------
// Sets the application mode of the texture
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::setApplyMode(int applyMode)
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
            printf("vsTextureCubeAttribute::setApplyMode: Bad apply mode "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the application mode of the texture
// ------------------------------------------------------------------------
int vsTextureCubeAttribute::getApplyMode()
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
void vsTextureCubeAttribute::setMagFilter(int newFilter)
{
    // Translate the new filter mode to its OSG counterpart and apply it
    // to the TextureCubeMap object
    switch (newFilter)
    {
        case VS_TEXTURE_MAGFILTER_NEAREST:
            osgTextureCube->setFilter(osg::Texture::MAG_FILTER,
                osg::Texture::NEAREST);
            break;
        case VS_TEXTURE_MAGFILTER_LINEAR:
            osgTextureCube->setFilter(osg::Texture::MAG_FILTER,
                osg::Texture::LINEAR);
            break;
        default:
            printf("vsTextureCubeAttribute::setMagFilter: Bad filter value\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the magnification filter used by the texture
// ------------------------------------------------------------------------
int vsTextureCubeAttribute::getMagFilter()
{
    // Translate the current MagFilter mode on the osg::TextureCubeMap into
    // a VESS value and return it
    switch (osgTextureCube->getFilter(osg::Texture::MAG_FILTER))
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
void vsTextureCubeAttribute::setMinFilter(int newFilter)
{
    // Translate the new filter mode to its OSG counterpart and apply it
    // to the TextureCubeMap object
    switch (newFilter)
    {
        case VS_TEXTURE_MINFILTER_NEAREST:
            osgTextureCube->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::NEAREST);
            break;
        case VS_TEXTURE_MINFILTER_LINEAR:
            osgTextureCube->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::LINEAR);
            break;
        case VS_TEXTURE_MINFILTER_MIPMAP_NEAREST:
            osgTextureCube->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::NEAREST_MIPMAP_NEAREST);
            break;
        case VS_TEXTURE_MINFILTER_MIPMAP_LINEAR:
            osgTextureCube->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::LINEAR_MIPMAP_LINEAR);
            break;
        default:
            printf("vsTextureCubeAttribute::setMinFilter: Bad filter value\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the minification filter used by the texture
// ------------------------------------------------------------------------
int vsTextureCubeAttribute::getMinFilter()
{
    // Translate the current MinFilter mode on the osg::Texture into
    // a VESS value and return it
    switch (osgTextureCube->getFilter(osg::Texture::MIN_FILTER))
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
void vsTextureCubeAttribute::setGenMode(int genMode)
{
    // Translate the genMode to an OSG value and set it on the TexGen
    switch (genMode)
    {
        case VS_TEXTURE_GEN_OBJECT_LINEAR:
            osgTexGen->setMode(osg::TexGen::OBJECT_LINEAR);
            break;
        case VS_TEXTURE_GEN_EYE_LINEAR:
            osgTexGen->setMode(osg::TexGen::EYE_LINEAR);
            break;
        case VS_TEXTURE_GEN_SPHERE_MAP:
            osgTexGen->setMode(osg::TexGen::SPHERE_MAP);
            break;
        case VS_TEXTURE_GEN_NORMAL_MAP:
            osgTexGen->setMode(osg::TexGen::NORMAL_MAP);
            break;
        case VS_TEXTURE_GEN_REFLECTION_MAP:
            osgTexGen->setMode(osg::TexGen::REFLECTION_MAP);
            break;
        default:
            printf("vsTextureCubeAttribute::setGenMode: Bad generation mode "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
int vsTextureCubeAttribute::getGenMode()
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

// ------------------------------------------------------------------------
// Return the texture unit used in the texture attribute
// ------------------------------------------------------------------------
unsigned int vsTextureCubeAttribute::getTextureUnit()
{
    return textureUnit;
}

// ------------------------------------------------------------------------
// Private function
// Sets the modes on the StateSet of this node's OSG node to reflect the
// settings of this attribute
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::setOSGAttrModes(vsNode *node)
{
    unsigned int attrMode;
    osg::StateSet *osgStateSet;
    
    // Start with the osg::StateAttribute mode set to ON
    attrMode = osg::StateAttribute::ON;

    // If the vsTextureCubeAttribute's override flag is set, change the
    // osg::StateAttribute's mode to OVERRIDE
    if (overrideFlag)
        attrMode |= osg::StateAttribute::OVERRIDE;

    // Get the StateSet for the given node
    osgStateSet = getOSGStateSet(node);

    // Set the Texture and TexEnv attributes and the StateAttribute mode
    // on the node's StateSet
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTextureCube,
        attrMode);
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexEnv, attrMode);
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexGen, attrMode);
}

// ------------------------------------------------------------------------
// Internal function
// Notifies the attribute that it is being added to the given node's
// attribute list
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::attach(vsNode *node)
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
void vsTextureCubeAttribute::detach(vsNode *node)
{
    osg::StateSet *osgStateSet;

    // Get the osg::StateSet for this node
    osgStateSet = getOSGStateSet(node);

    // Reset the Texture and TexEnv states to INHERIT
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTextureCube,
        osg::StateAttribute::INHERIT);
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexEnv,
        osg::StateAttribute::INHERIT);
    osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexGen,
        osg::StateAttribute::INHERIT);

    // Finish with standard StateAttribute detaching
    vsStateAttribute::detach(node);
}

// ------------------------------------------------------------------------
// Internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTextureCubeAttribute::attachDuplicate(vsNode *theNode)
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
bool vsTextureCubeAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTextureCubeAttribute *attr;
    unsigned char *image1, *image2;
    int xval1, yval1, xval2, yval2, val1, val2, loop;
    
    // Make sure the given attribute is valid
    if (!attribute)
        return false;

    // Check to see if we're comparing this attribute to itself
    if (this == attribute)
        return true;
    
    // Make sure the given attribute is a texture attribute
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
        return false;

    // Cast the given attribute to a texture attribute
    attr = (vsTextureCubeAttribute *)attribute;

    // Compare image data and settings (note that both attributes must
    // point to the _same_ image data for them to be considered equivalent.
    for (loop = 0; loop < VS_TEXTURE_CUBE_SIDES; loop++)
    {
        getImage(loop, &image1, &xval1, &yval1, &val1);
        attr->getImage(loop, &image2, &xval2, &yval2, &val2);
        if ((image1 != image2) || (xval1 != xval2) || (yval1 != val2) ||
            (val1 != val2))
            return false;
    }

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
void vsTextureCubeAttribute::setOSGImage(int face, osg::Image *osgImage)
{
    // Insure the face is a valid index
    if ((face < 0) || (face > (VS_TEXTURE_CUBE_SIDES - 1)))
    {
        printf("vsTextureCubeAttribute::setOSGImage: Index out of bounds\n");
        return;
    }

    // Release the existing image
    if (osgTexImage[face])
        osgTexImage[face]->unref();

    // Store and reference the new image
    osgTexImage[face] = osgImage;
    osgTexImage[face]->ref();

    // Instruct the OSG texture object to use the new image
    osgTextureCube->setImage((osg::TextureCubeMap::Face) face,
        osgTexImage[face]);
}

// ------------------------------------------------------------------------
// Return the base OSG object used to represent this texture.
// ------------------------------------------------------------------------
osg::TextureCubeMap *vsTextureCubeAttribute::getBaseLibraryObject()
{
    return osgTextureCube;
}
