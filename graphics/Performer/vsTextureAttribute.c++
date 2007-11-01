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

#include "vsTextureAttribute.h++"

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the Performer texture objects for the
// default texture unit (0) and initializes default settings
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute()
{
    // Create the Performer texture and texture environment objects
    performerTexture = new pfTexture();
    performerTexture->ref();
    performerTexEnv = new pfTexEnv();
    performerTexEnv->ref();
    performerTexEnv->setMode(PFTE_MODULATE);
    performerTexGen = NULL;
    textureMatrix.makeIdent();
    textureMatrixEnabled = false;

    // Set to the default texture unit.
    textureUnit = 0;
}

// ------------------------------------------------------------------------
// Constructor - Creates the Performer texture objects for the specified
// texture unit and initializes default settings
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute(unsigned int unit)
{
    // Create the Performer texture and texture environment objects
    performerTexture = new pfTexture();
    performerTexture->ref();
    performerTexEnv = new pfTexEnv();
    performerTexEnv->ref();
    performerTexEnv->setMode(PFTE_MODULATE);
    performerTexGen = NULL;
    textureMatrix.makeIdent();
    textureMatrixEnabled = false;

    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureAttribute::vsTextureAttribute: Invalid texture unit, "
            "using default of 0\n");
        textureUnit = 0;
    }
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureAttribute::vsTextureAttribute(unsigned int unit, pfTexture *texObject,
    pfTexEnv *texEnvObject, pfTexGen *texGenObject, pfMatrix texMat, 
    bool useTexMat)
{
    // Store pointers to the specified Performer texture and texture
    // environment objects
    performerTexture = texObject;
    performerTexture->ref();
    performerTexEnv = texEnvObject;
    performerTexEnv->ref();
    performerTexGen = texGenObject;
    if (performerTexGen)
        performerTexGen->ref();
    textureMatrixEnabled = useTexMat;
    if (textureMatrixEnabled)
        textureMatrix.copy(texMat);
    else
        textureMatrix.makeIdent();

    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureAttribute::vsTextureAttribute: Invalid texture unit, "
            "using default of 0\n");
        textureUnit = 0;
    }
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
    if (performerTexGen)
    {
        performerTexGen->unref();
        pfDelete(performerTexGen);
    }

    // Try removing a link between this attribute and one of the Performer
    // textures, in the case that the vsGeometry constructor put one in
    // in the first place.
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
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
            printf("vsTextureAttribute::setImage: Bad data format value\n");
            return;
    }

    // Set the image data and format information on the Performer texture
    performerTexture->setFormat(PFTEX_INTERNAL_FORMAT, PFTEX_RGBA_8);
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
// Notifies the texture attribute that the texture data has been changed by
// some outside source, and forces it to retransfer the data to the
// graphics hardware.
// ------------------------------------------------------------------------
void vsTextureAttribute::reloadTextureData()
{
    // The Performer texture object already has the pointer to the texture
    // data, we just need a way to tell the pfTexture that the data changed.
    // This is done by 'dirtying' the object. Since there's no direct-purpose
    // 'dirty' call, we have to dirty the object by calling any of it's
    // parameter-set functions. Getting and immediately setting some value
    // that the texture object has as a member variable should do just fine.
    performerTexture->setLoadImage(performerTexture->getLoadImage());
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
        case VS_TEXTURE_APPLY_BLEND:
            performerTexEnv->setMode(PFTE_BLEND);
            break;
        case VS_TEXTURE_APPLY_ADD:
            performerTexEnv->setMode(PFTE_ADD);
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
        case PFTE_BLEND:
            return VS_TEXTURE_APPLY_BLEND;
        case PFTE_ADD:
            return VS_TEXTURE_APPLY_ADD;
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
// Sets the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
void vsTextureAttribute::setGenMode(int genMode)
{
    // Translate the genMode to a Performer value and set it on the pfTexGen
    switch (genMode)
    {
        case VS_TEXTURE_GEN_OBJECT_LINEAR:
            if (performerTexGen == NULL)
            {
                performerTexGen = new pfTexGen();
                performerTexGen->ref();
            }
            performerTexGen->setMode(PF_S, PFTG_OBJECT_LINEAR);
            performerTexGen->setMode(PF_T, PFTG_OBJECT_LINEAR);
            performerTexGen->setMode(PF_R, PFTG_OBJECT_LINEAR);
            break;
        case VS_TEXTURE_GEN_EYE_LINEAR:
            if (performerTexGen == NULL)
            {
                performerTexGen = new pfTexGen();
                performerTexGen->ref();
            }
            performerTexGen->setMode(PF_S, PFTG_EYE_LINEAR);
            performerTexGen->setMode(PF_T, PFTG_EYE_LINEAR);
            performerTexGen->setMode(PF_R, PFTG_EYE_LINEAR);
            break;
        case VS_TEXTURE_GEN_SPHERE_MAP:
            if (performerTexGen == NULL)
            {
                performerTexGen = new pfTexGen();
                performerTexGen->ref();
            }
            performerTexGen->setMode(PF_S, PFTG_SPHERE_MAP);
            performerTexGen->setMode(PF_T, PFTG_SPHERE_MAP);
            performerTexGen->setMode(PF_R, PFTG_SPHERE_MAP);
            break;
        case VS_TEXTURE_GEN_NORMAL_MAP:
            if (performerTexGen == NULL)
            {
                performerTexGen = new pfTexGen();
                performerTexGen->ref();
            }
            performerTexGen->setMode(PF_S, PFTG_NORMAL_MAP);
            performerTexGen->setMode(PF_T, PFTG_NORMAL_MAP);
            performerTexGen->setMode(PF_R, PFTG_NORMAL_MAP);
            break;
        case VS_TEXTURE_GEN_REFLECTION_MAP:
            if (performerTexGen == NULL)
            {
                performerTexGen = new pfTexGen();
                performerTexGen->ref();
            }
            performerTexGen->setMode(PF_S, PFTG_REFLECTION_MAP);
            performerTexGen->setMode(PF_T, PFTG_REFLECTION_MAP);
            performerTexGen->setMode(PF_R, PFTG_REFLECTION_MAP);
            break;
        case VS_TEXTURE_GEN_OFF:
            if (performerTexGen)
            {
                performerTexGen->setMode(PF_S, PFTG_OFF);
                performerTexGen->setMode(PF_T, PFTG_OFF);
                performerTexGen->setMode(PF_R, PFTG_OFF);
            }
            break;
        default:
            printf("vsTextureAttribute::setGenMode: Bad generation mode "
                "value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
int vsTextureAttribute::getGenMode()
{
    if (performerTexGen)
    {
        // Translate the current texture coordinate generation mode on the
        // pfTexGen into a VESS value and return it
        switch (performerTexGen->getMode(PF_S))
        {
            case PFTG_OBJECT_LINEAR:
                return VS_TEXTURE_GEN_OBJECT_LINEAR;
            case PFTG_EYE_LINEAR:
                return VS_TEXTURE_GEN_EYE_LINEAR;
            case PFTG_SPHERE_MAP:
                return VS_TEXTURE_GEN_SPHERE_MAP;
            case PFTG_NORMAL_MAP:
                return VS_TEXTURE_GEN_NORMAL_MAP;
            case PFTG_REFLECTION_MAP:
                return VS_TEXTURE_GEN_REFLECTION_MAP;
            case PFTG_OFF:
            default:
                return VS_TEXTURE_GEN_OFF;
        }
    }
    else
        return VS_TEXTURE_GEN_OFF;
}

// ------------------------------------------------------------------------
// Set a new texture matrix
// ------------------------------------------------------------------------
void vsTextureAttribute::setTextureMatrix(atMatrix newMatrix)
{
    // Convert the atMatrix into a pfMatrix
    for (int loop = 0; loop < 4; loop++)
        for (int sloop = 0; sloop < 4; sloop++)
            textureMatrix[loop][sloop] = newMatrix[sloop][loop];

    // Set that we're now using the texture matrix, so the proper modes
    // are set in the Performer state
    textureMatrixEnabled = true;
}

// ------------------------------------------------------------------------
// Retrieve the current texture matrix
// ------------------------------------------------------------------------
atMatrix vsTextureAttribute::getTextureMatrix()
{
    atMatrix vsMat;

    // Convert the current texture matrix into a atMatrix and return it
    for (int loop = 0; loop < 4; loop++)
        for (int sloop = 0; sloop < 4; sloop++)
            vsMat[sloop][loop] = textureMatrix[loop][sloop];
    return vsMat;
}

// ------------------------------------------------------------------------
// Return the set texture unit for this textureAttribute.
// ------------------------------------------------------------------------
unsigned int vsTextureAttribute::getTextureUnit()
{
    return textureUnit;
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
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTextureAttribute::saveCurrent()
{
    vsGraphicsState *gState;

    // Get the current vsGraphicsState object
    gState = vsGraphicsState::getInstance();

    // Save the current texture state in our save list
    if (gState->getTexture(textureUnit))
        attrSaveList[attrSaveCount] = gState->getTexture(textureUnit);
    else if (gState->getTextureCube(textureUnit))
        attrSaveList[attrSaveCount] = gState->getTextureCube(textureUnit);
    else if (gState->getTextureRect(textureUnit))
        attrSaveList[attrSaveCount] = gState->getTextureRect(textureUnit);
    else
        attrSaveList[attrSaveCount] = NULL;

    attrSaveCount++;
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTextureAttribute::apply()
{
    // Get the current vsGraphicsState object
    vsGraphicsState *gState = vsGraphicsState::getInstance();

    // Set the current texture state to this object
    gState->setTexture(textureUnit, this);

    // Lock the texture state if overriding is enabled
    if (overrideFlag)
        gState->lockTexture(textureUnit, this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTextureAttribute::restoreSaved()
{
    vsGraphicsState *gState;
    vsStateAttribute *tempPointer;

    // Get the current vsGraphicsState object
    gState = vsGraphicsState::getInstance();

    // Unlock the texture if overriding was enabled
    if (overrideFlag)
        gState->unlockTexture(textureUnit, this);

    // Reset the current texture to its previous value
    attrSaveCount--;
    tempPointer = (vsStateAttribute *)(attrSaveList[attrSaveCount]);

    if (tempPointer == NULL)
        gState->setTexture(textureUnit, NULL);
    else if (tempPointer->getAttributeType() == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
        gState->setTextureRect(
            textureUnit, (vsTextureRectangleAttribute *) tempPointer);
    else if (tempPointer->getAttributeType() == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
        gState->setTextureCube(textureUnit,
            (vsTextureCubeAttribute *) tempPointer);
    else if (tempPointer->getAttributeType() == VS_ATTRIBUTE_TYPE_TEXTURE)
        gState->setTexture(
            textureUnit, (vsTextureAttribute *) tempPointer);
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTextureAttribute::setState(pfGeoState *state)
{
    // Set textures as enabled and set our Performer texture objects
    // on the geostate
    state->setMultiMode(PFSTATE_ENTEXTURE, textureUnit, PF_ON);
    state->setMultiAttr(PFSTATE_TEXENV, textureUnit, performerTexEnv);
    state->setMultiAttr(PFSTATE_TEXTURE, textureUnit, performerTexture);
    if (performerTexGen)
    {
        state->setMultiMode(PFSTATE_ENTEXGEN, textureUnit, PF_ON);
        state->setMultiAttr(PFSTATE_TEXGEN, textureUnit, performerTexGen);
    }
    if (textureMatrixEnabled)
    {
        state->setMultiMode(PFSTATE_ENTEXMAT, textureUnit, PF_ON);
        state->setMultiAttr(PFSTATE_TEXMAT, textureUnit, &textureMatrix);
    }
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
    atMatrix mat1, mat2;
    
    // NULL check
    if (!attribute)
        return false;

    // Equal pointer check
    if (this == attribute)
        return true;
    
    // Type check
    if (attribute->getAttributeType() != VS_ATTRIBUTE_TYPE_TEXTURE)
        return false;

    // Type cast
    attr = (vsTextureAttribute *)attribute;

    // Image data check
    getImage(&image1, &xval1, &yval1, &val1);
    attr->getImage(&image2, &xval2, &yval2, &val2);
    if ((image1 != image2) || (xval1 != xval2) || (yval1 != val2) ||
        (val1 != val2))
        return false;

    // Horizontal boundary mode check
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_S);
    if (val1 != val2)
        return false;

    // Vertical boundary mode check
    val1 = getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    val2 = attr->getBoundaryMode(VS_TEXTURE_DIRECTION_T);
    if (val1 != val2)
        return false;

    // Texture coordinate generation mode check
    val1 = getGenMode();
    val2 = attr->getGenMode();
    if (val1 != val2)
        return false;

    // Apply mode check
    val1 = getApplyMode();
    val2 = attr->getApplyMode();
    if (val1 != val2)
        return false;

    // Magnification filter check
    val1 = getMagFilter();
    val2 = attr->getMagFilter();
    if (val1 != val2)
        return false;

    // Minification filter check
    val1 = getMinFilter();
    val2 = attr->getMinFilter();
    if (val1 != val2)
        return false;

    // Texture unit check
    val1 = getTextureUnit();
    val2 = attr->getTextureUnit();
    if (val1 != val2)
        return false;

    // Texture matrix check
    mat1 = getTextureMatrix();
    mat2 = attr->getTextureMatrix();
    if (!mat1.isEqual(mat2))
        return false;

    // Attributes are equivalent if all checks pass
    return true;
}
