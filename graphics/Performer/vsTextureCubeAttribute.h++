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
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_TEXTURE_CUBE_ATTRIBUTE_HPP
#define VS_TEXTURE_CUBE_ATTRIBUTE_HPP

#include <Performer/pr/pfTexture.h>
#include "vsStateAttribute.h++"
#include "vsTextureAttribute.h++"

#define VS_TEXTURE_CUBE_SIDES 6

enum VS_GRAPHICS_DLL vsTextureCubeFace
{
    VS_TEXTURE_CUBE_POSITIVE_X,
    VS_TEXTURE_CUBE_NEGATIVE_X,
    VS_TEXTURE_CUBE_POSITIVE_Y,
    VS_TEXTURE_CUBE_NEGATIVE_Y,
    VS_TEXTURE_CUBE_POSITIVE_Z,
    VS_TEXTURE_CUBE_NEGATIVE_Z
};
                                                                                
enum VS_GRAPHICS_DLL vsTextureGenMode
{
    VS_TEXTURE_GEN_OBJECT_LINEAR,
    VS_TEXTURE_GEN_EYE_LINEAR,
    VS_TEXTURE_GEN_SPHERE_MAP,
    VS_TEXTURE_GEN_NORMAL_MAP,
    VS_TEXTURE_GEN_REFLECTION_MAP
};

class VS_GRAPHICS_DLL vsTextureCubeAttribute : public vsStateAttribute
{
private:

    pfTexture    *performerTexture;
    pfTexEnv     *performerTexEnv;
    pfTexGen     *performerTexGen;

VS_INTERNAL:

                    vsTextureCubeAttribute(pfTexture *texObject,
                                           pfTexEnv *texEnvObject,
                                           pfTexGen *texGenObject);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                          vsTextureCubeAttribute();
    virtual               ~vsTextureCubeAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    
    void                  setImage(int face, unsigned char *imageData,
                                   int xSize, int ySize, int dataFormat);
    void                  getImage(int face, unsigned char **imageData,
                                   int *xSize, int *ySize, int *dataFormat);
    
    void                  loadImageFromFile(int face, char *filename);

    void                  reloadTextureData(int face);

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
};

#endif
