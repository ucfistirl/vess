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
    performerTexture->setFormat(PFTEX_INTERNAL_FORMAT, PFTEX_RGBA_8);
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
    
    if (boundaryMode == VS_TEXTURE_BOUNDARY_REPEAT)
        wrapType = PFTEX_REPEAT;
    else
        wrapType = PFTEX_CLAMP;

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

    if (whichDirection == VS_TEXTURE_DIRECTION_T)
        wrapType = performerTexture->getRepeat(PFTEX_WRAP_T);
    else
        wrapType = performerTexture->getRepeat(PFTEX_WRAP_S);

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
    switch (performerTexEnv->getMode())
    {
        case PFTE_DECAL:
            return VS_TEXTURE_APPLY_DECAL;
        case PFTE_MODULATE:
            return VS_TEXTURE_APPLY_MODULATE;
        case PFTE_REPLACE:
            return VS_TEXTURE_APPLY_REPLACE;
    }

    return -1;
}

// ------------------------------------------------------------------------
// Sets the magnification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setMagFilter(int newFilter)
{
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
    switch (performerTexture->getFilter(PFTEX_MAGFILTER))
    {
        case PFTEX_POINT:
            return VS_TEXTURE_MAGFILTER_NEAREST;
        case PFTEX_LINEAR:
            return VS_TEXTURE_MAGFILTER_LINEAR;
    }
    
    return -1;
}

// ------------------------------------------------------------------------
// Sets the minification filter used by the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setMinFilter(int newFilter)
{
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
    
    return -1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTextureAttribute::saveCurrent()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    attrSaveList[attrSaveCount++] = gState->getTexture();
}

// ------------------------------------------------------------------------
// VESS internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTextureAttribute::apply()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setTexture(this);
}

// ------------------------------------------------------------------------
// VESS internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTextureAttribute::restoreSaved()
{
    vsGraphicsState *gState = (vsSystem::systemObject)->getGraphicsState();

    gState->setTexture((vsTextureAttribute *)(attrSaveList[--attrSaveCount]));
}

// ------------------------------------------------------------------------
// VESS internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTextureAttribute::setState(pfGeoState *state)
{
    state->setMode(PFSTATE_ENTEXTURE, PF_ON);
    state->setAttr(PFSTATE_TEXENV, performerTexEnv);
    state->setAttr(PFSTATE_TEXTURE, performerTexture);
}
