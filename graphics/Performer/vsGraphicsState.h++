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
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsLightAttribute.h++"
#include "vsGrowableArray.h++"

class vsGraphicsState
{
private:

    static vsGraphicsState     *classInstance;

    vsBackfaceAttribute        *backfaceAttr;
    vsFogAttribute             *fogAttr;
    vsMaterialAttribute        *materialAttr;
    vsShadingAttribute         *shadingAttr;
    vsTextureAttribute         *textureAttr;
    vsTransparencyAttribute    *transparencyAttr;
    vsWireframeAttribute       *wireframeAttr;
    
    vsGrowableArray            lightAttrList;
    int                        lightAttrCount;
    
    void                       *backfaceLock;
    void                       *fogLock;
    void                       *materialLock;
    void                       *shadingLock;
    void                       *textureLock;
    void                       *transparencyLock;
    void                       *wireframeLock;

                               vsGraphicsState();

VS_INTERNAL:

    void        applyState(pfGeoState *state);

public:

    static vsGraphicsState *getInstance();

    void          clearState();

    void          setBackface(vsBackfaceAttribute *newAttrib);
    void          setFog(vsFogAttribute *newAttrib);
    void          setMaterial(vsMaterialAttribute *newAttrib);
    void          setShading(vsShadingAttribute *newAttrib);
    void          setTexture(vsTextureAttribute *newAttrib);
    void          setTransparency(vsTransparencyAttribute *newAttrib);
    void          setWireframe(vsWireframeAttribute *newAttrib);
    
    void          addLight(vsLightAttribute *lightAttrib);
    void          removeLight(vsLightAttribute *lightAttrib);

    vsBackfaceAttribute        *getBackface();
    vsFogAttribute             *getFog();
    vsMaterialAttribute        *getMaterial();
    vsShadingAttribute         *getShading();
    vsTextureAttribute         *getTexture();
    vsTransparencyAttribute    *getTransparency();
    vsWireframeAttribute       *getWireframe();
    
    vsLightAttribute           *getLight(int index);
    int                        getLightCount();
    
    void          lockBackface(void *lockAddr);
    void          lockFog(void *lockAddr);
    void          lockMaterial(void *lockAddr);
    void          lockShading(void *lockAddr);
    void          lockTexture(void *lockAddr);
    void          lockTransparency(void *lockAddr);
    void          lockWireframe(void *lockAddr);

    void          unlockBackface(void *lockAddr);
    void          unlockFog(void *lockAddr);
    void          unlockMaterial(void *lockAddr);
    void          unlockShading(void *lockAddr);
    void          unlockTexture(void *lockAddr);
    void          unlockTransparency(void *lockAddr);
    void          unlockWireframe(void *lockAddr);
};

#endif
