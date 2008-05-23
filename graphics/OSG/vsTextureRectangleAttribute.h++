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
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_TEXTURE_RECTANGLE_ATTRIBUTE_HPP
#define VS_TEXTURE_RECTANGLE_ATTRIBUTE_HPP

#include <osg/TextureRectangle>
#include <osg/Texture>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/Image>
#include "vsStateAttribute.h++"
#include "vsTextureAttribute.h++"

class VS_GRAPHICS_DLL vsTextureRectangleAttribute : public vsStateAttribute
{
private:

    osg::TextureRectangle    *osgTexture;
    osg::TexEnv              *osgTexEnv;
    osg::TexEnvCombine       *osgTexEnvCombine;
    osg::TexGen              *osgTexGen;
    osg::TexMat              *osgTexMat;
    osg::Image               *osgTexImage;

    unsigned int             textureUnit;

    bool                     removeTexGen;

    virtual void             setOSGAttrModes(vsNode *node);

VS_INTERNAL:

                             vsTextureRectangleAttribute(unsigned int unit,
                                       osg::TextureRectangle *texObject,
                                       osg::TexEnv *texEnvObject,
                                       osg::TexEnvCombine *texEnvCombineObject,
                                       osg::TexGen *texGenObject,
                                       osg::TexMat *texMatObject);

    virtual void             attach(vsNode *node);
    virtual void             detach(vsNode *node);

    virtual void             attachDuplicate(vsNode *theNode);

    virtual bool             isEquivalent(vsAttribute *attribute);

    void                     setOSGImage(osg::Image *osgImage);

    osg::TextureRectangle    *getBaseLibraryObject();

public:

                           vsTextureRectangleAttribute();
                           vsTextureRectangleAttribute(unsigned int unit);
    virtual                ~vsTextureRectangleAttribute();

    virtual const char     *getClassName();
    virtual int            getAttributeType();
    virtual vsAttribute    *clone();
    
    void                   setImage(unsigned char *imageData, int xSize,
                                    int ySize, int dataFormat);
    void                   getImage(unsigned char **imageData, int *xSize,
                                    int *ySize, int *dataFormat);
    
    void                   loadImageFromFile(char *filename);

    void                   reloadTextureData();

    bool                   isTransparent();

    void                   setBoundaryMode(int whichDirection, int boundaryMode);
    int                    getBoundaryMode(int whichDirection);
    
    void                   setApplyMode(int applyMode);
    int                    getApplyMode();

    void                   setMagFilter(int newFilter);
    int                    getMagFilter();
    void                   setMinFilter(int newFilter);
    int                    getMinFilter();

    void                   setBaseColor(atVector color);
    atVector               getBaseColor();

    void                   setGenMode(int genMode);
    int                    getGenMode();

    void                   setTextureMatrix(atMatrix newMatrix);
    atMatrix               getTextureMatrix();

    void                   setTextureUnit(unsigned int unit);
    unsigned int           getTextureUnit();
};

#endif
