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

#include "vsTextureRectangleAttribute.h++"
#include "vsNode.h++"
#include "vsGeometry.h++"
#include <osgDB/ReadFile>
#include <stdio.h>

// ------------------------------------------------------------------------
// Default Constructor - Creates the OSG texture objects for unit 0 and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute()
{
    // Default unit is 0, the first texture.
    textureUnit = 0;

    // Create and reference new OSG TextureRectangle and TexEnv objects
    osgTexture = new osg::TextureRectangle();
    osgTexture->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexEnvCombine = NULL;
    osgTexGen = NULL;
    osgTexMat = NULL;

    // Initialize the TexGen remove flag to false.
    removeTexGen = false;

    // Start with no image data
    osgTexImage = NULL;

    // Initialize the osg::TextureRectangle
    osgTexture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTexture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setApplyMode(VS_TEXTURE_APPLY_MODULATE);
}

// ------------------------------------------------------------------------
// Constructor - Creates the OSG texture objects for the specified unit
// and initializes default settings
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute(unsigned int unit)
{
    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureRectangleAttribute::vsTextureRectangleAttribute: "
            "Invalid texture unit, using default of 0\n");
        textureUnit = 0;
    }

    // Create and reference new OSG TextureRectangle and TexEnv objects
    osgTexture = new osg::TextureRectangle();
    osgTexture->ref();
    osgTexEnv = new osg::TexEnv();
    osgTexEnv->ref();
    osgTexEnvCombine = NULL;
    osgTexGen = NULL;
    osgTexMat = NULL;

    // Initialize the TexGen remove flag to false.
    removeTexGen = false;

    // Start with no image data
    osgTexImage = NULL;

    // Initialize the osg::TextureRectangle
    osgTexture->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    osgTexture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);

    // Initialize the texture attribute
    setBoundaryMode(VS_TEXTURE_DIRECTION_ALL, VS_TEXTURE_BOUNDARY_CLAMP);
    setApplyMode(VS_TEXTURE_APPLY_MODULATE);
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute(unsigned int unit,
    osg::TextureRectangle *texObject, osg::TexEnv *texEnvObject,
    osg::TexEnvCombine *texEnvCombineObject, osg::TexGen *texGenObject,
    osg::TexMat *texMatObject)
{
    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureRectangleAttribute::vsTextureRectangleAttribute: \n");
        printf("    Invalid texture unit, using default of 0\n");
        textureUnit = 0;
    }

    // Save and reference the TextureRectangle, TexEnv, and TexMat objects
    // as well as the texture image itself
    osgTexture = texObject;
    osgTexture->ref();
    osgTexEnv = texEnvObject;
    if (osgTexEnv)
        osgTexEnv->ref();
    osgTexEnvCombine = texEnvCombineObject;
    if (osgTexEnvCombine)
        osgTexEnvCombine->ref();
    osgTexGen = texGenObject;
    if (osgTexGen)
        osgTexGen->ref();
    osgTexMat = texMatObject;
    if (osgTexMat)
        osgTexMat->ref();
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
    // Unreference the TextureRectangle, TexEnv, and TexMat objects
    osgTexture->unref();
    if (osgTexEnv)
        osgTexEnv->unref();
    if (osgTexEnvCombine)
        osgTexEnvCombine->unref();
    if (osgTexGen)
        osgTexGen->unref();
    if (osgTexMat)
        osgTexMat->unref();

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
// Returns a clone of this attribute
// ------------------------------------------------------------------------
vsAttribute *vsTextureRectangleAttribute::clone()
{
    vsTextureRectangleAttribute *newAttrib;
    osg::TextureRectangle *newOSGTexture;
    osg::TexEnv *newOSGTexEnv;
    osg::TexEnvCombine *newOSGTexEnvCombine;
    osg::TexGen *newOSGTexGen;
    osg::TexMat *newOSGTexMat;

    // Share the osg::TextureRectangle object that we're using
    if (osgTexture)
        newOSGTexture = osgTexture;
    else
        newOSGTexture = NULL;

    // Create copies of the remaining texture-related objects
    // Texture enviroment
    if (osgTexEnv)
        newOSGTexEnv = new osg::TexEnv(*osgTexEnv);
    else
        newOSGTexEnv = NULL;

    // Texture enviroment combiner
    if (osgTexEnvCombine)
        newOSGTexEnvCombine = new osg::TexEnvCombine(*osgTexEnvCombine);
    else
        newOSGTexEnvCombine = NULL;

    // Texture coordinate generation
    if (osgTexGen)
        newOSGTexGen = new osg::TexGen(*osgTexGen);
    else
        newOSGTexGen = NULL;

    // Texture matrix
    if (osgTexMat)
        newOSGTexMat = new osg::TexMat(*osgTexMat);
    else
        newOSGTexMat = NULL;

    // Create the new attribute with the texture object copies we made
    newAttrib = new vsTextureRectangleAttribute(this->textureUnit,
        newOSGTexture, newOSGTexEnv, newOSGTexEnvCombine, newOSGTexGen,
        newOSGTexMat);

    // Attach and reference the texture image.   We re-use the image data
    // instead of cloning it to save on texture memory.  Typically, when
    // cloning a texture, you want to use the same image anyway
    newAttrib->setOSGImage(osgTexImage);

    // Give the clone our name
    newAttrib->setName((char *)getName());
    
    // Return the clone
    return newAttrib;
}

// ------------------------------------------------------------------------
// Sets the image data that this texture will display
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setImage(unsigned char *imageData,
    int xSize, int ySize, int dataFormat)
{
    int internalFormat;
    int pixelFormat;

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
            internalFormat = GL_LUMINANCE;
            pixelFormat = GL_LUMINANCE;
            break;
        case VS_TEXTURE_DFORMAT_INTENSITY_ALPHA:
            internalFormat = GL_LUMINANCE_ALPHA;
            pixelFormat = GL_LUMINANCE_ALPHA;
            break;
        case VS_TEXTURE_DFORMAT_RGB:
            internalFormat = GL_RGB;
            pixelFormat = GL_RGB;
            break;
        case VS_TEXTURE_DFORMAT_RGBA:
            internalFormat = GL_RGBA;
            pixelFormat = GL_RGBA;
            break;
        case VS_TEXTURE_DFORMAT_BGRA:
            internalFormat = GL_RGBA;
            pixelFormat = GL_BGRA;
            break;
        case VS_TEXTURE_DFORMAT_DXT1:
            internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            pixelFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            break;
        case VS_TEXTURE_DFORMAT_DXT1_ALPHA:
            internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            pixelFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case VS_TEXTURE_DFORMAT_DXT3:
            internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            pixelFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case VS_TEXTURE_DFORMAT_DXT5:
            internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            pixelFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            printf("vsTextureRectangleAttribute::setImage: "
                "Bad data format value");
            return;
    }

    // Pass the image data and settings to the osg::Image
    osgTexImage->setImage(xSize, ySize, 1, internalFormat, pixelFormat,
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
        case GL_BGRA:
            format = VS_TEXTURE_DFORMAT_BGRA;
            break;
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            format = VS_TEXTURE_DFORMAT_DXT1;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            format = VS_TEXTURE_DFORMAT_DXT1_ALPHA;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            format = VS_TEXTURE_DFORMAT_DXT3;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            format = VS_TEXTURE_DFORMAT_DXT5;
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
    osgDB::ReaderWriter::Options *options;

    // Unreference the current texture image (if any)
    if (osgTexImage)
        osgTexImage->unref();

    // Create a ReaderWriter::Options object specifying that we want .dds
    // files to be flipped vertically (this accounts for the different
    // texture coordinate systems between DirectX and OpenGL)
    options = new osgDB::ReaderWriter::Options("dds_flip");
    options->ref();

    // Read the image file into a new osg::Image object, passing the options
    // object
    osgTexImage = osgDB::readImageFile(filename, options);

    // Get rid of the options object
    options->unref();

    // If successful, set the TextureRectangle to use the new image and 
    // reference the image locally
    if (osgTexImage)
    {
        osgTexture->setImage(osgTexImage);
        osgTexImage->ref();
    }
    else
        printf("vsTextureRectangleAttribute::loadImageFromFile: "
            "Unable to load image from file %s\n", filename);
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
// Returns whether the current texture image has transparent pixels (alpha
// less than 1.0)
// ------------------------------------------------------------------------
bool vsTextureRectangleAttribute::isTransparent()
{
    // See if we have a texture image loaded
    if (osgTexImage)
    {
        // See if OSG thinks the image is translucent first
        if (osgTexImage->isImageTranslucent())
           return true;
        else
        {
           // OSG has a hard time detecting transparency in compressed
           // textures.  If the image data format is compressed and it
           // has an alpha channel, we'll assume it is translucent
           if ((osgTexImage->getPixelFormat() == 
                    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ||
               (osgTexImage->getPixelFormat() == 
                    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) ||
               (osgTexImage->getPixelFormat() == 
                    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT))
              return true;
           else
              return false;
        }
    }

    // No texture image, so not transparent
    return false;
}

// ------------------------------------------------------------------------
// Sets the boundary mode for the one axis of the texture. The boundary
// mode affects how texture coordinates that are out of the standard
// 0.0-1.0 bounds are treated.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setBoundaryMode(int whichDirection,
    int boundaryMode)
{
    // Print an error if any mode other than CLAMP is specified
    if (boundaryMode != VS_TEXTURE_BOUNDARY_CLAMP)
    {
        printf("vsTextureRectangleAttribute::setBoundaryMode:\n");
        printf("    Only CLAMP boundary mode is valid for texture "
            "rectangles.\n");
    }
}

// ------------------------------------------------------------------------
// Retrieves the boundary mode for one axis of the texture
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getBoundaryMode(int whichDirection)
{
    // The only mode allowed is CLAMP
    return VS_TEXTURE_BOUNDARY_CLAMP;
}

// ------------------------------------------------------------------------
// Sets the application mode of the texture
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setApplyMode(int applyMode)
{
    // See if we're using an osg::TexEnv or osg::TexEnvCombine
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
            case VS_TEXTURE_APPLY_BLEND:
                osgTexEnv->setMode(osg::TexEnv::BLEND);
                break;
            case VS_TEXTURE_APPLY_ADD:
                osgTexEnv->setMode(osg::TexEnv::ADD);
                break;
            default:
                printf("vsTextureRectangleAttribute::setApplyMode: "
                    "Bad apply mode value\n");
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
            case VS_TEXTURE_APPLY_BLEND:
                osgTexEnvCombine->
                    setCombine_RGB(osg::TexEnvCombine::INTERPOLATE);
                osgTexEnvCombine->
                    setCombine_Alpha(osg::TexEnvCombine::INTERPOLATE);
                break;
            case VS_TEXTURE_APPLY_ADD:
                osgTexEnvCombine->setCombine_RGB(osg::TexEnvCombine::ADD);
                osgTexEnvCombine->
                    setCombine_Alpha(osg::TexEnvCombine::ADD);
                break;
            default:
                printf("vsTextureRectangleAttribute::setApplyMode: Bad apply "
                    "mode value\n");
                return;
        }
    }
}

// ------------------------------------------------------------------------
// Retrieves the application mode of the texture
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getApplyMode()
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
            case osg::TexEnv::BLEND:
                return VS_TEXTURE_APPLY_BLEND;
            case osg::TexEnv::ADD:
                return VS_TEXTURE_APPLY_ADD;
        }
    }
    else
    {
        // Fetch and translate the osg::TexEnvCombine's combine mode.
        // Return the translated value.
        switch (osgTexEnvCombine->getCombine_RGB())
        {
            case osg::TexEnvCombine::INTERPOLATE:
                if (osgTexEnvCombine->getCombine_Alpha() == 
                    osg::TexEnvCombine::REPLACE)
                    return VS_TEXTURE_APPLY_DECAL;
                else
                    return VS_TEXTURE_APPLY_BLEND;

            case osg::TexEnvCombine::MODULATE:
                return VS_TEXTURE_APPLY_MODULATE;

            case osg::TexEnvCombine::REPLACE:
                return VS_TEXTURE_APPLY_REPLACE;

            case osg::TexEnvCombine::ADD:
                return VS_TEXTURE_APPLY_ADD;
        }
    }

    // Return -1 if we don't recognize the TexEnv's mode
    return -1;
}

// ------------------------------------------------------------------------
// Sets the magnification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setMagFilter(int newFilter)
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
int vsTextureRectangleAttribute::getMagFilter()
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
void vsTextureRectangleAttribute::setMinFilter(int newFilter)
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
        default:
            printf("vsTextureRectangleAttribute::setMinFilter:\n");
            printf("    Bad filter value (only NEAREST or LINEAR allowed)\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the minification filter used by the texture
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getMinFilter()
{
    // Translate the current MinFilter mode on the osg::Texture2D into
    // a VESS value and return it
    switch (osgTexture->getFilter(osg::Texture::MIN_FILTER))
    {
        case osg::Texture::NEAREST:
            return VS_TEXTURE_MINFILTER_NEAREST;
        case osg::Texture::LINEAR:
            return VS_TEXTURE_MINFILTER_LINEAR;
    }

    return -1;
}


// ------------------------------------------------------------------------
// Set the base color of the texture environment
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setBaseColor(atVector color)
{
    osg::Vec4 osgColor;

    // Get the current texture base color as an OSG vector
    osgColor.set(color[0], color[1], color[2], color[3]);

    // Set the color on the appropriate texture environment object
    if (osgTexEnvCombine != NULL)
        osgTexEnvCombine->setConstantColor(osgColor);
    else
        osgTexEnv->setColor(osgColor);
}

// ------------------------------------------------------------------------
// Get the base color of the texture environment
// ------------------------------------------------------------------------
atVector vsTextureRectangleAttribute::getBaseColor()
{
    osg::Vec4 osgColor;

    // Get the current base color from the appropriate OSG object
    if (osgTexEnvCombine != NULL)
        osgColor = osgTexEnvCombine->getConstantColor();
    else
        osgColor = osgTexEnv->getColor();

    // Return the color as a atVector
    return atVector(osgColor[0], osgColor[1], osgColor[2], osgColor[3]);
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
            default:
                return VS_TEXTURE_GEN_OFF;
        }
    }
    else
        return VS_TEXTURE_GEN_OFF;
}

// ------------------------------------------------------------------------
// Sets the texture matrix
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setTextureMatrix(atMatrix newTransform)
{
    osg::Matrixf osgMatrix;
    bool createdMat;

    // Convert the atMatrix into an osg::Matrix
    for (int loop = 0; loop < 4; loop++)
        for (int sloop = 0; sloop < 4; sloop++)
            osgMatrix(loop, sloop) = newTransform[sloop][loop];

    // See if we have an osg::TexMat to hold the texture matrix
    if (osgTexMat)
    {
        // We have one already, so don't create one
        createdMat = false;
    }
    else
    {
        // We don't have one, so create one
        osgTexMat = new osg::TexMat();
        osgTexMat->ref();
        createdMat = true;
    }

    // Apply osgMatrix to the osgTexMat
    osgTexMat->setMatrix(osgMatrix);

    // If we just created the texture matrix, let all owning nodes know
    // about the new state
    if (createdMat)
    {
        markOwnersDirty();
        setAllOwnersOSGAttrModes();
    }
}

// ------------------------------------------------------------------------
// Retrieves the texture matrix
// ------------------------------------------------------------------------
atMatrix vsTextureRectangleAttribute::getTextureMatrix()
{
    osg::Matrixf osgMatrix;
    atMatrix vsMat;

    // If we don't have a texture matrix, just return an identity matrix
    if (!osgTexMat)
    {
        vsMat.setIdentity();
        return vsMat;
    }

    // Get the current texture matrix
    osgMatrix = osgTexMat->getMatrix();

    // Convert the osg::Matrix into a atMatrix and return it
    for (int loop = 0; loop < 4; loop++)
        for (int sloop = 0; sloop < 4; sloop++)
            vsMat[sloop][loop] = osgMatrix(loop, sloop);
    return vsMat;
}

// ------------------------------------------------------------------------
// Changes the texture unit for this texture attribute.  This will fail if
// the texture attribute is already attached
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setTextureUnit(unsigned int unit)
{
    // If we're already on the right texture unit, don't do anything
    if (textureUnit == unit)
        return;

    // Make sure the attribute isn't already attached
    if (isAttached())
    {
        printf("vsTextureAttribute::setTextureUnit:\n");
        printf("    Cannot change texture unit when texture attribute "
            "is attached!\n");

        return;
    }

    // Change the texture unit
    textureUnit = unit;
}

// ------------------------------------------------------------------------
// Return the texture unit for this texture attribute
// ------------------------------------------------------------------------
unsigned int vsTextureRectangleAttribute::getTextureUnit()
{
    return textureUnit;
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

    // If a texture transformation matrix has been provided, update the state
    // set to reflect that
    if (osgTexMat)
        osgStateSet->setTextureAttributeAndModes(textureUnit, osgTexMat,
            attrMode);
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
void vsTextureRectangleAttribute::attachDuplicate(vsNode *theNode)
{
    // Attach a clone of this attribute to the given node
    theNode->addAttribute(this->clone());
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
    atMatrix mat1, mat2;
    
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
        
    // Compare texture matrices
    mat1 = getTextureMatrix();
    mat2 = attr->getTextureMatrix();
    if(!mat1.isEqual(mat2))
        return false;

    // If all pass, the attribute is equivalent
    return true;
}

// ------------------------------------------------------------------------
// Internal function
// Fetches the OSG Image object from this texture.  Mainly used for 
// cloning the texture attribute, but sharing the Image
// ------------------------------------------------------------------------
osg::Image *vsTextureRectangleAttribute::getOSGImage()
{
    // Return our osg::Image 
    return osgTexImage;
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
    if (osgTexImage)
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
