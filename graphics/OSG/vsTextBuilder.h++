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

#ifndef VS_TEXTBUILDER_HPP
#define VS_TEXTBUILDER_HPP

#ifdef WIN32
    #include <windows.h>
#endif

#include "osgText/Font"

#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsComponent.h++"
#include "vsTextureAttribute.h++"

#ifndef CALLBACK
#define CALLBACK
#endif

#define VS_DEFAULT_FONT_POINT_SIZE  12
#define VS_DEFAULT_FONT_RESOLUTION  72

//#define VS_OSG_TEXT_SCALE           0.07514f
#define VS_OSG_TEXT_SCALE           0.005f

enum VS_GRAPHICS_DLL vsTextBuilderJustification
{
    VS_TEXTBUILDER_JUSTIFY_LEFT,
    VS_TEXTBUILDER_JUSTIFY_RIGHT,
    VS_TEXTBUILDER_JUSTIFY_CENTER
};

class VS_GRAPHICS_DLL vsTextBuilder : public vsObject
{
private:

    osgText::Font           *osgFont;

    vsVector                fontColor;
    vsMatrix                transformMatrix;
    int                     fontJustification;

    vsMatrix                osgScaleMatrix;

    osgText::Font::Glyph    *osgGlyphArray[256];
    vsTextureAttribute      *textureAttrArray[256];

    osgText::Font::Glyph    *getOSGGlyph(unsigned char ch);
    vsTextureAttribute      *getTextureAttribute(unsigned char ch);

    void                    setupTextureAttribute(unsigned char ch);

    void                    justifyLine(vsComponent *lineParent,
                                        int lineStartIdx, int lineEndIdx,
                                        double lineLength);

public:

                   vsTextBuilder();
                   vsTextBuilder(char *newFont);
                   vsTextBuilder(char *newFont, vsVector newColor);
                   vsTextBuilder(char *newFont, vsVector newColor,
                                 vsMatrix newTransform);
    virtual        ~vsTextBuilder();

    virtual const char    *getClassName();

    void           setFont(char *newFont);
    void           setColor(vsVector newColor);
    void           setTransformMatrix(vsMatrix newTransform);
    void           setJustification(int newJustification);

    vsComponent    *buildText(char *text);
};

#endif
