// File vsMaterialAttribute.h++

#ifndef VS_MATERIAL_ATTRIBUTE_HPP
#define VS_MATERIAL_ATTRIBUTE_HPP

#include <Performer/pr/pfMaterial.h>
#include "vsAttribute.h++"
#include "vsGrowableArray.h++"

enum vsMaterialSide
{
    VS_MATERIAL_SIDE_FRONT,
    VS_MATERIAL_SIDE_BACK,
    VS_MATERIAL_SIDE_BOTH
};

enum vsMaterialColor
{
    VS_MATERIAL_COLOR_AMBIENT,
    VS_MATERIAL_COLOR_DIFFUSE,
    VS_MATERIAL_COLOR_SPECULAR,
    VS_MATERIAL_COLOR_EMISSIVE
};

enum vsMaterialColorMode
{
    VS_MATERIAL_CMODE_AMBIENT,
    VS_MATERIAL_CMODE_DIFFUSE,
    VS_MATERIAL_CMODE_SPECULAR,
    VS_MATERIAL_CMODE_EMISSIVE,
    VS_MATERIAL_CMODE_AMBIENT_DIFFUSE,
    VS_MATERIAL_CMODE_NONE
};

class vsMaterialAttribute : public vsAttribute
{
private:

    pfMaterial         *frontMaterial;
    pfMaterial         *backMaterial;
    
    vsGrowableArray    savedAttr;
    int                saveCount;

VS_INTERNAL:

                    vsMaterialAttribute(pfMaterial *front, pfMaterial *back);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState();

public:

                   vsMaterialAttribute();
                   ~vsMaterialAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();
    
    void           setColor(int side, int whichColor, double r, double g,
                            double b);
    void           getColor(int side, int whichColor, double *r, double *g,
                            double *b);

    void           setAlpha(int side, double alpha);
    double         getAlpha(int side);
    
    void           setShininess(int side, double shine);
    double         getShininess(int side);
    
    void           setColorMode(int side, int colorMode);
    int            getColorMode(int side);
};

#endif
