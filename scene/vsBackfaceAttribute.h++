// File vsBackfaceAttribute.h++

#ifndef VS_BACKFACE_ATTRIBUTE_HPP
#define VS_BACKFACE_ATTRIBUTE_HPP

#include "vsGrowableArray.h++"
#include "vsAttribute.h++"

class vsBackfaceAttribute : public vsAttribute
{
private:

    int                backfaceVal;
    
    vsGrowableArray    backAttrSave;
    int                saveCount;

VS_INTERNAL:

                    vsBackfaceAttribute(int initVal);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState();

    static void     setDefault();

public:

                   vsBackfaceAttribute();
                   ~vsBackfaceAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
