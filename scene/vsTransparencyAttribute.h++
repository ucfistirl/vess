// File vsTransparencyAttribute.h++

#ifndef VS_TRANSPARENCY_ATTRIBUTE_HPP
#define VS_TRANSPARENCY_ATTRIBUTE_HPP

#include <Performer/pr.h>
#include "vsAttribute.h++"

class vsTransparencyAttribute : public vsAttribute
{
private:

    int         transpValue;
    int         savedValue;

VS_INTERNAL:

                    vsTransparencyAttribute(int type);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();

public:

                   vsTransparencyAttribute();
                   ~vsTransparencyAttribute();

    virtual int    getAttributeType();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
