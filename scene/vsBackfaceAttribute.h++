// File vsBackfaceAttribute.h++

#ifndef VS_BACKFACE_ATTRIBUTE_HPP
#define VS_BACKFACE_ATTRIBUTE_HPP

#include <Performer/pr/pfLight.h>
#include <Performer/pr.h>
#include "vsAttribute.h++"

class vsBackfaceAttribute : public vsAttribute
{
private:

    int         backfaceVal;
    
    int         backCullSave, backLightSave;

VS_INTERNAL:

                    vsBackfaceAttribute(int initVal);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();

public:

                   vsBackfaceAttribute();
                   ~vsBackfaceAttribute();

    virtual int    getAttributeType();

    void           enable();
    void           disable();
    int            isEnabled();
};

#endif
