// File vsGraphicsState.h++

#ifndef VS_GRAPHICS_STATE_HPP
#define VS_GRAPHICS_STATE_HPP

#include "vsBackfaceAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsTransparencyAttribute.h++"

class vsGraphicsState
{
private:

    vsBackfaceAttribute        *currentBackface, *newBackface;
    vsFogAttribute             *currentFog, *newFog;
    vsMaterialAttribute        *currentMaterial, *newMaterial;
    vsShadingAttribute         *currentShading, *newShading;
    vsTextureAttribute         *currentTexture, *newTexture;
    vsTransparencyAttribute    *currentTransparency, *newTransparency;

public:

                  vsGraphicsState();
                  ~vsGraphicsState();

    void          clearState();
    void          applyState();

    void          setBackface(vsBackfaceAttribute *newAttrib);
    void          setFog(vsFogAttribute *newAttrib);
    void          setMaterial(vsMaterialAttribute *newAttrib);
    void          setShading(vsShadingAttribute *newAttrib);
    void          setTexture(vsTextureAttribute *newAttrib);
    void          setTransparency(vsTransparencyAttribute *newAttrib);

    vsBackfaceAttribute        *getBackface();
    vsFogAttribute             *getFog();
    vsMaterialAttribute        *getMaterial();
    vsShadingAttribute         *getShading();
    vsTextureAttribute         *getTexture();
    vsTransparencyAttribute    *getTransparency();

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
