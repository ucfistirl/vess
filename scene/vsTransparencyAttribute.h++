// File vsTransparencyAttribute.h++

#ifndef VS_TRANSPARENCY_ATTRIBUTE_HPP
#define VS_TRANSPARENCY_ATTRIBUTE_HPP

#include "vsAttribute.h++"
#include "vsGrowableArray.h++"

class vsTransparencyAttribute : public vsAttribute
{
private:

    int                transpValue;

    vsGrowableArray    savedAttr;
    int                saveCount;

VS_INTERNAL:

                    vsTransparencyAttribute(int type);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState();

    static void     setDefault();

public:

                   vsTransparencyAttribute();
                   ~vsTransparencyAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
