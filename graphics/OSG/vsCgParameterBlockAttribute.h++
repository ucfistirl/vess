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
//    VESS Module:  vsCgParameterBlockAttribute.h++
//
//    Description:  Attribute to contain CgParameters that are applied to
//                  the scene from where this attribute is attached down.
//                  It correlates to the osgNV::ParameterBlock.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CG_PARAMETER_BLOCK_ATTRIBUTE_HPP
#define VS_CG_PARAMETER_BLOCK_ATTRIBUTE_HPP

#include <osgNV/ParameterBlock>
#include "vsStateAttribute.h++"
#include "vsCgParameter.h++"
#include "vsGrowableArray.h++"

class VS_GRAPHICS_DLL vsCgParameterBlockAttribute : public vsStateAttribute
{
private:

    osgNV::ParameterBlock    *parameterBlock;

    vsGrowableArray          *parameterArray;
    int                      parameterCount;

    virtual void       setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:

                          vsCgParameterBlockAttribute();
    virtual               ~vsCgParameterBlockAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();

    void                  addCgParameter(vsCgParameter *parameter);
    void                  removeCgParameter(vsCgParameter *parameter);
    vsCgParameter         *getCgParameter(int index);
    int                   getCgParameterCount();
};

#endif
