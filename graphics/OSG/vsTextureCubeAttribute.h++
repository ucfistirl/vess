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
//    VESS Module:  vsTextureCubeAttribute.h++
//
//    Description:  Attribute that specifies a texture cube to use for
//                  effects like evironment mapping on geometry
//
//    Author(s):    Duvan Cope, Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_TEXTURE_CUBE_ATTRIBUTE_HPP
#define VS_TEXTURE_CUBE_ATTRIBUTE_HPP

#include <osg/TextureCubeMap>
#include <osg/Texture>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TexMat>
#include <osg/Image>
#include "vsStateAttribute.h++"
#include "vsTextureAttribute.h++"

#define VS_TEXTURE_CUBE_SIDES 6

enum vsTextureCubeFace
{
    VS_TEXTURE_CUBE_POSITIVE_X,
    VS_TEXTURE_CUBE_NEGATIVE_X,
    VS_TEXTURE_CUBE_POSITIVE_Y,
    VS_TEXTURE_CUBE_NEGATIVE_Y,
    VS_TEXTURE_CUBE_POSITIVE_Z,
    VS_TEXTURE_CUBE_NEGATIVE_Z
};

class VESS_SYM vsTextureCubeAttribute : public vsStateAttribute
{
private:

    osg::TextureCubeMap    *osgTextureCube;
    osg::TexEnv            *osgTexEnv;
    osg::TexEnvCombine     *osgTexEnvCombine;
    osg::TexGen            *osgTexGen;
    osg::TexMat            *osgTexMat;
    osg::Image             *osgTexImage[VS_TEXTURE_CUBE_SIDES];

    unsigned int           textureUnit;

    virtual void           setOSGAttrModes(vsNode *node);

VS_INTERNAL:

                          vsTextureCubeAttribute(unsigned int unit,
                                       osg::TextureCubeMap *texObject,
                                       osg::TexEnv *texEnvObject,
                                       osg::TexEnvCombine *texEnvCombineObject,
                                       osg::TexGen *texGenObject,
                                       osg::TexMat *texMatObject);

    virtual void          attach(vsNode *node);
    virtual void          detach(vsNode *node);

    virtual void          attachDuplicate(vsNode *theNode);

    virtual bool          isEquivalent(vsAttribute *attribute);

    void                  setOSGImage(int face, osg::Image *osgImage);

    osg::TextureCubeMap   *getBaseLibraryObject();

public:

                           vsTextureCubeAttribute();
                           vsTextureCubeAttribute(unsigned int unit);
    virtual                ~vsTextureCubeAttribute();

    virtual const char     *getClassName();
    virtual int            getAttributeType();
    virtual vsAttribute    *clone();
    
    void                   setImage(int face, unsigned char *imageData,
                                    int xSize, int ySize, int dataFormat);
    void                   getImage(int face, unsigned char **imageData,
                                    int *xSize, int *ySize, int *dataFormat);
    
    void                   loadImageFromFile(int face, char *filename);

    void                   reloadTextureData(int face);

    bool                   isTransparent();

    void                   setBoundaryMode(int whichDirection,
                                           int boundaryMode);
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
