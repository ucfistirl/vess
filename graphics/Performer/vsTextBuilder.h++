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

enum VS_GRAPHICS_DLL vsTextBuilderJustification
{
    VS_TEXTBUILDER_JUSTIFY_LEFT,
    VS_TEXTBUILDER_JUSTIFY_RIGHT,
    VS_TEXTBUILDER_JUSTIFY_CENTER
};

class VS_GRAPHICS_DLL vsTextBuilder : public vsObject
{
private:
    pfFont                  *font;
    vsDatabaseLoader        *loader;
    vsVector                color;
    vsMatrix                transformMatrix;
    int                     justification;

    void                    colorGraph(vsNode *node);

public:
                       vsTextBuilder();
                       vsTextBuilder(char *newFont);
                       vsTextBuilder(char *newFont, vsVector newColor);
                       vsTextBuilder(char *newFont, vsVector newColor, vsMatrix newTransform);

    virtual            ~vsTextBuilder();

    virtual const char *getClassName();

    void               setFont(char *newFont);
    void               setColor(vsVector newColor);
    void               setTransformMatrix(vsMatrix newTransform);
    void               setJustification(int newJustification);
 
    vsComponent        *buildText(char *text);
};

#endif
