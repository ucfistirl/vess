// File vsGraphicsState.h++

#ifndef VS_GRAPHICS_STATE_HPP
#define VS_GRAPHICS_STATE_HPP

#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsLightAttribute.h++"
#include "vsGrowableArray.h++"

class vsGraphicsState
{
private:

    vsBackfaceAttribute        *backfaceAttr;
    vsFogAttribute             *fogAttr;
    vsMaterialAttribute        *materialAttr;
    vsShadingAttribute         *shadingAttr;
    vsTextureAttribute         *textureAttr;
    vsTransparencyAttribute    *transparencyAttr;
    
    vsGrowableArray            lightAttrList;
    int                        lightAttrCount;

VS_INTERNAL:

    void        applyState(pfGeoState *state);

public:

                  vsGraphicsState();
                  ~vsGraphicsState();

    void          clearState();

    void          setBackface(vsBackfaceAttribute *newAttrib);
    void          setFog(vsFogAttribute *newAttrib);
    void          setMaterial(vsMaterialAttribute *newAttrib);
    void          setShading(vsShadingAttribute *newAttrib);
    void          setTexture(vsTextureAttribute *newAttrib);
    void          setTransparency(vsTransparencyAttribute *newAttrib);
    
    void          addLight(vsLightAttribute *lightAttrib);
    void          removeLight(vsLightAttribute *lightAttrib);

    vsBackfaceAttribute        *getBackface();
    vsFogAttribute             *getFog();
    vsMaterialAttribute        *getMaterial();
    vsShadingAttribute         *getShading();
    vsTextureAttribute         *getTexture();
    vsTransparencyAttribute    *getTransparency();
    
    vsLightAttribute           *getLight(int index);
    int                        getLightCount();

    static int    isSameBackface(vsAttribute *firstAttr,
                                 vsAttribute *secondAttr);
    static int    isSameFog(vsAttribute *firstAttr, vsAttribute *secondAttr);
    static int    isSameMaterial(vsAttribute *firstAttr,
                                 vsAttribute *secondAttr);
    static int    isSameShading(vsAttribute *firstAttr,
                                vsAttribute *secondAttr);
    static int    isSameTexture(vsAttribute *firstAttr,
                                vsAttribute *secondAttr);
    static int    isSameTransparency(vsAttribute *firstAttr,
                                     vsAttribute *secondAttr);
};

#endif
