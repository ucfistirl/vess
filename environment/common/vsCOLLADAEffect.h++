#ifndef VS_COLLADA_EFFECT_HPP
#define VS_COLLADA_EFFECT_HPP

#include "atString.h++"
#include "atList.h++"
#include "vsCOLLADAEffectParameter.h++"


enum vsCOLLADAEffectType
{
    VS_COLLADA_EFFECT_FIXED,
    VS_COLLADA_EFFECT_GLSL
};


class VESS_SYM vsCOLLADAEffect : public vsObject
{
protected:

    atString    effectID;
    atList      *effectParameters;

public:

                                        vsCOLLADAEffect(atString id);
                                        ~vsCOLLADAEffect();

    virtual vsCOLLADAEffect             *clone(atString cloneID) = 0;

    virtual atString                    getID();
    virtual vsCOLLADAEffectType         getType() = 0;

    virtual void                        addParameter(
                                              vsCOLLADAEffectParameter *param);
    virtual vsCOLLADAEffectParameter    *getParameter(atString name);
    virtual vsCOLLADAEffectParameter    *getParameter(u_long index);
};

#endif

