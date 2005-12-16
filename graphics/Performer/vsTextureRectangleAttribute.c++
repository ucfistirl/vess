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
//    VESS Module:  vsTextureRectangleAttribute.c++
//
//    Description:  Attribute that specifies which texture should be used
//                  to cover geometry. Works on textures that don't have
//                  power-of-two dimensions.
//
//    Author(s):    Bryan Kline, Casey Thurston, Jason Daly
//
//------------------------------------------------------------------------

#include "vsTextureRectangleAttribute.h++"

#include "vsGraphicsState.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates the Performer texture objects for the
// default texture unit (0) and initializes default settings
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute()
{
    // Create the Performer texture and texture environment objects
    performerTexture = new pfTexture();
    performerTexture->ref();
    performerTexEnv = new pfTexEnv();
    performerTexEnv->ref();
    performerTexEnv->setMode(PFTE_DECAL);
    performerTexGen = NULL;

    // Set to the default texture unit
    textureUnit = 0;

    // Create a shared-memory structure for use by the node traversal function
    textureData = (vsTextureRectangleData *)pfMalloc(
        sizeof(vsTextureRectangleData), pfGetSharedArena());

    // Fill in the structure's fields with default values. -1 is used as a
    // sentinel value for the target because actual values are unsigned.
    textureData->target = -1;
    textureData->internalFormat = 0;
    textureData->width = 0;
    textureData->height = 0;
    textureData->format = 0;
    textureData->type = 0;
    textureData->unit = 0;
    textureData->multitexture = false;
    textureData->data = NULL;

    textureData->dirty = true;
}

// ------------------------------------------------------------------------
// Constructor - Creates the Performer texture objects for the specified
// texture unit and initializes default settings
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute(unsigned int unit)
{
    // Create the Performer texture and texture environment objects
    performerTexture = new pfTexture();
    performerTexture->ref();
    performerTexEnv = new pfTexEnv();
    performerTexEnv->ref();
    performerTexEnv->setMode(PFTE_DECAL);
    performerTexGen = NULL;

    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureRectangleAttribute::vsTextureRectangleAttribute: "
            "Invalid texture unit, using default of 0\n");
        textureUnit = 0;
    }

    // Create a shared-memory structure for use by the node traversal function
    textureData = (vsTextureRectangleData *)pfMalloc(
        sizeof(vsTextureRectangleData), pfGetSharedArena());

    // Fill in the structure's fields with default values. -1 is used as a
    // sentinel value for the target because actual values are unsigned.
    textureData->target = -1;
    textureData->internalFormat = 0;
    textureData->width = 0;
    textureData->height = 0;
    textureData->format = 0;
    textureData->type = 0;
    textureData->unit = unit;
    textureData->multitexture = false;
    textureData->data = NULL;

    textureData->dirty = true;
}

// ------------------------------------------------------------------------
// Internal function
// Constructor - Sets the texture attribute up as already attached
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::vsTextureRectangleAttribute(unsigned int unit,
    pfTexture *texObject, pfTexEnv *texEnvObject, pfTexGen *texGenObject)
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

    // Set to the specified texture unit.
    if ((unit >= 0) && (unit < VS_MAXIMUM_TEXTURE_UNITS))
        textureUnit = unit;
    else
    {
        printf("vsTextureRectangleAttribute::vsTextureRectangleAttribute: "
            "Invalid texture unit, using default of 0\n");
        textureUnit = 0;
    }

    // Create a shared-memory structure for use by the node traversal function
    textureData = (vsTextureRectangleData *)pfMalloc(
        sizeof(vsTextureRectangleData), pfGetSharedArena());

    // Fill in the structure's fields with default values. -1 is used as a
    // sentinel value for the target because actual values are unsigned.
    textureData->target = -1;
    textureData->internalFormat = 0;
    textureData->width = 0;
    textureData->height = 0;
    textureData->format = 0;
    textureData->type = 0;
    textureData->unit = unit;
    textureData->multitexture = false;
    textureData->data = NULL;

    textureData->dirty = true;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTextureRectangleAttribute::~vsTextureRectangleAttribute()
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

    // Free the shared memory used to hold the texture and the structure
    // for passing data down the Performer pipeline
    if (textureData->data)
        pfFree(textureData->data);
    pfFree(textureData);
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
    int format, comp, dataSize;

    // Decode the data format value into a Performer data format constant,
    // a GL data format constant, and the number of bytes per pixel
    switch (dataFormat)
    {
        case VS_TEXTURE_DFORMAT_INTENSITY:
            textureData->format = GL_LUMINANCE;
            format = PFTEX_LUMINANCE;
            comp = 1;
            break;
        case VS_TEXTURE_DFORMAT_INTENSITY_ALPHA:
            textureData->format = GL_LUMINANCE_ALPHA;
            format = PFTEX_LUMINANCE_ALPHA;
            comp = 2;
            break;
        case VS_TEXTURE_DFORMAT_RGB:
            textureData->format = GL_RGB;
            format = PFTEX_RGB;
            comp = 3;
            break;
        case VS_TEXTURE_DFORMAT_RGBA:
            textureData->format = GL_RGBA;
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

    // Update the shared memory structure with the new data
    textureData->internalFormat = comp;
    textureData->width = xSize;
    textureData->height = ySize;
    textureData->type = GL_UNSIGNED_BYTE;

    // Signal that the texture data must be reloaded
    textureData->dirty = true;

    // Free the old texture
    if (textureData->data)
        pfFree(textureData->data);

    // Calculate the total amount of data taken up by the texture
    dataSize = xSize * ySize * comp;

    // Allocate space for texture data and copy it into the new variable
    textureData->data = pfMalloc(dataSize, pfGetSharedArena());
    memcpy(textureData->data, imageData, dataSize);
}

// ------------------------------------------------------------------------
// Retrieves a pointer to the image data that this texture is set to
// display, as well as its size and format. NULL pointers may be passed in
// for undesired values.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::getImage(unsigned char **imageData,
    int *xSize, int *ySize, int *dataFormat)
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
void vsTextureRectangleAttribute::loadImageFromFile(char *filename)
{
    int xSize, ySize, comp, dataSize;
    unsigned int *imageData;

    // Set the internal data format of the texture data to 32 bits per
    // texel, with 8 bits each red, green, blue, and alpha
    performerTexture->setFormat(PFTEX_INTERNAL_FORMAT, PFTEX_RGBA_8);

    // Load the texture data from the designated file
    if (!(performerTexture->loadFile(filename)))
    {
        printf("vsTextureRectangleAttribute::loadImageFromFile: Unable to "
            "load image\n");
        return;
    }

    // Query the texture for its data, format, and size
    performerTexture->getImage(&imageData, &comp, &xSize, &ySize, NULL);

    // Update the shared memory structure with the new data
    textureData->internalFormat = comp;
    textureData->width = xSize;
    textureData->height = ySize;
    textureData->format = GL_RGBA;
    textureData->type = GL_UNSIGNED_BYTE;
                                                                                                                                                             
    // Free the old texture
    if (textureData->data)
        pfFree(textureData->data);
                                                                                                                                                             
    // Signal that the texture data must be reloaded
    textureData->dirty = true;

    // Calculate the total amount of data taken up by the texture
    dataSize = xSize * ySize * comp;
                                                                                                                                                             
    // Allocate space for texture data and copy it into the new variable
    textureData->data = pfMalloc(dataSize, pfGetSharedArena());
    memcpy(textureData->data, imageData, dataSize);
}

// ------------------------------------------------------------------------
// Notifies the texture attribute that the texture data has been changed by
// some outside source, and forces it to retransfer the data to the
// graphics hardware.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::reloadTextureData()
{
    int xSize, ySize, comp;
    unsigned int *data;

    // Query the texture for its data, format, and size
    performerTexture->getImage(&data, &comp, &xSize, &ySize, NULL);

    // Feed the updated data through the pipeline
    memcpy(textureData->data, data, xSize * ySize * comp);

    // Signal that the texture data must be reloaded
    textureData->dirty = true;
}

// ------------------------------------------------------------------------
// Sets the boundary mode for the one axis of the texture. The boundary
// mode affects how texture coordinates that are out of the standard
// 0.0-1.0 bounds are treated.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setBoundaryMode(int whichDirection,
                                                  int boundaryMode)
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
int vsTextureRectangleAttribute::getBoundaryMode(int whichDirection)
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
void vsTextureRectangleAttribute::setApplyMode(int applyMode)
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
            printf("vsTextureRectangleAttribute::setApplyMode: Bad apply "
                "mode value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the application mode of the texture
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getApplyMode()
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
// Sets the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setGenMode(int genMode)
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
            printf("vsTextureRectangleAttribute::setGenMode: Bad generation "
                "mode value\n");
            return;
    }
}

// ------------------------------------------------------------------------
// Retrieves the texture coordinate generation mode of the texture
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::getGenMode()
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
// Return the set texture unit for this textureAttribute.
// ------------------------------------------------------------------------
unsigned int vsTextureRectangleAttribute::getTextureUnit()
{
    return textureUnit;
}

// ------------------------------------------------------------------------
// Internal Function
// Set up the traversal functions and data for the attached node's draw
// process traversal, enabling it for the rectangular texturing.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::attach(vsNode *node)
{
    pfNode *performerNode;

    // Grab the base pfNode-type object from the current vsNode
    performerNode = node->getBaseLibraryObject();

    // Use the custom traversal functions on this node during the draw process
    performerNode->setTravFuncs(PFTRAV_DRAW,
        vsTextureRectangleAttribute::preTravFunc,
        vsTextureRectangleAttribute::postTravFunc);

    // Ensure that the traversal has the structure with the texture data
    performerNode->setTravData(PFTRAV_DRAW, textureData);

    // Signal that the texture data must be reloaded
    textureData->dirty = true;

    // Account for the new node that this attribute is attached to
    attachedCount++;
}

// ------------------------------------------------------------------------
// Internal Function
// Remove the traversal functions and data for the attached node's draw
// process traversal, returning it to its previous state.
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::detach(vsNode *node)
{
    pfNode *performerNode;

    // Grab the base pfNode-type object from the current vsNode
    performerNode = node->getBaseLibraryObject();

    // Go back to using the default draw process traversals on this node
    performerNode->setTravFuncs(PFTRAV_DRAW, NULL, NULL);

    // Ensure that the attribute accounts for one fewer node
    attachedCount--;
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
// Saves the current attribute
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::saveCurrent()
{
    vsGraphicsState *gState;

    // Get the current vsGraphicsState object
    gState = vsGraphicsState::getInstance();

    // Save the current texture state in our save list, taking into account
    // that the texture may be of many different types
    if (gState->getTexture(textureUnit))
        attrSaveList[attrSaveCount] = gState->getTexture(textureUnit);
    else if (gState->getTextureCube(textureUnit))
        attrSaveList[attrSaveCount] = gState->getTextureCube(textureUnit);
    else if (gState->getTextureRect(textureUnit))
        attrSaveList[attrSaveCount] = gState->getTextureRect(textureUnit);
    else
        attrSaveList[attrSaveCount] = NULL;

    // Move to the next item in the save list
    attrSaveCount++;
}

// ------------------------------------------------------------------------
// Internal function
// Sets the current attribute to this one
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::apply()
{
    vsGraphicsState *gState;

    // Get the current vsGraphicsState object
    gState = vsGraphicsState::getInstance();

    // Set the current texture state to this object
    gState->setTextureRect(textureUnit, this);

    // Lock the texture state if overriding is enabled
    if (overrideFlag)
        gState->lockTexture(textureUnit, this);
}

// ------------------------------------------------------------------------
// Internal function
// Restores the current attribute to the last saved one
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::restoreSaved()
{
    vsGraphicsState *gState;
    vsStateAttribute *previousState;

    // Get the current vsGraphicsState object
    gState = vsGraphicsState::getInstance();

    // Unlock the texture if overriding was enabled
    if (overrideFlag)
        gState->unlockTexture(textureUnit, this);

    // Extract the previous state from the save list
    attrSaveCount--;
    previousState = (vsStateAttribute *)(attrSaveList[attrSaveCount]);

    // Set the current graphics state to that of the previous state based
    // on its specific type
    if (previousState == NULL)
    {
        // The previous texture was null, so affect this upon the current state
        gState->setTexture(textureUnit, NULL);
    }
    else if (previousState->
        getAttributeType() == VS_ATTRIBUTE_TYPE_TEXTURE_RECTANGLE)
    {
        // Set the previous state as a texture rectangle
        gState->setTextureRect(
            textureUnit, (vsTextureRectangleAttribute *) previousState);
    }
    else if (previousState->getAttributeType() == VS_ATTRIBUTE_TYPE_TEXTURE_CUBE)
    {
        // Set the previous state as a texture cube
        gState->setTextureCube(textureUnit,
            (vsTextureCubeAttribute *) previousState);
    }
    else if (previousState->getAttributeType() == VS_ATTRIBUTE_TYPE_TEXTURE)
    {
        // Set the previous state as a standard texture
        gState->setTexture(textureUnit, (vsTextureAttribute *)previousState);
    }

    // Signal that the texture data must be reloaded
    textureData->dirty = true;
}

// ------------------------------------------------------------------------
// Internal function
// Applies the settings in this attribute to the graphics library
// ------------------------------------------------------------------------
void vsTextureRectangleAttribute::setState(pfGeoState *state)
{
    // Set textures as enabled and set our Performer texture objects
    // on the geostate
    state->setMultiMode(PFSTATE_ENTEXTURE, textureUnit, PF_ON);
    state->setMultiAttr(PFSTATE_TEXENV, textureUnit, performerTexEnv);
    if (performerTexGen)
    {
        state->setMultiMode(PFSTATE_ENTEXGEN, textureUnit, PF_ON);
        state->setMultiAttr(PFSTATE_TEXGEN, textureUnit, performerTexGen);
    }
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
    attr = (vsTextureRectangleAttribute *)attribute;

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

    // Texture unit check
    val1 = getTextureUnit();
    val2 = attr->getTextureUnit();
    if (val1 != val2)
        return false;

    // Attributes are equivalent if all checks pass
    return true;
}

// ------------------------------------------------------------------------
// Static private function
// Is responsible for determining available GL extensions for rectangular
// texturing, storing the old texture state, and loading texture rectangle
// data for its node.
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::preTravFunc(pfTraverser *trav, void *data)
{
    vsTextureRectangleData *textureData;
    char *extensionString;
    char *token;

    // Extract the data structure from shared memory
    textureData = (vsTextureRectangleData *)data;

    // The sentinel value of -1 indicates an uninitialized texture structure
    if (textureData->target == -1)
    {
        // Grab the extensions string
        extensionString = (char *)glGetString(GL_EXTENSIONS);

        // Only continue if the extensions string is valid
        if (extensionString)
        {
            // Move through the extensions string token by token, until the
            // string is exhausted or one of the texture rectangle extensions
            // is located
            token = strtok(extensionString, " ");
            while (token != NULL)
            {
                // Check for the ARB texture rectangle extension
                #ifdef GL_ARB_texture_rectangle
                    if ((textureData->target == -1) &&
                        (strcmp(token, "GL_ARB_texture_rectangle") == 0))
                    {
                        textureData->target = GL_TEXTURE_RECTANGLE_ARB;
                    }
                #endif

                // Check for the NV texture rectangle extension
                #ifdef GL_NV_texture_rectangle
                    if ((textureData->target == -1) &&
                        (strcmp(token, "GL_NV_texture_rectangle") == 0))
                    {
                        textureData->target = GL_TEXTURE_RECTANGLE_NV;
                    }
                #endif
                
                // Check for the multitexturing, first by checking if the
                // OpenGL version is 1.3 or greater, second by checking for
                // the ARB_multitexture extension
                #ifdef GL_VERSION_1_3
                    textureData->multitexture = true;
                #else
                    #ifdef GL_ARB_multitexture
                        if (strcmp(token, "GL_ARB_multitexture") == 0)
                            textureData->multitexture = true;

                        // Under Windows, we have to query for the functions 
                        // we need                        
                        #ifdef WIN32
                            textureData->glActiveTextureARB = 
                                (PFNGLACTIVETEXTUREARBPROC)
                                    wglGetProcAddress("glActiveTextureARB");
                        #endif
                    #endif
                #endif

                // Move to the next token
                token = strtok(NULL, " ");
            }
        }

        // If a texture rectangle extension could not be found, default to
        // standard 2D texturing
        if (textureData->target == -1)
        {
            textureData->target = GL_TEXTURE_2D;
        }

        // Create a texture name for the texture that will be applied to
        // the node's child geometry
        glGenTextures(1, &textureData->name);
    }

    // Make sure we're talking to the right texture unit
    #ifdef GL_VERSION_1_3
        if (textureData->multitexture)
            glActiveTexture(GL_TEXTURE0 + textureData->unit);
    #else 
        #ifdef GL_ARB_multitexture
            #ifdef WIN32
                if (textureData->multitexture)
                    textureData->glActiveTextureARB(GL_TEXTURE0_ARB + 
                        textureData->unit);          
            #else
                if (textureData->multitexture)
                    glActiveTextureARB(GL_TEXTURE0_ARB + textureData->unit);
            #endif
        #endif
    #endif

    // If we're using plain 2D texturing, skip the check to see if the
    // special texture targets are enabled
    if (textureData->target != GL_TEXTURE_2D)
    {
        // See if there is already a texture rectangle target enabled
        textureData->enabledFlag = glIsEnabled(textureData->target);
        if (!textureData->enabledFlag)
        {
            // If the target isn't enabled, enable it now
            glEnable(textureData->target);
        }
    }

    // Skip the next steps if we don't have valid texture data yet
    if (textureData->data)
    {
        // Remember the name of the texture currently bound to this target
        // and bind the one we created in its place
        glGetIntegerv(textureData->target, &textureData->oldName);
        glBindTexture(textureData->target, textureData->name);

        // Only reload the data if it has been modified
        if (textureData->dirty)
        {
            // Copy the texture data to the texture target, avoiding any MIP-
            // mapping or borders (these aren't supported by texture rectangle)
            glTexImage2D(textureData->target, 0, textureData->internalFormat,
                textureData->width, textureData->height, 0, textureData->format,
                textureData->type, textureData->data);

            // The data has been loaded, so set the dirty flag to false
            textureData->dirty = false;
        }
    }

    // Continue the draw traversal to this node's children
    return PFTRAV_CONT;
}

// ------------------------------------------------------------------------
// Static private function
// Restores the original texture state after the traversal has finished
// ------------------------------------------------------------------------
int vsTextureRectangleAttribute::postTravFunc(pfTraverser *trav, void *data)
{
    vsTextureRectangleData *textureData;

    // Extract the data structure from shared memory
    textureData = (vsTextureRectangleData *)data;

    // Make sure we're talking to the right texture unit
    #ifdef GL_VERSION_1_3
        if (textureData->multitexture)
            glActiveTexture(GL_TEXTURE0 + textureData->unit);
    #else
        #ifdef GL_ARB_multitexture
            #ifdef WIN32
                if (textureData->multitexture)
                    textureData->glActiveTextureARB(GL_TEXTURE0_ARB + 
                        textureData->unit);
            #else
                if (textureData->multitexture)
                    glActiveTextureARB(GL_TEXTURE0_ARB + textureData->unit);
            #endif
        #endif
    #endif

    // If we have valid texture data, revert to the old texture
    // that was bound on this target previously
    if (textureData->data)
        glBindTexture(textureData->target, textureData->oldName);

    // Skip the next step if we're using plain 2D texturing
    if (textureData->target != GL_TEXTURE_2D)
    {
        // If we had to enable the texture target previously,
        // disable it again now
        if (!textureData->enabledFlag)
            glDisable(textureData->target);
    }

    // Continue the draw traversal on this node 
    return PFTRAV_CONT;
}

