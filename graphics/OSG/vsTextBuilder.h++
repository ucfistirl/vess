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

#ifndef VS_TEXTBUILDER_HPP
#define VS_TEXTBUILDER_HPP

#include <GL/glu.h>

#include "FTVectoriser.h"
#include "FTFace.h"

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsGrowableArray.h++"
#include "vsTransformAttribute.h++"

#ifndef CALLBACK
#define CALLBACK
#endif

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
    // Static variables used by the glu tesselate callbacks.
    static vsVector         currentColor;
    static vsComponent      *letterComponent;
    static vsGeometry       *primitiveGeometry;
    static vsGrowableArray  *vertexArray;
    static int              primitiveLength;
    static double           letterOffset;

    // Variables to describe the text this builder creates.
    FTFace                  face;
    vsVector                color;
    vsMatrix                transformMatrix;
    unsigned int            pointSize;
    unsigned int            resolution;
    int                     justification;

    // Boolean variables used to insure we have all the information needed
    // to create text.
    bool                    fontLoaded;
    bool                    error;
    bool                    initialized;

    // Tesselate Callbacks.
    static void CALLBACK    tesselateError(GLenum type);
    static void CALLBACK    tesselateVertex(void *vertexData);
    static void CALLBACK    tesselateBegin(GLenum type);
    static void CALLBACK    tesselateEnd();
    static void CALLBACK    tesselateCombine(GLdouble coords[3],
                                             void *vertex_data[4],
                                             GLfloat weight[4],
                                             void **outData);

    // Tesselate the glyph and return the offset the glyph introduces.
    double                  tesselateLetter(FT_Glyph glyph);

VS_INTERNAL:

    static void             deleteVertexArray();

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
