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
//    VESS Module:  vsGraphicsState.h++
//
//    Description:  Object used internally by VESS to track the current
//                  graphics state during a scene graph traversal
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_GRAPHICS_STATE_HPP
#define VS_GRAPHICS_STATE_HPP

#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShaderAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include "vsTextureRectangleAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsLightAttribute.h++"
#include "vsGrowableArray.h++"
#include "vsGeometry.h++"

class VS_GRAPHICS_DLL vsGraphicsState : public vsObject
{
private:

    static vsGraphicsState         *classInstance;

    vsBackfaceAttribute            *backfaceAttr;
    vsFogAttribute                 *fogAttr;
    vsMaterialAttribute            *materialAttr;
    vsShaderAttribute              *shaderAttr;
    vsShadingAttribute             *shadingAttr;
    vsTextureAttribute             *textureAttr[VS_MAXIMUM_TEXTURE_UNITS];
    vsTextureCubeAttribute         *textureCubeAttr[VS_MAXIMUM_TEXTURE_UNITS];
    vsTextureRectangleAttribute    *textureRectAttr[VS_MAXIMUM_TEXTURE_UNITS];
    vsTransparencyAttribute        *transparencyAttr;
    vsWireframeAttribute           *wireframeAttr;
    
    vsGrowableArray            lightAttrList;
    int                        lightAttrCount;
    
    void                       *backfaceLock;
    void                       *fogLock;
    void                       *materialLock;
    void                       *shaderLock;
    void                       *shadingLock;
    void                       *textureLock[VS_MAXIMUM_TEXTURE_UNITS];
    void                       *transparencyLock;
    void                       *wireframeLock;

                               vsGraphicsState();

VS_INTERNAL:

    virtual        ~vsGraphicsState();

    static void    deleteInstance();

    void           applyState(pfGeoState *state);

public:

    virtual const char     *getClassName();

    static vsGraphicsState *getInstance();

    void          clearState();

    void          setBackface(vsBackfaceAttribute *newAttrib);
    void          setFog(vsFogAttribute *newAttrib);
    void          setMaterial(vsMaterialAttribute *newAttrib);
    void          setShader(vsShaderAttribute *newAttrib);
    void          setShading(vsShadingAttribute *newAttrib);
    void          setTexture(unsigned int unit, vsTextureAttribute *newAttrib);
    void          setTextureCube(unsigned int unit,
                                 vsTextureCubeAttribute *newAttrib);
    void          setTextureRect(unsigned int unit,
                                 vsTextureRectangleAttribute *newAttrib);
    void          setTransparency(vsTransparencyAttribute *newAttrib);
    void          setWireframe(vsWireframeAttribute *newAttrib);
 
    void          addLight(vsLightAttribute *lightAttrib);
    void          removeLight(vsLightAttribute *lightAttrib);

    vsBackfaceAttribute            *getBackface();
    vsFogAttribute                 *getFog();
    vsMaterialAttribute            *getMaterial();
    vsShaderAttribute              *getShader();
    vsShadingAttribute             *getShading();
    vsTextureAttribute             *getTexture(unsigned int unit);
    vsTextureCubeAttribute         *getTextureCube(unsigned int unit);
    vsTextureRectangleAttribute    *getTextureRect(unsigned int unit);
    vsTransparencyAttribute        *getTransparency();
    vsWireframeAttribute           *getWireframe();

    vsLightAttribute           *getLight(int index);
    int                        getLightCount();
    
    void          lockBackface(void *lockAddr);
    void          lockFog(void *lockAddr);
    void          lockMaterial(void *lockAddr);
    void          lockShader(void *lockAddr);
    void          lockShading(void *lockAddr);
    void          lockTexture(unsigned int unit, void *lockAddr);
    void          lockTransparency(void *lockAddr);
    void          lockWireframe(void *lockAddr);

    void          unlockBackface(void *lockAddr);
    void          unlockFog(void *lockAddr);
    void          unlockMaterial(void *lockAddr);
    void          unlockShader(void *lockAddr);
    void          unlockShading(void *lockAddr);
    void          unlockTexture(unsigned int unit, void *lockAddr);
    void          unlockTransparency(void *lockAddr);
    void          unlockWireframe(void *lockAddr);
};

#endif
