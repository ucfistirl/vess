// File vsBackfaceAttribute.h++

#ifndef VS_BACKFACE_ATTRIBUTE_HPP
#define VS_BACKFACE_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

class vsBackfaceAttribute : public vsStateAttribute
{
private:

    pfLightModel       *lightModel;
    int                cullfaceVal;

VS_INTERNAL:

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsBackfaceAttribute();
                   ~vsBackfaceAttribute();

    virtual int    getAttributeType();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
