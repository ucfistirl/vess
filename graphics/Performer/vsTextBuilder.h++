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

#ifndef VS_TEXTBUILDER_HPP
#define VS_TEXTBUILDER_HPP

#include <Performer/pf/pfText.h>
#include <Performer/pr/pfString.h>
#include <Performer/pfdu.h>

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsGrowableArray.h++"
#include "vsTransformAttribute.h++"
#include "vsDatabaseLoader.h++"

#define VS_DEFAULT_FONT_POINT_SIZE  10
#define VS_DEFAULT_FONT_RESOLUTION  72

enum vsTextBuilderJustification
{
    VS_TEXTBUILDER_JUSTIFY_LEFT,
    VS_TEXTBUILDER_JUSTIFY_RIGHT,
    VS_TEXTBUILDER_JUSTIFY_CENTER
};

class vsTextBuilder
{
private:
    // Variables to describe the text this builder creates.
    pfFont                  *font;
    vsDatabaseLoader        *loader;
    vsVector                color;
    vsMatrix                transformMatrix;
    unsigned int            pointSize;
    unsigned int            resolution;
    int                     justification;

    void                    colorGraph(vsNode *node);

public:
    // Constructors that accept different setup data.
    vsTextBuilder();
    vsTextBuilder(char *newFont);
    vsTextBuilder(char *newFont, unsigned int newPointSize,
                  unsigned int newResolution);
    vsTextBuilder(char *newFont, unsigned int newPointSize,
                  unsigned int newResolution, vsVector newColor);

    ~vsTextBuilder();

    // Set the font, color, and size.
    void         setFont(char *newFont);
    void         setColor(vsVector newColor);
    void         setSize(unsigned int newPointSize,
                         unsigned int newResolution);
    void         setTransformMatrix(vsMatrix newTransform);
    void         setJustification(int newJustification);

    // Return a vsComponent that draws the given text.
    vsComponent  *buildText(char *text);
};

#endif
