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

#include <osg/ShadeModel>
#include "vsStateAttribute.h++"

enum vsShadingMode
{
    VS_SHADING_GOURAUD,
    VS_SHADING_FLAT
};

class vsShadingAttribute : public vsStateAttribute
{
private:

    osg::ShadeModel    *shadeModel;

    virtual void       setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual int     isEquivalent(vsAttribute *attribute);

public:

                          vsShadingAttribute();
    virtual               ~vsShadingAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();

    void                  setShading(int shadingMode);
    int                   getShading();
};

#endif
