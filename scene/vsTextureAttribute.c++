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

#include "vsTextureAttribute.h++"

#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the Performer texture objects and
// initializes default settings
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute()
{
    // Create the Performer texture and texture environment objects
    performerTexture = new pfTexture();
    performerTexture->ref();
    performerTexEnv = new pfTexEnv();
    performerTexEnv->ref();
    performerTexEnv->setMode(PFTE_DECAL);
}

// ------------------------------------------------------------------------
// VESS internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute(pfTexture *texObject,
    pfTexEnv *texEnvObject)
{
    // Store pointers to the specified Performer texture and texture
    // environment objects
    performerTexture = texObject;
    performerTexture->ref();
    performerTexEnv = texEnvObject;
    performerTexEnv->ref();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTextureAttribute::~vsTextureAttribute()
{
    // Delete the Performer objects
    performerTexture->unref();
    pfDelete(performerTexture);
    performerTexEnv->unref();
    pfDelete(performerTexEnv);

    // Try removing a link between this attribute and one of the Performer
    // textures, in the case that the vsGeometry constructor put one in
    // in the first place.
    ((vsSystem::systemObject)->getNodeMap())->removeLink(this,
        VS_OBJMAP_FIRST_LIST);
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
    int format, comp;

    // Decode the data format value into a Performer data format constant
    // and the number of bytes per pixel.
    switch (dataFormat)
    {
        case VS_TEXTURE_DFORMAT_INTENSITY:
            format = PFTEX_LUMINANCE;
            comp = 1;
            break;
        case VS_TEXTURE_DFORMAT_INTENSITY_ALPHA:
            format = PFTEX_LUMINANCE_ALPHA;
            comp = 2;
            break;
        case VS_TEXTURE_DFORMAT_RGB:
            format = PFTEX_RGB;
            comp = 3;
            break;
        case VS_TEXTURE_DFORMAT_RGBA:
            format = PFTEX_RGBA;
            comp = 4;
            break;
        default:
            printf("vsTextureAttribute::setImage: Bad data format value");
            return;
    }

    // Set the image data and format information on the Performer texture
    performerTexture->setFormat(PFTEX_INTERNAL_FORMAT,
        PFTEX_RGBA_8 | PFTEX_GEN_MIPMAP_FORMAT);
    performerTexture->setFormat(PFTEX_EXTERNAL_FORMAT, PFTEX_UNSIGNED_BYTE);
    performerTexture->setFormat(PFTEX_IMAGE_FORMAT, format);
    performerTexture->setImage((uint *)imageData, comp, xSize, ySize, 1);
}

// ------------------------------------------------------------------------
// Retrieves a pointer to the image data that this texture is set to
// display, as well as its size and format. NULL pointers may be passed in
// for undesired values.
// ------------------------------------------------------------------------
void vsTextureAttribute::getImage(unsigned char **imageData, int *xSize,
                                   int *ySize, int *dataFormat)
{
    uint *image;
    int comp, ns, nt, nr;
    int format;
    
    // Get the image data from the Performer texture
    performerTexture->getImage(&image, &comp, &ns, &nt, &nr);
    switch (comp)
    {
        case 1:
            format = VS_TEXTURE_DFORMAT_INTENSITY;
            break;
        case 2:
            format = VS_TEXTURE_DFORMAT_INTENSITY_ALPHA;
            break;
        case 3:
            format = VS_TEXTURE_DFORMAT_RGB;
            break;
        case 4:
            format = VS_TEXTURE_DFORMAT_RGBA;
            break;
    }

    // Return those values that the user desires
    if (imageData)
        *imageData = ((unsigned char *)image);
    if (xSize)
        *xSize = ns;
    if (ySize)
        *ySize = nt;
    if (dataFormat)
        *dataFormat = format;
}

// ------------------------------------------------------------------------
// Loads texture image data from the file with the indicated name
// ------------------------------------------------------------------------
void vsTextureAttribute::loadImageFromFile(char *filename)
{
    // Set the internal data format of the texture data to 32 bits per
    // texel, with 8 bits each red, green, blue, and alpha
    performerTexture->setFormat(PFTEX_INTERNAL_FORMAT, PFTEX_RGBA_8);

    // Load the texture data from the designated file
    if (!(performerTexture->loadFile(filename)))
        printf("vsTextureAttribute::loadImageFromFile: Unable to load image\n");
}

// ------------------------------------------------------------------------
// Sets the boundary mode for the one axis of the texture. The boundary
// mode affects how texture coordinates that are out of the standard
// 0.0-1.0 bounds are treated.
// ------------------------------------------------------------------------
void vsTextureAttribute::setBoundaryMode(int whichDirection, int boundaryMode)
{
    int wrapType;
    
    // Translate the VESS texture wrap constant to Performer
    if (boundaryMode == VS_TEXTURE_BOUNDARY_REPEAT)
        wrapType = PFTEX_REPEAT;
    else
        wrapType = PFTEX_CLAMP;

    // Set the desired Performer texture wrap mode based on the direction
    // constant
    switch (whichDirection)
    {
        case VS_TEXTURE_DIRECTION_S:
            performerTexture->setRepeat(PFTEX_WRAP_S, wrapType);
            break;
        case VS_TEXTURE_DIRECTION_T:
            performerTexture->setRepeat(PFTEX_WRAP_T, wrapType);
            break;
        case VS_TEXTURE_DIRECTION_ALL:
            performerTexture->setRepeat(PFTEX_WRAP, wrapType);
            break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the boundary mode for one axis of the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getBoundaryMode(int whichDirection)
{
    int wrapType;

    // Set the desired Performer texture wrap mode based on the direction
    // constant
    if (whichDirection == VS_TEXTURE_DIRECTION_T)
        wrapType = performerTexture->getRepeat(PFTEX_WRAP_T);
    else
        wrapType = performerTexture->getRepeat(PFTEX_WRAP_S);

    // Translate the Performer texture wrap constant to VESS
    if (wrapType == PFTEX_REPEAT)
        return VS_TEXTURE_BOUNDARY_REPEAT;
    else
        return VS_TEXTURE_BOUNDARY_CLAMP;
}

// ------------------------------------------------------------------------
// Sets the application mode of the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setApplyMode(int applyMode)
{
    // Set the Performer texture environment's apply mode based on the
    // VESS apply mode constant passed in
    switch (applyMode)
    {
        case VS_TEXTURE_APPLY_DECAL:
            performerTexEnv->setMode(PFTE_DECAL);
            break;
        case VS_TEXTURE_APPLY_MODULATE:
            performerTexEnv->setMode(PFTE_MODULATE);
            break;
        case VS_TEXTURE_APPLY_REPLACE:
            performerTexEnv->setMode(PFTE_REPLACE);
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
    // Get the Performer texture environment mode, and translate that
    // to a VESS apply mode constant
    switch (performerTexEnv->getMode())
    {
        case PFTE_DECAL:
            return VS_TEXTURE_APPLY_DECAL;
        case PFTE_MODULATE:
            return VS_TEXTURE_APPLY_MODULATE;
        case PFTE_REPLACE:
            return VS_TEXTURE_APPLY_REPLACE;
    }

    // If the mode is unrecognized, return an error value
    return -1;
}

// ------------------------------------------------------------------------
// Sets the magnification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setMagFilter(int newFilter)
{
    // Translate the VESS magnification filter constant to a call to
    // the Performer texture filter mode set function
    switch (newFilter)
    {
        case VS_TEXTURE_MAGFILTER_NEAREST:
            performerTexture->setFilter(PFTEX_MAGFILTER, PFTEX_POINT);
            break;
        case VS_TEXTURE_MAGFILTER_LINEAR:
            performerTexture->setFilter(PFTEX_MAGFILTER, PFTEX_LINEAR);
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
    // Get the magnification filter constant value from the Performer
    // texture object, and translate that to a VESS constant
    switch (performerTexture->getFilter(PFTEX_MAGFILTER))
    {
        case PFTEX_POINT:
            return VS_TEXTURE_MAGFILTER_NEAREST;
        case PFTEX_LINEAR:
            return VS_TEXTURE_MAGFILTER_LINEAR;
    }
    
    // If the mode is unrecognized, return an error value
    return -1;
}

// ------------------------------------------------------------------------
// Sets the minification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setMinFilter(int newFilter)
{
    // Translate the VESS minification filter constant to a call to
    // the Performer texture filter mode set function
    switch (newFilter)
    {
        case VS_TEXTURE_MINFILTER_NEAREST:
            performerTexture->setFilter(PFTEX_MINFILTER, PFTEX_POINT);
            break;
        case VS_TEXTURE_MINFILTER_LINEAR:
            performerTexture->setFilter(PFTEX_MINFILTER, PFTEX_LINEAR);
            break;
        case VS_TEXTURE_MINFILTER_MIPMAP_NEAREST:
            performerTexture->setFilter(PFTEX_MINFILTER, PFTEX_MIPMAP_POINT);
            break;
        case VS_TEXTURE_MINFILTER_MIPMAP_LINEAR:
            performerTexture->setFilter(PFTEX_MINFILTER, PFTEX_MIPMAP_LINEAR);
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
    // Get the minification filter constant value from the Performer
    // texture object, and translate that to a VESS constant
    switch (performerTexture->getFilter(PFTEX_MINFILTER))
    {
        case PFTEX_POINT:
            return VS_TEXTURE_MINFILTER_NEAREST;
        case PFTEX_LINEAR:
            return VS_TEXTURE_MINFILTER_LINEAR;
        case PFTEX_MIPMAP_POINT:
            return VS_TEXTURE_MINFILTER_MIPMAP_NEAREST;
        case PFTEX_MIPMAP_LINEAR:
            return VS_TEXTURE_MINFILTER_MIPMAP_LINEAR;
    }
    
    // If the mode is unrecognized, return an error value
    return -1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Attaches a duplicate of this attribute to the given node
// ------------------------------------------------------------------------
void vsTextureAttribute::attachDuplicate(vsNode *theNode)
{
    vsTextureAttribute *newAttrib;
    pfTexture *newTex;
    pfTexEnv *newTexEnv;
    
    // Create new Performer objects as duplicates of the ones we have
    newTex = new pfTexture();
    newTex->copy(performerTexture);
    newTexEnv = new pfTexEnv();
    newTexEnv->copy(performerTexEnv);
    
    // Create a duplicate texture attribute using the new Performer objects
    newAttrib = new vsTextureAttribute(newTex, newTexEnv);

    // Attach the duplicate attribute to the specified node
    theNode->addAttribute(newAttrib);
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTextureAttribute::saveCurrent()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Save the current texture state in our save list
    attrSaveList[attrSaveCount++] = gState->getTexture();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTextureAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Set the current texture state to this object
    gState->setTexture(this);

    // Lock the texture state if overriding is enabled
    if (overrideFlag)
        gState->lockTexture(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTextureAttribute::restoreSaved()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    // Unlock the wireframe state if overriding was enabled
    if (overrideFlag)
        gState->unlockTexture(this);

    // Reset the current wireframe state to its previous value
    gState->setTexture((vsTextureAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTextureAttribute::setState(pfGeoState *state)
{
    // Set textures as enabled and set our Performer texture objects
    // on the geostate
    state->setMode(PFSTATE_ENTEXTURE, PF_ON);
    state->setAttr(PFSTATE_TEXENV, performerTexEnv);
    state->setAttr(PFSTATE_TEXTURE, performerTexture);
}

// ------------------------------------------------------------------------
// VESS internal function
// Determines if the specified attribute has state information that is
// equivalent to what this attribute has
// ------------------------------------------------------------------------
int vsTextureAttribute::isEquivalent(vsAttribute *attribute)
{
    vsTextureAttribute *attr;
    unsigned char *image1, *image2;
    int xval1, yval1, xval2, yval2, val1, val2;
    
    // NULL check
    if (!attribute)
        return VS_FALSE;

    // Equal pointer check
    if (this == attribute)
        return VS_TRUE;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TEXTURE)
        return VS_FALSE;

    // Type cast
    attr = (vsTextureAttribute *)attribute;

    // Image data check
    getImage(&image1, &xval1, &yval1, &val1);
    attr->getImage(&image2, &xval2, &yval2, &val2);
    if ((image1 != image2) || (xval1 != xval2) || (yval1 != val2) ||
        (val1 != val2))
        return VS_FALSE;

    // Horizontal boundary mode check
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    if (val1 != val2)
        return VS_FALSE;

    // Vertical boundary mode check
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    if (val1 != val2)
        return VS_FALSE;

    // Apply mode check
    val1 = getApplyMode();
    val2 = attr->getApplyMode();
    if (val1 != val2)
        return VS_FALSE;

    // Magnification filter check
    val1 = getMagFilter();
    val2 = attr->getMagFilter();
    if (val1 != val2)
        return VS_FALSE;

    // Minification filter check
    val1 = getMinFilter();
    val2 = attr->getMinFilter();
    if (val1 != val2)
        return VS_FALSE;

    // Attributes are equivalent if all checks pass
    return VS_TRUE;
}
