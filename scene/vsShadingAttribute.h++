// File vsShadingAttribute.h++

#ifndef VS_SHADING_ATTRIBUTE_HPP
#define VS_SHADING_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

enum vsShadingMode
{
    VS_SHADING_GOURAUD,
    VS_SHADING_FLAT
};

class vsShadingAttribute : public vsStateAttribute
{
private:

    int         shadeVal;

VS_INTERNAL:

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsShadingAttribute();
                   ~vsShadingAttribute();

    virtual int    getAttributeType();
    
    void           setShading(int shadingMode);
    int            getShading();
};

#endif
