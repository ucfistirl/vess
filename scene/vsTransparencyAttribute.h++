// File vsTransparencyAttribute.h++

#ifndef VS_TRANSPARENCY_ATTRIBUTE_HPP
#define VS_TRANSPARENCY_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

class vsTransparencyAttribute : public vsStateAttribute
{
private:

    int         transpValue;

VS_INTERNAL:

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsTransparencyAttribute();
    virtual        ~vsTransparencyAttribute();

    virtual int    getAttributeType();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
