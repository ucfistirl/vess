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
//    VESS Module:  vsTextureRectangleAttribute.h++
//
//    Description:  Attribute that specifies which texture should be used
//                  to cover geometry. Works on textures that don't have
//                  power-of-two dimensions.
//
//    Author(s):    Bryan Kline, Casey Thurston, Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_TEXTURE_RECTANGLE_ATTRIBUTE_HPP
#define VS_TEXTURE_RECTANGLE_ATTRIBUTE_HPP

#include "vsGlobals.h++"
#include "vsGeometry.h++"
#include "vsNode.h++"
#include "vsStateAttribute.h++"
#include "vsTextureAttribute.h++"
#include <Performer/pf/pfTraverser.h>
#include <stdlib.h>
#include <string>

struct VS_GRAPHICS_DLL vsTextureRectangleData
{
    int         target;
    int         internalFormat;
    int         width;
    int         height;
    int         format;
    int         type;
    void        *data;
    int         unit;
    int         multitexture;
    int         magFilter;
    int         minFilter;

    bool        dirty;

    int         enabledFlag;
    int         oldName;
    GLuint      name;

    #if defined(WIN32) && defined(GL_ARB_multitexture)
        PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
    #endif
};

class VS_GRAPHICS_DLL vsTextureRectangleAttribute : public vsStateAttribute
{
private:

    pfTexture       *performerTexture;
    pfTexEnv        *performerTexEnv;
    pfTexGen        *performerTexGen;
    pfMatrix        textureMatrix;
    bool            textureMatrixEnabled;

    unsigned int    textureUnit;

    vsTextureRectangleData    *textureData;

    static int      preTravFunc(pfTraverser *trav, void *data);
    static int      postTravFunc(pfTraverser *trav, void *data);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                          vsTextureRectangleAttribute();
                          vsTextureRectangleAttribute(unsigned int unit);
                          vsTextureRectangleAttribute(unsigned int unit,
                              pfTexture *texObject, pfTexEnv *texEnvObject,
                              pfTexGen *texGenObject, pfMatrix texMat,
                              bool useTexMat);

    virtual               ~vsTextureRectangleAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
 
    void                  setImage(unsigned char *imageData, int xSize,
                                   int ySize, int dataFormat);
    void                  getImage(unsigned char **imageData, int *xSize,
                                   int *ySize, int *dataFormat);
 
    void                  loadImageFromFile(char *filename);

    void                  reloadTextureData();

    void                  setBoundaryMode(int whichDirection, int boundaryMode);
    int                   getBoundaryMode(int whichDirection);
 
    void                  setApplyMode(int applyMode);
    int                   getApplyMode();

    void                  setMagFilter(int newFilter);
    int                   getMagFilter();
    void                  setMinFilter(int newFilter);
    int                   getMinFilter();

    void                  setGenMode(int genMode);
    int                   getGenMode();

    void                  setTextureMatrix(atMatrix newMatrix);
    atMatrix              getTextureMatrix();

    unsigned int          getTextureUnit();
};

#endif
