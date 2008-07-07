#ifndef VS_COLLADA_FIXED_EFFECT_HPP
#define VS_COLLADA_FIXED_EFFECT_HPP

#include "atMap.h++"
#include "vsCOLLADAEffect.h++"
#include "vsMaterialAttribute.h++"
#include "vsTextureAttribute.h++"

class vsCOLLADAFixedEffect : public vsCOLLADAEffect
{
protected:

    vsMaterialAttribute    *material;
    atMap                  *textures;

public:

                                    vsCOLLADAFixedEffect(atString id);
                                    ~vsCOLLADAFixedEffect();

    virtual const char              *getClassName();

    virtual vsCOLLADAFixedEffect    *clone(atString cloneID);

    virtual vsCOLLADAEffectType     getType();

    virtual void                    setMaterial(vsMaterialAttribute *mat);
    virtual vsMaterialAttribute     *getMaterial();
    virtual void                    addTexture(atString texCoordName,
                                               vsTextureAttribute *tex);
    virtual atList                  *getTextures(atString texCoordName);
    virtual vsTextureAttribute      *getTextureFromParam(atString paramID);
};

#endif
