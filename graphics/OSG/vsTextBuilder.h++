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


#include "vsObject.h++"
#include "atVector.h++"
#include "atMatrix.h++"
#include "vsComponent.h++"
#include "vsTextureAttribute.h++"
#include "osgText/Font"

#ifndef CALLBACK
#define CALLBACK
#endif

#define VS_DEFAULT_FONT_POINT_SIZE  12
#define VS_DEFAULT_FONT_RESOLUTION  72

//#define VS_OSG_TEXT_SCALE           0.07514f
#define VS_OSG_TEXT_SCALE           0.005f

enum  vsTextBuilderJustification
{
    VS_TEXTBUILDER_JUSTIFY_LEFT,
    VS_TEXTBUILDER_JUSTIFY_RIGHT,
    VS_TEXTBUILDER_JUSTIFY_CENTER
};

class VESS_SYM vsTextBuilder : public vsObject
{
private:

    osgText::Font             *osgFont;

    atVector                  fontColor;
    osgText::FontResolution   fontResolution;
    atMatrix                  transformMatrix;
    int                       fontJustification;

    atMatrix                  scaleMatrix;

    osgText::Font::Glyph      *osgGlyphArray[256];
    vsTextureAttribute        *textureAttrArray[256];

    osgText::Font::Glyph      *getOSGGlyph(unsigned char ch);
    vsTextureAttribute        *getTextureAttribute(unsigned char ch);

    void                      setupTextureAttribute(unsigned char ch);

    void                      justifyLine(vsComponent *lineParent,
                                          int lineStartIdx, int lineEndIdx,
                                          double lineLength);

public:

                   vsTextBuilder();
                   vsTextBuilder(char *newFont);
                   vsTextBuilder(char *newFont, atVector newColor);
                   vsTextBuilder(char *newFont, atVector newColor,
                                 atMatrix newTransform);
    virtual        ~vsTextBuilder();

    virtual const char    *getClassName();

    void           setFont(char *newFont);
    void           setScale(double xScale, double yScale, double zScale);
    void           setColor(atVector newColor);
    void           setTransformMatrix(atMatrix newTransform);
    void           setJustification(int newJustification);

    int            getJustification();

    vsComponent    *buildText(char *text);
};

#endif
