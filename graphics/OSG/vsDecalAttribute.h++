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
//    VESS Module:  vsDecalAttribute.h++
//
//    Description:  Attribute that specifies that the children of the
//                  component be drawn with different depth offsets in
//                  order to reduce z-fighting
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_DECAL_ATTRIBUTE_HPP
#define VS_DECAL_ATTRIBUTE_HPP

#include <osg/Group>
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsDecalCallback;

class vsDecalAttribute : public vsAttribute
{
private:

    vsDecalCallback    *decalCallback;
    osg::Group         *bottomGroup;

VS_INTERNAL:

    virtual int     canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

public:

                          vsDecalAttribute();
    virtual               ~vsDecalAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();
};

#endif
