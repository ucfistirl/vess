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
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsTextBuilder.h++"

// ------------------------------------------------------------------------
// Default Constructor - Sets the font size to the defaults, and sets the
// color to white.  Must still set the font.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder()
{
    // Create the loader that will be used to convert the
    // performer subgraph.
    loader = new vsDatabaseLoader();

    // Set the font to normal.
    font = NULL;

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the default of white.
    setColor(vsVector(1.0, 1.0, 1.0, 1.0));

    // Set the size to the defined defaults.
    setSize(VS_DEFAULT_FONT_POINT_SIZE, VS_DEFAULT_FONT_RESOLUTION);

    // Initialize the transform to the identity so it does not alter
    // the appearance of the text.
    transformMatrix.setIdentity();
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font, sets the font size to the
// defaults, and sets the color to white.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont)
{
    // Create the loader that will be used to convert the
    // performer subgraph.
    loader = new vsDatabaseLoader();

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
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font, sets the font size to the
// specified values, and sets the color to white.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont, unsigned int newPointSize,
                             unsigned int newResolution)
{
    // Create the loader that will be used to convert the
    // performer subgraph.
    loader = new vsDatabaseLoader();

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the default of white.
    setColor(vsVector(1.0, 1.0, 1.0, 1.0));

    // Attempt to set the font to the given one.
    setFont(newFont);

    // Set the size to the given size.
    setSize(newPointSize, newResolution);

    // Initialize the transform to the identity so it does not alter
    // the appearance of the text.
    transformMatrix.setIdentity();
}

// ------------------------------------------------------------------------
// Constructor - Loads the specified font, sets the font size to the
// specified values, and sets the color to given color.
// ------------------------------------------------------------------------
vsTextBuilder::vsTextBuilder(char *newFont, unsigned int newPointSize,
                             unsigned int newResolution, vsVector newColor)
{
    // Create the loader that will be used to convert the
    // performer subgraph.
    loader = new vsDatabaseLoader();

    // Set the justification to the default centered.
    setJustification(VS_TEXTBUILDER_JUSTIFY_CENTER);

    // Set the color to the given new color.
    setColor(newColor);

    // Attempt to set the font to the given one.
    setFont(newFont);

    // Set the size to the given size.
    setSize(newPointSize, newResolution);

    // Initialize the transform to the identity so it does not alter
    // the appearance of the text.
    transformMatrix.setIdentity();
}

// ------------------------------------------------------------------------
// Destructor - Free the allocated variables used in this object.
// ------------------------------------------------------------------------
vsTextBuilder::~vsTextBuilder()
{
    // If we have a valid font pointer, delete it.
    if (font)
    {
        pfDelete(font);
    }

    // Delete the loader.
    delete loader;
}

// ------------------------------------------------------------------------
// Attempts to load the font, if there is an error it will print out a
// message.
// ------------------------------------------------------------------------
void vsTextBuilder::setFont(char *newFont)
{
    // If we have already loaded a font, delete it.
    if (font)
    {
        pfDelete(font);
        font = NULL;
    }

    // Attempt to open, if the return value is NULL, then there was an error.
    if ((font = pfdLoadFont_type1(newFont, PFDFONT_FILLED)) == NULL)
    {
        // Print an error message to indicate the font was not loaded.
        fprintf(stderr, "Unable to Open font: %s\n", newFont);
    }
    // Else set the font loaded.
    else
    {
        // Set the size.  This is done to insure the new loaded font
        // is configured to the proper size.
        setSize(pointSize, resolution);
    }
}

// ------------------------------------------------------------------------
// Set the color of this object's text.
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

    // If we have a font loaded.
    if (font)
    {
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
            fprintf(stderr, "Unknown justification mode.\n");
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
    vsObjectMap           *nodeMap;
    vsObjectMap           *attributeMap;
    vsNode                *letterNode;
    pfString              *textString;
    pfGeode               *letterGeode;
    int                   stringLength;
    int                   letter;
    int                   charIndex;

    // Get the length of the given string.
    stringLength = strlen(text);

    // Initialize the textComponent to NULL.
    textComponent = NULL;

    // If this text builder object is does not have a font loaded, return NULL.
    if (font == NULL)
    {
        return(textComponent);
    }

    // Have performer generate the geometry for the letters in the string.
    textString = new pfString();
    textString->setString(text);

    // Tell Performer to justify the way this object is setup to.
    switch (justification)
    {
        case VS_TEXTBUILDER_JUSTIFY_LEFT:
            textString->setMode(PFSTR_JUSTIFY, PFSTR_LEFT);
            break;
        case VS_TEXTBUILDER_JUSTIFY_RIGHT:
            textString->setMode(PFSTR_JUSTIFY, PFSTR_RIGHT);
            break;
        case VS_TEXTBUILDER_JUSTIFY_CENTER:
            textString->setMode(PFSTR_JUSTIFY, PFSTR_CENTER);
            break;
    }

    // Set the font of the string and flatten the geometry.
    textString->setFont(font);
    textString->flatten();

    // Create the object maps that will be used to facilitate the convertion
    // of the performer sub-graph to the vess sub-graph.
    nodeMap = new vsObjectMap();
    attributeMap = new vsObjectMap();

    // Go through each letter of the string to create vess geometry and add
    // them to the textComponent.
    for (letter = 0; letter < stringLength; letter++)
    {
        // Create a performer geode out of the letter geoset.
        letterGeode = new pfGeode();
        letterGeode->addGSet((pfGeoSet *)textString->getCharGSet(letter));

        // Convert the performer node to a vess node.
        letterNode = loader->convertNode(letterGeode, nodeMap, attributeMap);

        // If the tesselation generated a letter.
        if (letterNode != NULL)
        {
            // If the component has not been created.
            if (textComponent == NULL)
            {
                // Create the text component.
                textComponent = new vsComponent();
            }

            // Color the subgraph with this object's color vector.
            colorGraph(letterNode);

            // Add the letter to it.
            textComponent->addChild(letterNode);
        }

// DAC - Possible memory leak?
        // Delete the geode we used to create the vsGeometry.
//        pfDelete(letterGeode);
    }

// DAC - Possible memory leak?
    // Delete the string and geo state used to create the geometry.
//    pfDelete(textString);

    // Delete the object maps that were used to aid in converting the
    // performer subgraph to the vess subgraph.
    delete nodeMap;
    delete attributeMap;

    // Create and attach a transform to this text with the stored matrix.
    textTransform = new vsTransformAttribute();
    textTransform->setDynamicTransform(transformMatrix);
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
// Private function to recursively color the geometry of a node with this
// object's color vector.  This is used to color the VESS subgraph that is
// generated with convertNode.
// ------------------------------------------------------------------------
void vsTextBuilder::colorGraph(vsNode *node)
{
    int         loop;
    int         childCount;

    switch (node->getNodeType())
    {
        // If it is a component, recurse through all its children.
        case VS_NODE_TYPE_COMPONENT:
            childCount = node->getChildCount();
            for (loop = 0; loop < childCount; loop++)
            {
                colorGraph(node->getChild(loop));
            }
            break;
        // If it is a geometry, color it.
        case VS_NODE_TYPE_GEOMETRY:
            // Color the geometry using the stored color value in this object.
            ((vsGeometry *) node)->setBinding(VS_GEOMETRY_COLORS,
                VS_GEOMETRY_BIND_OVERALL);
            ((vsGeometry *) node)->setDataListSize(VS_GEOMETRY_COLORS, 1);
            ((vsGeometry *) node)->setData(VS_GEOMETRY_COLORS, 0, color);
            break;
    }
}