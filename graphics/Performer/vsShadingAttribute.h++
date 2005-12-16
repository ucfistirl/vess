//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsShadingAttribute.h++
//
//    Description:  Attribute that specifies the shading model used for
//                  the geometry
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SHADING_ATTRIBUTE_HPP
#define VS_SHADING_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"

enum vsShadingMode
{
    VS_SHADING_GOURAUD,
    VS_SHADING_FLAT
};

class VS_GRAPHICS_DLL vsShadingAttribute : public vsStateAttribute
{
private:

    int         shadeVal;

VS_INTERNAL:

    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                          vsShadingAttribute();
    virtual               ~vsShadingAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    
    void                  setShading(int shadingMode);
    int                   getShading();
};

#endif
