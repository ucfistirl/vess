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
//    VESS Module:  vsTextureAttribute.h++
//
//    Description:  Attribute that specifies which texture should be used
//                  to cover geometry
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_TEXTURE_ATTRIBUTE_HPP
#define VS_TEXTURE_ATTRIBUTE_HPP

#include <Performer/pr/pfTexture.h>
#include "vsStateAttribute.h++"

enum VS_GRAPHICS_DLL vsTextureDirection
{
    VS_TEXTURE_DIRECTION_S,
    VS_TEXTURE_DIRECTION_T,
    VS_TEXTURE_DIRECTION_ALL
};

enum VS_GRAPHICS_DLL vsTextureBoundaryMode
{
    VS_TEXTURE_BOUNDARY_REPEAT,
    VS_TEXTURE_BOUNDARY_CLAMP
};

enum VS_GRAPHICS_DLL vsTextureApplyMode
{
    VS_TEXTURE_APPLY_DECAL,
    VS_TEXTURE_APPLY_MODULATE,
    VS_TEXTURE_APPLY_REPLACE
};

enum VS_GRAPHICS_DLL vsTextureDataFormat
{
    VS_TEXTURE_DFORMAT_INTENSITY,
    VS_TEXTURE_DFORMAT_INTENSITY_ALPHA,
    VS_TEXTURE_DFORMAT_RGB,
    VS_TEXTURE_DFORMAT_RGBA
};

enum VS_GRAPHICS_DLL vsTextureMagnificationFilter
{
    VS_TEXTURE_MAGFILTER_NEAREST,
    VS_TEXTURE_MAGFILTER_LINEAR
};

enum VS_GRAPHICS_DLL vsTextureMinificationFilter
{
    VS_TEXTURE_MINFILTER_NEAREST,
    VS_TEXTURE_MINFILTER_LINEAR,
    VS_TEXTURE_MINFILTER_MIPMAP_NEAREST,
    VS_TEXTURE_MINFILTER_MIPMAP_LINEAR
};

enum VS_GRAPHICS_DLL vsTextureGenMode
{
    VS_TEXTURE_GEN_OBJECT_LINEAR,
    VS_TEXTURE_GEN_EYE_LINEAR,
    VS_TEXTURE_GEN_SPHERE_MAP,
    VS_TEXTURE_GEN_NORMAL_MAP,
    VS_TEXTURE_GEN_REFLECTION_MAP,
    VS_TEXTURE_GEN_OFF
};

class VS_GRAPHICS_DLL vsTextureAttribute : public vsStateAttribute
{
private:

    pfTexture     *performerTexture;
    pfTexEnv      *performerTexEnv;
    pfTexGen      *performerTexGen;
    unsigned int  textureUnit;

VS_INTERNAL:

                    vsTextureAttribute(unsigned int unit,
                                       pfTexture *texObject,
                                       pfTexEnv *texEnvObject, 
                                       pfTexGen *texGenObject);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                          vsTextureAttribute();
                          vsTextureAttribute(unsigned int unit);
    virtual               ~vsTextureAttribute();

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

    unsigned int          getTextureUnit();

};

#endif
