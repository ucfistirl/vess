// File vsShadingAttribute.h++

#ifndef VS_SHADING_ATTRIBUTE_HPP
#define VS_SHADING_ATTRIBUTE_HPP

#include "vsGrowableArray.h++"
#include "vsAttribute.h++"

enum vsShadingMode
{
    VS_SHADING_GOURAUD,
    VS_SHADING_FLAT
};

class vsShadingAttribute : public vsAttribute
{
private:

    int                shadeVal;
    
    vsGrowableArray    shadeAttrSave;
    int                saveCount;

VS_INTERNAL:

                    vsShadingAttribute(int performerShading);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState();
    
    static void     setDefault();

public:

                   vsShadingAttribute();
                   ~vsShadingAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();
    
    void           setShading(int shadingMode);
    int            getShading();
};

#endif
