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
//    VESS Module:  vsWireframeAttribute.h++
//
//    Description:  Attribute that specifies that geometry should be drawn
//                  in wireframe mode rather than filled
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_WIREFRAME_ATTRIBUTE_HPP
#define VS_WIREFRAME_ATTRIBUTE_HPP

#include <osg/PolygonMode>
#include "vsStateAttribute.h++"

class VS_GRAPHICS_DLL vsWireframeAttribute : public vsStateAttribute
{
private:

    osg::PolygonMode    *osgPolyMode;

    virtual void        setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual int     isEquivalent(vsAttribute *attribute);

public:

                          vsWireframeAttribute();
    virtual               ~vsWireframeAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();

    void                  enable();
    void                  disable();
    int                   isEnabled();
    
};

#endif
