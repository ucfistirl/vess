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
//                  a given font and with the given color.  This class is
//                  NOT thread safe because of the static data and
//                  functions.  Only one iteration of the buildText
//                  function can safely be executed at a time.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsTextBuilder.h++"

// Declare the static variables.
vsGrowableArray  *vsTextBuilder::vertexArray = new vsGrowableArray(60, 9);
vsComponent      *vsTextBuilder::letterComponent;
vsGeometry       *vsTextBuilder::primitiveGeometry;
vsVector         vsTextBuilder::currentColor;
double           vsTextBuilder::letterOffset;
int              vsTextBuilder::primitiveLength;

vsGrowableArray  *vsTextBuilder::combinedVertices = new vsGrowableArray(50, 10);
int              vsTextBuilder::combinedVertexCount;

// ------------------------------------------------------------------------
// Default Constructor - Sets the font size to the defaults, and sets the
// color to white.  Must still set the font.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder()
{
    // Initialize state variables to indicate the builder is not ready
    // to build any text.
    fontLoaded = false;
    error = false;
    initialized = false;

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the default of white.
    setColor(vsVector(1.0, 1.0, 1.0, 1.0));

    // Set the size to the defined defaults.
    setSize(VS_DEFAULT_FONT_POINT_SIZE, VS_DEFAULT_FONT_RESOLUTION);

    // Initialize the transform to the identity so it does not alter
    // the appearance of the text.
    transformMatrix.setIdentity();

    // Set the osg scale matrix.  This matrix attempts to scale down the text
    // so it matches with the Performer size.
    osgScaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font, sets the font size to the
// defaults, and sets the color to white.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont)
{
    // Initialize state variables to indicate the builder is not ready
    // to build any text.
    fontLoaded = false;
    error = false;
    initialized = false;

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the default of white.
    setColor(vsVector(1.0, 1.0, 1.0, 1.0));

    // Attempt to set the font to the given one.
    setFont(newFont);

    // Set the size to the defined defaults.
    setSize(VS_DEFAULT_FONT_POINT_SIZE, VS_DEFAULT_FONT_RESOLUTION);

    // Initialize the transform to the identity so it does not alter
    // the appearance of the text.
    transformMatrix.setIdentity();

    // Set the osg scale matrix.  This matrix attempts to scale down the text
    // So it matches with the Performer size.
    osgScaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font and sets the color to given color.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont, vsVector newColor)
{
    // Initialize state variables to indicate the builder is not ready
    // to build any text.
    fontLoaded = false;
    error = false;
    initialized = false;

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the given new color.
    setColor(newColor);

    // Attempt to set the font to the given one.
    setFont(newFont);

    // Set the size to the defined defaults.
    setSize(VS_DEFAULT_FONT_POINT_SIZE, VS_DEFAULT_FONT_RESOLUTION);

    // Initialize the transform to the identity so it does not alter
    // the appearance of the text.
    transformMatrix.setIdentity();

    // Set the osg scale matrix.  This matrix attempts to scale down the text
    // So it matches with the Performer size.
    osgScaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font and sets the color to given color.
// Also set the transform that will be automatrically applied to the text.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont, vsVector newColor,
                             vsMatrix newTransform)
{
    // Initialize state variables to indicate the builder is not ready
    // to build any text.
    fontLoaded = false;
    error = false;
    initialized = false;

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the given new color.
    setColor(newColor);

    // Attempt to set the font to the given one.
    setFont(newFont);

    // Set the size to the defined defaults.
    setSize(VS_DEFAULT_FONT_POINT_SIZE, VS_DEFAULT_FONT_RESOLUTION);

    // Initialize the transform to the given matrix.
    setTransformMatrix(newTransform);

    // Set the osg scale matrix.  This matrix attempts to scale down the text
    // So it matches with the Performer size.
    osgScaleMatrix.setScale(VS_OSG_TEXT_SCALE, VS_OSG_TEXT_SCALE,
        VS_OSG_TEXT_SCALE);
}

// ------------------------------------------------------------------------
// Destructor - There is nothing that needs to be done in this destructor.
// ------------------------------------------------------------------------
vsTextBuilder::~vsTextBuilder()
{
}

// ------------------------------------------------------------------------
// Attempts to load the font, if there is an error it will print out a
// message.
// ------------------------------------------------------------------------
void vsTextBuilder::setFont(char *newFont)
{
    // If we have already loaded a font.
    if (fontLoaded)
    {
        // Close the loaded font.
        face.Close();

        // Set this object to not initialized, which means it cannot build
        // text.
        fontLoaded = false;
        initialized = false;
    }

    // Attempt to open, if the return value is false, then there was an error.
    if (!face.Open(newFont))
    {
        // Print an error message to indicate the font was not loaded.
        printf("vsTextBuilder::setFont: Unable to Open font: %s\n", newFont);

        // Set this object to not initialized, which means it cannot build
        // text.
        fontLoaded = false;
        initialized = false;
    }
    // Else the font was loaded
    else
    {
        // Specify that the font was loaded.
        fontLoaded = true;

        // If there are no errors.
        if (!error)
        {
            // Set the size.  This is done to insure the new loaded font
            // is configured to the proper size.
            setSize(pointSize, resolution);
        }
    }
}

// ------------------------------------------------------------------------
// Set the color of this objects text.
// ------------------------------------------------------------------------
void vsTextBuilder::setColor(vsVector newColor)
{
    color = newColor;
}

// ------------------------------------------------------------------------
// Set the point size and resolution of the font.
// ------------------------------------------------------------------------
void vsTextBuilder::setSize(unsigned int newPointSize,
                            unsigned int newResolution)
{
    // Store the given values.
    pointSize = newPointSize;
    resolution = newResolution;

    // Only attempt to set the size if we have a font loaded.
    if (fontLoaded)
    {
        // Set its size to the stored values.
        face.Size(pointSize, resolution);

        // If we have an error.
        if (face.Error())
        {
            // Print an error message. 
            printf("vsTextBuilder::setSize: Error occured after trying to set
                font size to: %d, %d\n", pointSize, resolution);

            // Set this object to not initialized, which means it cannot build
            // text.  Specify there was an error as well.
            error = true;
            initialized = false;
        }
        // Else there were no problems.
        else
        {
            // Set the error flag to false, and specify that the builder is
            // ready to build text.
            error = false;
            initialized = true;
        }
    }
}

// ------------------------------------------------------------------------
// Set the local transform matrix to the given one.  This matrix is
// given to a transform attribute that is attached to all text components.
// ------------------------------------------------------------------------
void vsTextBuilder::setTransformMatrix(vsMatrix newTransform)
{
    transformMatrix = newTransform;
}

// ------------------------------------------------------------------------
// Set the justification that will be used when rendering the text.
// ------------------------------------------------------------------------
void vsTextBuilder::setJustification(int newJustification)
{
    // Insure the new justification mode is valid, and set it if it is.
    switch (newJustification)
    {
        case VS_TEXTBUILDER_JUSTIFY_LEFT:
        case VS_TEXTBUILDER_JUSTIFY_RIGHT:
        case VS_TEXTBUILDER_JUSTIFY_CENTER:
            justification = newJustification;
            break;
        default:
            printf("void vsTextBuilder::setJustification: Unknown justification
                mode.\n");
            break;
    }
}

// ------------------------------------------------------------------------
// Create and return a subgraph that describes how to draw the given text
// with the specified font, color, and size.
// ------------------------------------------------------------------------
vsComponent *vsTextBuilder::buildText(char *text)
{
    vsTransformAttribute  *textTransform;
    vsComponent           *textComponent;
    vsComponent           *textRoot;
    FT_Glyph              *glyph;
    int                   stringLength;
    int                   letter;
    int                   charIndex;

    // Get the length of the given string.
    stringLength = strlen(text);

    // Initialize the textComponent to NULL.
    textComponent = NULL;

    // If this text builder object is not fully initialized, return NULL.
    if (!initialized)
    {
        return(textComponent);
    }

    // Set the static color variable to this objects color.
    currentColor = color;

    // Reinitialize the static offset variable to 0.
    letterOffset = 0;

    // Go through each letter and add up how much space they take up.
    for (letter = 0; letter < stringLength; letter++)
    {
        // Get the index into this loaded font for the current letter.
        charIndex = face.CharIndex(text[letter]);

        // Get the freetype glyph of the current letter.
        glyph = face.Glyph(charIndex, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP);

        // If there was any error with the face, print error.
        if (face.Error())
        {
            printf("vsTextBuilder::buildText: Error occured trying to get
                glyph: %d\n", charIndex);
        }
        // Else tesselate the glyph.
        else
        {
            // Get the amount of space on the x axis that this glyph will take
            // place, as specified by the font.  Add it to the offset so
            // we end up with the total length of the text when the for loop
            // finishes.
            letterOffset += (double) ((*glyph)->advance.x >> 16);

            // Destroy the glyph.
            FT_Done_Glyph((*glyph));
        }
    }

    // Set up the offset to reflect what justification we are using.
    switch (justification)
    {
        case VS_TEXTBUILDER_JUSTIFY_LEFT:
            // If we are doing left justification, it is like that by default,
            // So simply set the offset to 0.
            letterOffset = 0;
            break;
        case VS_TEXTBUILDER_JUSTIFY_RIGHT:
            // If we are doing right justification, then insure we end
            // at 0 when we are doing drawing the text.  This is done by
            // starting to draw letters at the inverse of their total length.
            letterOffset = - letterOffset;
            break;
        case VS_TEXTBUILDER_JUSTIFY_CENTER:
            // If we are centering the text, then simply offset the initial
            // letter to begin at negative half the length of the string.
            letterOffset = - (letterOffset / 2.0);
            break;
    }

    // Go through each letter of the string to tesselate and add it to the
    // textComponent.
    for (letter = 0; letter < stringLength; letter++)
    {
        // Reinitialize the static letterComponent to NULL, we are building
        // a new letter every itteration.
        letterComponent = NULL;

        // Get the index into this loaded font for the current letter.
        charIndex = face.CharIndex(text[letter]);

        // Get the freetype glyph of the current letter.
        glyph = face.Glyph(charIndex, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP);

        // If there was any error with the face, print error.
        if (face.Error())
        {
            printf("vsTextBuilder::buildText: Error occured trying to get
                glyph: %d\n", charIndex);
        }
        // Else tesselate the glyph.
        else
        {
            // Store the offset of the tesselated glyph, so we can move the
            // next glyphs to the right properly.
            letterOffset += tesselateLetter(*glyph);
        }

        // If the tesselation generated a letter.
        if (letterComponent != NULL)
        {
            // If the component has not been created.
            if (textComponent == NULL)
            {
                // Create the text component.
                textComponent = new vsComponent();
            }
            // Add the letter to it.
            textComponent->addChild(letterComponent);
        }
    }

    // Create and attach a transform to this text with the stored matrix.
    textTransform = new vsTransformAttribute();
    textTransform->setDynamicTransform(transformMatrix);

    // Set the post transform to the osg scale matrix.  This will cause the
    // text to be scaled after the transform is applied.
    textTransform->setPostTransform(osgScaleMatrix);

    // Give the component the transform attribute.
    textComponent->addAttribute(textTransform);

    // Create a root node with no attributes attached to allow for the
    // user to add attributes easier and not step on the ones in
    // textComponent.
    textRoot = new vsComponent();
    textRoot->addChild(textComponent);

    // Return the root with the text component we completed.
    return(textRoot);
}

// ------------------------------------------------------------------------
// Tesselate Callback - Called if the tesselator encounters an error,
// simply prints out the error now.
// ------------------------------------------------------------------------
void CALLBACK vsTextBuilder::tesselateError(GLenum type)
{
    printf("vsTextComponent::tesselateError: %d\n", type);
}

// ------------------------------------------------------------------------
// Tesselate Callback - Called to create a vertex.
// ------------------------------------------------------------------------
void CALLBACK vsTextBuilder::tesselateVertex(void *vertexData)
{
    vsVector *vertex;

    // Create a VESS vector from the data.  Give z the y values so
    // the text is on the x and z axis like Performer's text.
    // These are two dimensional polygons so what would normally be z is 0.
    // Therefore y will be zero, in order to have the text drawn in the X,Z
    // plane.
    vertex = new vsVector(((double *)vertexData)[0] + letterOffset,
        0.0, ((double *)vertexData)[1]);

    // Place the vertex into the vertex array.
    vertexArray->setData(primitiveLength, vertex);

    // Increment the count of primitives to reflect its place in the array.
    primitiveLength++;
}

// ------------------------------------------------------------------------
// Tesselate Callback - Called to begin creating a primitive.
// ------------------------------------------------------------------------
void CALLBACK vsTextBuilder::tesselateBegin(GLenum type)
{
    // Create the VESS geometry for this primitive.
    primitiveGeometry = new vsGeometry();

    // Re-initialize the primitive length to 0.
    primitiveLength = 0;

    // According to which OpenGL primitive we get, set up the vsGeometry.
    switch(type)
    {
        case GL_POINTS:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_POINTS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_LINES:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINES);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_LINE_STRIP:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINE_STRIPS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_LINE_LOOP:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINE_LOOPS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_TRIANGLES:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRIS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_TRIANGLE_STRIP:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_STRIPS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_TRIANGLE_FAN:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_FANS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_QUADS:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_QUAD_STRIP:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUAD_STRIPS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
        case GL_POLYGON:
            primitiveGeometry->setPrimitiveType(VS_GEOMETRY_TYPE_POLYS);
            primitiveGeometry->setPrimitiveCount(1);
            break;
    }
}

// ------------------------------------------------------------------------
// Tesselate Callback - Called to end the creation of a primitive.
// ------------------------------------------------------------------------
void CALLBACK vsTextBuilder::tesselateEnd()
{
    int loop;

    // Setup the geometry to accept all the vertices we have gathered.
    primitiveGeometry->setDataListSize(VS_GEOMETRY_VERTEX_COORDS,
        primitiveLength);
    primitiveGeometry->setPrimitiveLength(0, primitiveLength);

    // Set all the vertices we have gathered.
    for (loop = 0; loop < primitiveLength; loop++)
    {
        primitiveGeometry->setData(VS_GEOMETRY_VERTEX_COORDS, loop,
            *((vsVector *)vertexArray->getData(loop)));

        // After the data has been copied to the geometry, we don't need the
        // the vector that was holding it.
        delete ((vsVector *)(vertexArray->getData(loop)));
    }

    // Set the geometry to the color we have stored.
    primitiveGeometry->setBinding(VS_GEOMETRY_COLORS, VS_GEOMETRY_BIND_OVERALL);
    primitiveGeometry->setDataListSize(VS_GEOMETRY_COLORS, 1);
    primitiveGeometry->setData(VS_GEOMETRY_COLORS, 0, currentColor);

    // Give the geometry a normal which is perpendicular to its plane.
    primitiveGeometry->setBinding(VS_GEOMETRY_NORMALS,
        VS_GEOMETRY_BIND_OVERALL);
    primitiveGeometry->setDataListSize(VS_GEOMETRY_NORMALS, 1);
    primitiveGeometry->setData(VS_GEOMETRY_NORMALS, 0,
        vsVector(0.0, -1.0, 0.0));

    // Add the geometry to the letterComponent now that we are done with it.
    letterComponent->addChild(primitiveGeometry);
}

// ------------------------------------------------------------------------
// Tesselate Callback - Called to combine two vertices. 
// ------------------------------------------------------------------------
void CALLBACK vsTextBuilder::tesselateCombine(GLdouble coords[3],
    void *vertex_data[4], GLfloat weight[4], void **outData)
{
    double *vertex;

    // Allocate the new vertex.
    vertex = (double *) malloc(sizeof(double)*3);

    // Copy the coords data into the newly allocated vertex.
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];

    // Set the outData to point to the newly allocated vertex.
    *outData = vertex;

    // Keep track of the allocated vertex in our list so that we can
    // delete it later
    combinedVertices->setData(combinedVertexCount++, vertex);
}

// ------------------------------------------------------------------------
// This is the main tesselate call.  It accepts a glyph (letter) and
// tesselates it to generate the geometry needed to draw it in a 3D
// scene.
// ------------------------------------------------------------------------
double vsTextBuilder::tesselateLetter(FT_Glyph glyph)
{
    int            loop;
    int            sloop;
    int            dataOffset;
    GLUtesselator  *tobj;
    FTVectoriser   *vectoriser;
    double         *data;
    int            *contourLength;
    int            contourCount;
    int            pointCount;
    int            contourFlag;
    double         advance;

    // Initialize the advance to nothing.
    advance = 0.0;

    // Setup the glyph information for tesselation.
    // If the glyph format is not an outline, do not attempt to tesselate it.
    if (ft_glyph_format_outline != glyph->format)
    {
        // Destroy the glyph.
        FT_Done_Glyph(glyph);
        return(advance);
    }

    // Get the amount of space on the x axis that this glyph will take place,
    // as specified by the font.
    advance = (double) (glyph->advance.x >> 16);

    // Create the vectoriser for the given glyph.
    vectoriser = new FTVectoriser(glyph);

    // Have the vectoriser process it.
    vectoriser->Process();

    // Get the contourCount and return if it is less than 1.
    contourCount = vectoriser->contours();
    if (contourCount < 1)
    {
        // Destroy the glyph.
        FT_Done_Glyph(glyph);
        return(advance);
    }

    // Get the pointCount and return if it is less than 3.
    pointCount = vectoriser->points();
    if (pointCount < 3)
    {
        // Destroy the glyph.
        FT_Done_Glyph(glyph);
        return(advance);
    }

    // Allocate the space for the contour length array.
    contourLength = (int *) calloc(sizeof(int), contourCount);

    // Populate the array with the length of each contour.
    for(loop = 0; loop < contourCount; ++loop)
    {
        contourLength[loop] = vectoriser->contourSize(loop);
    }

    // Allocate the space for the point data array.
    data = (double *) calloc(sizeof(double), (pointCount * 3));

    // Populate the point data array with the points.
    vectoriser->MakeOutline(data);

    // Get the contourFlag, this specifies what kind of winding we use when
    // we tesselate the glyph.
    contourFlag = vectoriser->ContourFlag();

    // Delete the vectoriser, we do not need it anymore.
    delete vectoriser;

    // Create a new static letter component.
    letterComponent = new vsComponent();

    // Setup tesselation and begin.
    dataOffset = 0;
    tobj = gluNewTess();

    // Set the tesselate callbacks.
    gluTessCallback(tobj, GLU_TESS_BEGIN, (void (CALLBACK*)())tesselateBegin);
    gluTessCallback(tobj, GLU_TESS_VERTEX, (void (CALLBACK*)())tesselateVertex);
    gluTessCallback(tobj, GLU_TESS_COMBINE,
        (void (CALLBACK*)())tesselateCombine);
    gluTessCallback(tobj, GLU_TESS_END, (void (CALLBACK*)())tesselateEnd);
    gluTessCallback(tobj, GLU_TESS_ERROR, (void (CALLBACK*)())tesselateError);

    // If the font uses the even-odd winding rule, then set the tesselator
    // to do the same.
    if (contourFlag & ft_outline_even_odd_fill)
    {
        gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
    }
    // Else we must be using the non-zero winding rule.
    else
    {
        gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    }

    // Set the tolerence to combine tesselated vertices that are at within
    // the specified (0) distance.
    gluTessProperty(tobj, GLU_TESS_TOLERANCE, 0);

    // Set the normal that controls the winding direction of generated
    // polygons.
    gluTessNormal(tobj, 0.0, 0.0, 1.0);

    // Initialize calls to tesselateCombine to zero
    combinedVertexCount = 0;

    // Begin a polygon.
    gluTessBeginPolygon(tobj, NULL);

        // Process each contour.
        for (loop = 0; loop < contourCount; ++loop)
        {
            // Begin processing the contour.
            gluTessBeginContour(tobj);

                // Process each vertex of the contour.
                for (sloop = 0; sloop < contourLength[loop]; ++sloop)
                {
                    gluTessVertex(tobj, data + dataOffset, data + dataOffset);

                    // Add three to the data offset to skip the current
                    // processed x, y, z values.
                    dataOffset += 3;
                }

            // End the contour.
            gluTessEndContour(tobj);
        }

    // End the polygon.
    gluTessEndPolygon(tobj);

    // Delete memory created by the tesselateCombine callback
    for (loop = 0; loop < combinedVertexCount; loop++)
        free(combinedVertices->getData(loop));

    // Delete the OpenGL tesselator.
    gluDeleteTess(tobj);

    // Free the allocated space.
    free(data);
    free(contourLength);

    // Free the FreeType glyph object
    FT_Done_Glyph(glyph);

    // Return the amount of space this glyph takes in the X axis.
    return(advance);
}

// ------------------------------------------------------------------------
// Internal Function - Used to delete the static vertex array that this
// object needs.  Should only be called when no other textBuilders will
// be made.  Like the destructor to vsSystem.
// ------------------------------------------------------------------------
void vsTextBuilder::deleteVertexArray()
{
    // If there is a vertex array pointer, delete what it points to.
    if (vertexArray)
    {
        delete vertexArray;
        vertexArray = NULL;
    }

    // If there is a combined vertex pointer, delete what it points to.
    if (combinedVertices)
    {
        delete combinedVertices;
        combinedVertices = NULL;
    }
}
