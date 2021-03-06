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
//    VESS Module:  vsBackfaceAttribute.h++
//
//    Description:  Attribute for specifying the visibility of back-facing
//                  geometry
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_BACKFACE_ATTRIBUTE_HPP
#define VS_BACKFACE_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"
#include <osg/LightModel>
#include <osg/CullFace>

class VESS_SYM vsBackfaceAttribute : public vsStateAttribute
{
private:

    osg::LightModel    *lightModel;
    osg::CullFace      *cullFace;

    bool               backfaceEnabled;
    
    virtual void       setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                           vsBackfaceAttribute();
    virtual                ~vsBackfaceAttribute();

    virtual const char     *getClassName();
    virtual int            getAttributeType();
    virtual vsAttribute    *clone();

    void                   enable();
    void                   disable();
    bool                   isEnabled();
};

#endif
