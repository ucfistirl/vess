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
//    VESS Module:  vsTextBuilder.h++
//
//    Description:  Class that generates a vsComponent that is a subgraph
//                  with the geometry needed to draw the given text using
//                  a given font and with the given color.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsTextBuilder.h++"

#include "vsGeometry.h++"
#include "vsTransformAttribute.h++"
#include "vsTransparencyAttribute.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets the font size to the defaults, and sets the
// color to white.  Must still set the font.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder()
{
    int loop;

    // Start with no font
    osgFont = NULL;

    // Clear the glyph and texture arrays
    for (loop = 0; loop < 256; loop++)
    {
        osgGlyphArray[loop] = NULL;
        textureAttrArray[loop] = NULL;
    }

    // Default font color is opaque white
    fontColor.set(1.0, 1.0, 1.0, 1.0);

    // Default transformation is none
    transformMatrix.setIdentity();

    // Default font resolution is 128x128
    fontResolution = osgText::FontResolution(128, 128);

    // Default text justification is left justified
    fontJustification = VS_TEXTBUILDER_JUSTIFY_LEFT;

    // Set up the OSG scale matrix. This matrix is applied to every generated
    // text component to make its size more match that of the Performer
    // text builder.
    scaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font, sets the font size to the
// defaults, and sets the color to white.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont)
{
    int loop;

    // Default font color is opaque white
    fontColor.set(1.0, 1.0, 1.0, 1.0);

    // Default font resolution is 128x128
    fontResolution = osgText::FontResolution(128, 128);

    // Default transformation is none
    transformMatrix.setIdentity();

    // Default text justification is left justified
    fontJustification = VS_TEXTBUILDER_JUSTIFY_LEFT;

    // Set up the OSG scale matrix. This matrix is applied to every generated
    // text component to make its size more match that of the Performer
    // text builder.
    scaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);

    // Clear the glyph and texture arrays
    for (loop = 0; loop < 256; loop++)
    {
        osgGlyphArray[loop] = NULL;
        textureAttrArray[loop] = NULL;
    }

    // Fetch the desired font
    osgFont = NULL;
    setFont(newFont);
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font and sets the color to given color.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont, atVector newColor)
{
    int loop;

    // Copy the font color to our internal variable. If the size of the given
    // color vector is less than four, then make sure that the fourth element
    // of the color is one. (Text should default to opaque.)
    fontColor.setSize(4);
    fontColor.clearCopy(newColor);
    if (newColor.getSize() < 4)
        fontColor[3] = 1.0;

    // Default font resolution is 128x128
    fontResolution = osgText::FontResolution(128, 128);

    // Default transformation is none
    transformMatrix.setIdentity();

    // Default text justification is left justified
    fontJustification = VS_TEXTBUILDER_JUSTIFY_LEFT;

    // Set up the OSG scale matrix. This matrix is applied to every generated
    // text component to make its size more match that of the Performer
    // text builder.
    scaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);

    // Clear the glyph and texture arrays
    for (loop = 0; loop < 256; loop++)
    {
        osgGlyphArray[loop] = NULL;
        textureAttrArray[loop] = NULL;
    }

    // Fetch the desired font
    osgFont = NULL;
    setFont(newFont);
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font and sets the color to given color.
// Also set the transform that will be automatrically applied to the text.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont, atVector newColor,
                             atMatrix newTransform)
{
    int loop;

    // Copy the font color to our internal variable. If the size of the given
    // color vector is less than four, then make sure that the fourth element
    // of the color is one. (Text should default to opaque.)
    fontColor.setSize(4);
    fontColor.clearCopy(newColor);
    if (newColor.getSize() < 4)
        fontColor[3] = 1.0;

    // Default font resolution is 128x128
    fontResolution = osgText::FontResolution(128, 128);

    // Copy the font transformation matrix
    transformMatrix = newTransform;

    // Default text justification is left justified
    fontJustification = VS_TEXTBUILDER_JUSTIFY_LEFT;

    // Set up the OSG scale matrix. This matrix is applied to every generated
    // text component to make its size more match that of the Performer
    // text builder.
    scaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);

    // Clear the glyph and texture arrays
    for (loop = 0; loop < 256; loop++)
    {
        osgGlyphArray[loop] = NULL;
        textureAttrArray[loop] = NULL;
    }

    // Fetch the desired font
    osgFont = NULL;
    setFont(newFont);
}

// ------------------------------------------------------------------------
// Destructor - There is nothing that needs to be done in this destructor.
// ------------------------------------------------------------------------
vsTextBuilder::~vsTextBuilder()
{
    // Force the deletion of the OSG Font object through the use of the
    // setFont method.
    setFont(NULL);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTextBuilder::getClassName()
{
    return "vsTextBuilder";
}

// ------------------------------------------------------------------------
// Attempts to load the font, if there is an error it will print out a
// message.
// ------------------------------------------------------------------------
void vsTextBuilder::setFont(char *newFont)
{
    int loop;

    // If we already have a font created, delete it
    if (osgFont)
    {
        // Unreference and attempt to delete all of the vsTextureAttributes
        // that contain the character glyph textures. Those textures that
        // are still in use won't be destroyed, due to reference counting.
        for (loop = 0; loop < 256; loop++)
        {
            if (osgGlyphArray[loop])
            {
                // Get rid of the OSG Glyph object
                (osgGlyphArray[loop])->unref();
                osgGlyphArray[loop] = NULL;

                // Get rid of the corresponding VESS texture attribute
                vsObject::unrefDelete(textureAttrArray[loop]);
                textureAttrArray[loop] = NULL;
            }
        }

        // 'Delete' the font object by unreferencing it. OSG should delete
        // any objects that have no more references to them.
        osgFont->unref();

        // As a temporary action, set the font variable to NULL. This value
        // becomes permanent if we have to bail out of the font-opening
        // process at any point; a NULL font value is valid and simply means
        // that no font is currently open.
        osgFont = NULL;
    }

    // Attempt to open the new font; leave the font pointer NULL if a NULL
    // font name was specified.
    if (newFont)
    {
        // Create an OSG Font object from the data cotnained in the specified
        // font file
        osgFont = osgText::readFontFile(newFont);

        // If we have a valid Font object, reference it so that OSG doesn't
        // get tempted to delete it
        if (osgFont)
            osgFont->ref();

        // Increase the glyph image margin to 3 pixels, to avoid minification
        // artifacts (glyphs bleeding into other glyphs)
        osgFont->setGlyphImageMargin(3);

        // Error checking
        if (!osgFont)
            printf("vsTextBuilder::setFont: Error opening font file '%s'\n",
                newFont);
    }
}

// ------------------------------------------------------------------------
// Set the scale in each dimension of the text built by this object
// ------------------------------------------------------------------------
void vsTextBuilder::setScale(double xScale, double yScale, double zScale)
{
    scaleMatrix.setScale(xScale * VS_OSG_TEXT_SCALE,
        yScale * VS_OSG_TEXT_SCALE, zScale * VS_OSG_TEXT_SCALE);
}

// ------------------------------------------------------------------------
// Set the color of this objects text.
// ------------------------------------------------------------------------
void vsTextBuilder::setColor(atVector newColor)
{
    // Copy the font color to our internal variable. If the size of the given
    // color vector is less than four, then make sure that the fourth element
    // of the color is one. (Text should default to opaque.)
    fontColor.setSize(4);
    fontColor.clearCopy(newColor);
    if (newColor.getSize() < 4)
        fontColor[3] = 1.0;
}

// ------------------------------------------------------------------------
// Set the local transform matrix to the given one.  This matrix is
// given to a transform attribute that is attached to all text components.
// ------------------------------------------------------------------------
void vsTextBuilder::setTransformMatrix(atMatrix newTransform)
{
    transformMatrix = newTransform;
}

// ------------------------------------------------------------------------
// Set the justification that will be used when rendering the text.
// ------------------------------------------------------------------------
void vsTextBuilder::setJustification(int newJustification)
{
    // Insure the new justification mode is valid, and set it if it is
    switch (newJustification)
    {
        case VS_TEXTBUILDER_JUSTIFY_LEFT:
        case VS_TEXTBUILDER_JUSTIFY_RIGHT:
        case VS_TEXTBUILDER_JUSTIFY_CENTER:
            fontJustification = newJustification;
            break;

        default:
            printf("void vsTextBuilder::setJustification: Unknown justification"
                " mode.\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Return the justification mode in use by this text builder
// ------------------------------------------------------------------------
int vsTextBuilder::getJustification()
{
    return fontJustification;
}

// ------------------------------------------------------------------------
// Create and return a subgraph that describes how to draw the given text
// with the specified font, color, and size.
// ------------------------------------------------------------------------
vsComponent *vsTextBuilder::buildText(char *text)
{
    vsComponent *result;
    vsGeometry *letterGeom;
    osgText::Glyph *osgGlyph;
    vsTextureAttribute *letterTextureAttr;
    int loop;
    char previousChar;
    int lineStartIdx;
    atVector currentPos, offset, bearing;
    osg::Vec2 osgFontKerning;
    vsTransformAttribute *xformAttr;
    int charWidth, charHeight;
    vsTransparencyAttribute *transpAttr;
    int newlines;

    // If there is no currently active font, return a NULL vsComponent
    if (!osgFont)
        return NULL;

    // Create a new vsComponent to hold the characters
    result = new vsComponent();

    // The first line of characters starts at the first character
    lineStartIdx = 0;
    previousChar = 0;
    newlines = 0;

    // Start the drawing at the 'origin'
    currentPos.set(0.0, 0.0, 0.0);

    // Loop through the entire string, creating characters as we go
    for (loop = 0; loop < (int)strlen(text); loop++)
    {
        // Get the OSG Glyph and VESS texture attribute corresponding to this
        // character's value.
        osgGlyph = getOSGGlyph((unsigned char)(text[loop]));
        letterTextureAttr = getTextureAttribute((unsigned char)(text[loop]));

        // Compute the size of the character
        charWidth = osgGlyph->s() - (2 * osgFont->getGlyphImageMargin());
        charHeight = osgGlyph->t() - (2 * osgFont->getGlyphImageMargin());

        // Check for a newline character
        if (text[loop] == '\n')
        {
            // * Newline character. Justify this line, and shift the current
            // geometry placement position to the beginning of the next line.

            // Reposition the characters' geometry if the justification
            // requires it. The "- newlines" part of the expressions is meant
            // to compensate for the fact that newline characters aren't
            // actually rendered and therfore don't have geometry nodes under
            // the parent; subtracting the number of newlines encountered so
            // far accounts for the shift in numbering.
            justifyLine(result, lineStartIdx - newlines, loop - 1 - newlines,
                currentPos[AT_X]);

            // Move the draw position to the beginning of the next line
            currentPos[AT_X] = 0.0;
            currentPos[AT_Z] -= 128.0;

            // Reset the line marker
            lineStartIdx = loop+1;

            // Record that we've enountered another newline
            newlines++;
        }
        else
        {
            // If there is a previous character, then advance the draw
            // position by the inter-character amount designated by
            // the current font.
            if (loop > lineStartIdx)
            {
                // Get the inter-characer spacing between the previous
                // character and the new one
                //osgFontKerning = osgFont->getKerning(fontResolution,
                //    previousChar, text[loop], osgText::KERNING_UNFITTED);
                osgFontKerning = osgFont->getKerning(
                    (unsigned int) previousChar, (unsigned int) text[loop],
                    osgText::KERNING_UNFITTED);

                // Advance the draw position by the horizontal spacing value
                currentPos[AT_X] += osgFontKerning[0];
            }

            // * Create a geometry object to hold the character, and set up
            // the geometry data.
            letterGeom = new vsGeometry();

            // One quad
            letterGeom->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
            letterGeom->setPrimitiveCount(1);

            // Color
            letterGeom->setBinding(VS_GEOMETRY_COLORS,
                VS_GEOMETRY_BIND_OVERALL);
            letterGeom->setDataListSize(VS_GEOMETRY_COLORS, 1);
            letterGeom->setData(VS_GEOMETRY_COLORS, 0, fontColor);

            // Normal
            letterGeom->setBinding(VS_GEOMETRY_NORMALS,
                VS_GEOMETRY_BIND_OVERALL);
            letterGeom->setDataListSize(VS_GEOMETRY_NORMALS, 1);
            letterGeom->setData(VS_GEOMETRY_NORMALS, 0,
                atVector(0.0, -1.0, 0.0));

            // Vertex and texture coordinates
            letterGeom->setBinding(VS_GEOMETRY_TEXTURE_COORDS,
                VS_GEOMETRY_BIND_PER_VERTEX);
            letterGeom->setDataListSize(VS_GEOMETRY_VERTEX_COORDS, 4);
            letterGeom->setDataListSize(VS_GEOMETRY_TEXTURE_COORDS, 4);

            // calculate the bearing
            bearing.set((osgGlyph->getHorizontalBearing())[0], 0.0,
                (osgGlyph->getHorizontalBearing())[1]);

            // bottom left
            offset.set(0.0, 0.0, 0.0);
            letterGeom->setData(VS_GEOMETRY_VERTEX_COORDS, 0,
                currentPos + bearing + offset);
            letterGeom->setData(VS_GEOMETRY_TEXTURE_COORDS, 0,
                atVector(0.0, 0.0));

            // bottom right
            offset.set(charWidth, 0.0, 0.0);
            letterGeom->setData(VS_GEOMETRY_VERTEX_COORDS, 1,
                currentPos + bearing + offset);
            letterGeom->setData(VS_GEOMETRY_TEXTURE_COORDS, 1,
                atVector(1.0, 0.0));

            // top right
            offset.set(charWidth, 0.0, charHeight);
            letterGeom->setData(VS_GEOMETRY_VERTEX_COORDS, 2,
                currentPos + bearing + offset);
            letterGeom->setData(VS_GEOMETRY_TEXTURE_COORDS, 2,
                atVector(1.0, 1.0));

            // top left
            offset.set(0.0, 0.0, charHeight);
            letterGeom->setData(VS_GEOMETRY_VERTEX_COORDS, 3,
                currentPos + bearing + offset);
            letterGeom->setData(VS_GEOMETRY_TEXTURE_COORDS, 3,
                atVector(0.0, 1.0));

            // Get the texture for the character and attach it to the geometry
            letterTextureAttr =
                getTextureAttribute((unsigned char)(text[loop]));
            letterGeom->addAttribute(letterTextureAttr);

            // Add the new geometry to the parent component
            result->addChild(letterGeom);

            // Advance the 'draw position'
            currentPos[AT_X] +=
               osgGlyph->getHorizontalAdvance() * fontResolution.first;
        }

        // Record the current character for the next iteration
        previousChar = text[loop];
    }

    // Reposition the characters' geometry if the justification requires
    justifyLine(result, lineStartIdx - newlines, loop - 1 - newlines,
        currentPos[AT_X]);

    // Create a transform attribute for the text component
    xformAttr = new vsTransformAttribute();

    // Apply the transform and scale matricies to the attribute
    xformAttr->setDynamicTransform(transformMatrix);
    xformAttr->setPostTransform(scaleMatrix);
    result->addAttribute(xformAttr);

    // Create and apply a transparency attribute to the parent component. This
    // improves the quality of the characters, because the font rendering
    // process creates partially-translucent pixels that don't show up
    // correctly without transparency enabled.
    transpAttr = new vsTransparencyAttribute();
    transpAttr->enable();
    result->addAttribute(transpAttr);

    // Done.
    return result;
}

// ------------------------------------------------------------------------
// Private function
// Retrieves the OSG Glyph object corresponsing to the given character code
// ------------------------------------------------------------------------
osgText::Glyph *vsTextBuilder::getOSGGlyph(unsigned char ch)
{
    // Make sure that the specified Glyph exists
    if (!(osgGlyphArray[ch]))
        setupTextureAttribute(ch);

    return osgGlyphArray[ch];
}

// ------------------------------------------------------------------------
// Private function
// Retrieves the texture attribute corresponding to the given character
// code
// ------------------------------------------------------------------------
vsTextureAttribute *vsTextBuilder::getTextureAttribute(unsigned char ch)
{
    // Make sure that the specified texture attribute exists
    if (!(textureAttrArray[ch]))
        setupTextureAttribute(ch);

    return textureAttrArray[ch];
}

// ------------------------------------------------------------------------
// Private function
// Obtains the OSG Glyph and creates the VESS texture attribute
// corresponding to the given character code, if they are not already
// available.
// ------------------------------------------------------------------------
void vsTextBuilder::setupTextureAttribute(unsigned char ch)
{
    // No work to do if the character's glyph already exists
    if (osgGlyphArray[ch])
        return;

    // Obtain the Glyph for the designated character from the OSG Font object
    osgGlyphArray[ch] = osgFont->getGlyph(fontResolution, ch);
    (osgGlyphArray[ch])->ref();

    // Create the corresponding VESS texture attribute
    textureAttrArray[ch] = new vsTextureAttribute();
    (textureAttrArray[ch])->ref();

    // JPD:  If we don't do this, the text doesn't show up.  I'm trying to
    // figure out why...
    textureAttrArray[ch]->disableNonPowerOfTwo();

    // Set the new texture to use the new glyph. (Technically, this function
    // takes an OSG Image, but in this case Glyph is derived from Image so
    // everything works fine.)
    (textureAttrArray[ch])->setOSGImage(osgGlyphArray[ch]);

    // Set some of the parameters of the texture
    (textureAttrArray[ch])->setApplyMode(VS_TEXTURE_APPLY_MODULATE);
    (textureAttrArray[ch])->setBoundaryMode(VS_TEXTURE_DIRECTION_ALL,
                                            VS_TEXTURE_BOUNDARY_CLAMP);
    (textureAttrArray[ch])->setMagFilter(VS_TEXTURE_MAGFILTER_LINEAR);
    (textureAttrArray[ch])->setMinFilter(VS_TEXTURE_MINFILTER_MIPMAP_LINEAR);
}

// ------------------------------------------------------------------------
// Private function
// Applies justification to the 'line' of characters specified by the
// start and end indices, inclusive
// ------------------------------------------------------------------------
void vsTextBuilder::justifyLine(vsComponent *lineParent, int lineStartIdx,
    int lineEndIdx, double lineLength)
{
    int loop, sloop;
    vsNode *childNode;
    vsGeometry *childGeom;
    atVector vertexCoord;

    // If the current justification mode is LEFT justified, then there's no
    // work to do
    if (fontJustification == VS_TEXTBUILDER_JUSTIFY_LEFT)
        return;

    // For each vsGeometry in the range lineStartIdx-lineEndIdx of children on
    // the lineParent component, move the vertices of that geometry over as
    // specified by the justification mode.
    for (loop = lineStartIdx; loop <= lineEndIdx; loop++)
    {
        // Get the loop'th child of the component, and make sure that it's
        // a vsGeometry
        childNode = lineParent->getChild(loop);
        if (childNode->getNodeType() != VS_NODE_TYPE_GEOMETRY)
        {
            printf("vsTextBuilder::justifyLine: Non-geometry vsNode discovered"
                " on parent component\n");
            continue;
        }
        childGeom = (vsGeometry *)childNode;

        // For each vertex of this geometry, nudge the vertex to the 'left'
        for (sloop = 0; sloop < 4; sloop++)
        {
            // get
            vertexCoord = childGeom->getData(VS_GEOMETRY_VERTEX_COORDS, sloop);

            // offset
            switch (fontJustification)
            {
                case VS_TEXTBUILDER_JUSTIFY_CENTER:
                    vertexCoord[AT_X] -= (lineLength / 2.0);
                    break;

                case VS_TEXTBUILDER_JUSTIFY_RIGHT:
                    vertexCoord[AT_X] -= lineLength;
                    break;
            }

            // set
            childGeom->setData(VS_GEOMETRY_VERTEX_COORDS, sloop, vertexCoord);
        }
    }
}
