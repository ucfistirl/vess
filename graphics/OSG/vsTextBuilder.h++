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

#ifdef WIN32
    #include <windows.h>
#endif

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

#define VS_DEFAULT_FONT_POINT_SIZE  12
#define VS_DEFAULT_FONT_RESOLUTION  72

#define VS_OSG_TEXT_SCALE           0.07514f

enum VS_GRAPHICS_DLL vsTextBuilderJustification
{
    VS_TEXTBUILDER_JUSTIFY_LEFT,
    VS_TEXTBUILDER_JUSTIFY_RIGHT,
    VS_TEXTBUILDER_JUSTIFY_CENTER
};

class VS_GRAPHICS_DLL vsTextBuilder
{
private:
    static vsVector         currentColor;
    static vsComponent      *letterComponent;
    static vsGeometry       *primitiveGeometry;
    static vsGrowableArray  *vertexArray;
    static int              primitiveLength;
    static double           letterOffset;

    static vsGrowableArray  *combinedVertices;
    static int              combinedVertexCount;

    FTFace                  face;
    vsVector                color;
    vsMatrix                transformMatrix;
    vsMatrix                osgScaleMatrix;
    unsigned int            pointSize;
    unsigned int            resolution;
    int                     justification;

    bool                    fontLoaded;
    bool                    error;
    bool                    initialized;

    static void CALLBACK    tesselateError(GLenum type);
    static void CALLBACK    tesselateVertex(void *vertexData);
    static void CALLBACK    tesselateBegin(GLenum type);
    static void CALLBACK    tesselateEnd();
    static void CALLBACK    tesselateCombine(GLdouble coords[3],
                                             void *vertex_data[4],
                                             GLfloat weight[4],
                                             void **outData);

    double                  tesselateLetter(FT_Glyph glyph);

    void                    setSize(unsigned int newPointSize,
                                    unsigned int newResolution);

VS_INTERNAL:

    static void             deleteVertexArray();

public:
    vsTextBuilder();
    vsTextBuilder(char *newFont);
    vsTextBuilder(char *newFont, vsVector newColor);
    vsTextBuilder(char *newFont, vsVector newColor, vsMatrix newTransform);

    ~vsTextBuilder();

    void         setFont(char *newFont);
    void         setColor(vsVector newColor);
    void         setTransformMatrix(vsMatrix newTransform);
    void         setJustification(int newJustification);

    vsComponent  *buildText(char *text);
};

#endif
